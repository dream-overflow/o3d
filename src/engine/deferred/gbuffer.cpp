/**
 * @file gbuffer.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/gl.h"

#include "o3d/engine/deferred/gbuffer.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/texture2dms.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/renderer.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(GBuffer, ENGINE_GBUFFER, SceneEntity)

PixelFormat GBuffer::bufferFormatToPixelFormat(BufferType buffer, BufferFormat format) const
{
    switch (buffer) {
        case AMBIENT_BUFFER:
        case DIFFUSE_BUFFER:
        case SPECULAR_BUFFER:
            if (format == FORMAT_8UI) {
                return PF_RGBA_U8;
            } else if (format == FORMAT_16F) {
                return PF_RGBA_F16;
            } else if (format == FORMAT_32F) {
                return PF_RGBA_F32;
            }
            break;

        case POSITION_BUFFER:
            if (format == FORMAT_8UI) {
                return PF_RGB_U8;
            } else if (format == FORMAT_16F) {
                return PF_RGB_F16;
            } else if (format == FORMAT_32F) {
                return PF_RGB_F32;
            }
            break;

        case NORMAL_BUFFER:
            if (format == FORMAT_8UI) {
                return PF_RGBA_U8;
            } else if (format == FORMAT_16F) {
                return PF_RGBA_F16;
            } else if (format == FORMAT_32F) {
                return PF_RGBA_F32;
            }
            break;

        case DEPTH_BUFFER:
            if (format == FORMAT_8UI) {
                return PF_DEPTH;
            } else if (format == FORMAT_16F) {
                return PF_DEPTH;//_F16; @todo
            } else if (format == FORMAT_32F) {
                return PF_DEPTH_F32;
            } else if (format == FORMAT_24_8) {
                return PF_DEPTH_U24_STENCIL_U8;
            } else if (format == FORMAT_32F_8) {
                return PF_DEPTH_F32_STENCIL_U8;
            }

        default:
            break;
    }

    return PF_RGBA_U8;
}

GBuffer::GBuffer(BaseObject *parent) :
	SceneEntity(parent),
    m_fbo(getScene()->getContext()),
    m_drawBuffers(getScene()->getContext())
{
    setProfile(PROFILE_MEDIUM);

    for (UInt32 i = 0; i < NUM_BUFFERS_TYPE; ++i) {
        m_buffers[i].texture.setUser(this);
    }
}

GBuffer::~GBuffer()
{
    release();
}

void GBuffer::setFormat(GBuffer::BufferType buffer, GBuffer::BufferFormat format)
{
    if (m_fbo.isValid()) {
        O3D_ERROR(E_InvalidOperation("GBuffer exists. This operation can only be done before its creation."));
    }

    if (buffer >= 0 && buffer < NUM_BUFFERS_TYPE) {
        if (buffer == DEPTH_BUFFER) {
            switch(format) {
                case FORMAT_8:
                case FORMAT_8I:
                case FORMAT_8UI:
                case FORMAT_16I:
                case FORMAT_16UI:
                case FORMAT_32I:
                case FORMAT_32UI:
                    O3D_ERROR(E_InvalidOperation("This format is not allowed on depth buffer."));
                    break;

                case FORMAT_16F:
                case FORMAT_24:
                case FORMAT_32F:
                case FORMAT_24_8:
                case FORMAT_32F_8:
                    break;
            }
        } else {
            switch(format) {
                case FORMAT_8:
                case FORMAT_8I:
                case FORMAT_8UI:
                case FORMAT_16I:
                case FORMAT_16UI:
                case FORMAT_32I:
                case FORMAT_32UI:
                    O3D_ERROR(E_InvalidOperation("This format is not allowed on depth buffer."));
                    break;

                case FORMAT_16F:
                case FORMAT_24:
                case FORMAT_32F:
                case FORMAT_24_8:
                case FORMAT_32F_8:
                    O3D_ERROR(E_InvalidOperation("This format is not allowed on color buffer."));
                    break;
            }
        }

        // @todo MSAA format
        m_buffers[buffer].format = format;
    } else {
        O3D_ERROR(E_IndexOutOfRange("Buffer format"));
    }
}

GBuffer::BufferFormat GBuffer::getFormat(GBuffer::BufferType buffer) const
{
    if (buffer >= 0 && buffer < NUM_BUFFERS_TYPE) {
        return m_buffers[buffer].format;
    } else {
        O3D_ERROR(E_IndexOutOfRange("Buffer format"));
    }
}

void GBuffer::setProfile(GBuffer::Profiles profile)
{
    if (GL::getType() == GL::API_CUSTOM || GL::getType() == GL::API_GL) {
        if (getScene()->getRenderer()->getVersion() >= 400) {
            m_buffers[COLOR_BUFFER].type = COLOR_BUFFER;
            m_buffers[COLOR_BUFFER].actif = True;
            if (profile == PROFILE_LOW) {
                m_buffers[COLOR_BUFFER].format = FORMAT_8;
            } else {
                m_buffers[COLOR_BUFFER].format = FORMAT_16F;
            }

            m_buffers[AUX_COLOR_BUFFER].type = AUX_COLOR_BUFFER;
            m_buffers[AUX_COLOR_BUFFER].actif = False;
            if (profile == PROFILE_LOW) {
                m_buffers[AUX_COLOR_BUFFER].format = FORMAT_8;
            } else {
                m_buffers[AUX_COLOR_BUFFER].format = FORMAT_16F;
            }

            m_buffers[AMBIENT_BUFFER].type = AMBIENT_BUFFER;
            m_buffers[AMBIENT_BUFFER].actif = True;
            m_buffers[AMBIENT_BUFFER].format = FORMAT_8UI;

            m_buffers[DIFFUSE_BUFFER].type = DIFFUSE_BUFFER;
            m_buffers[DIFFUSE_BUFFER].actif = True;
            m_buffers[DIFFUSE_BUFFER].format = FORMAT_8UI;

            m_buffers[SPECULAR_BUFFER].type = SPECULAR_BUFFER;
            m_buffers[SPECULAR_BUFFER].actif = True;
            m_buffers[SPECULAR_BUFFER].format = FORMAT_8UI;

            m_buffers[NORMAL_BUFFER].type = NORMAL_BUFFER;
            m_buffers[NORMAL_BUFFER].actif = True;
            m_buffers[NORMAL_BUFFER].format = FORMAT_16F;

            m_buffers[DEPTH_BUFFER].type = DEPTH_BUFFER;
            m_buffers[DEPTH_BUFFER].actif = True;
            if (profile == PROFILE_HIGH) {
                m_buffers[DEPTH_BUFFER].format = FORMAT_32F_8;
            } else {
                m_buffers[DEPTH_BUFFER].format = FORMAT_24_8;
            }

            m_buffers[POSITION_BUFFER].type = POSITION_BUFFER;
            m_buffers[POSITION_BUFFER].actif = True;
            if (profile == PROFILE_LOW) {
                m_buffers[POSITION_BUFFER].format = FORMAT_16F;
            } else {
                m_buffers[POSITION_BUFFER].format = FORMAT_32F;
            }

            return;
        } else if (getScene()->getRenderer()->getVersion() >= 300) {
            m_buffers[COLOR_BUFFER].type = COLOR_BUFFER;
            m_buffers[COLOR_BUFFER].actif = True;
            if (profile == PROFILE_LOW) {
                m_buffers[COLOR_BUFFER].format = FORMAT_8;
            } else {
                m_buffers[COLOR_BUFFER].format = FORMAT_16F;
            }

            m_buffers[AUX_COLOR_BUFFER].type = AUX_COLOR_BUFFER;
            m_buffers[AUX_COLOR_BUFFER].actif = False;
            if (profile == PROFILE_LOW) {
                m_buffers[AUX_COLOR_BUFFER].format = FORMAT_8;
            } else {
                m_buffers[AUX_COLOR_BUFFER].format = FORMAT_16F;
            }

            m_buffers[AMBIENT_BUFFER].type = AMBIENT_BUFFER;
            m_buffers[AMBIENT_BUFFER].actif = True;
            m_buffers[AMBIENT_BUFFER].format = FORMAT_8UI;

            m_buffers[DIFFUSE_BUFFER].type = DIFFUSE_BUFFER;
            m_buffers[DIFFUSE_BUFFER].actif = True;
            m_buffers[DIFFUSE_BUFFER].format = FORMAT_8UI;

            m_buffers[SPECULAR_BUFFER].type = SPECULAR_BUFFER;
            m_buffers[SPECULAR_BUFFER].actif = True;
            m_buffers[SPECULAR_BUFFER].format = FORMAT_8UI;

            m_buffers[NORMAL_BUFFER].type = NORMAL_BUFFER;
            m_buffers[NORMAL_BUFFER].actif = True;
            m_buffers[NORMAL_BUFFER].format = FORMAT_16F;

            m_buffers[DEPTH_BUFFER].type = DEPTH_BUFFER;
            m_buffers[DEPTH_BUFFER].actif = True;
            if (profile == PROFILE_HIGH) {
                m_buffers[DEPTH_BUFFER].format = FORMAT_32F_8;
            } else {
                m_buffers[DEPTH_BUFFER].format = FORMAT_24_8;
            }

            m_buffers[POSITION_BUFFER].type = POSITION_BUFFER;
            m_buffers[POSITION_BUFFER].actif = True;
            if (profile == PROFILE_LOW) {
                m_buffers[POSITION_BUFFER].format = FORMAT_16F;
            } else {
                m_buffers[POSITION_BUFFER].format = FORMAT_32F;
            }

            return;
        }
    } else if (GL::getType() == GL::API_GLES_3) {
        if (getScene()->getRenderer()->getVersion() >= 300) {
            m_buffers[COLOR_BUFFER].type = COLOR_BUFFER;
            m_buffers[COLOR_BUFFER].actif = True;
            if (profile == PROFILE_LOW) {
                m_buffers[COLOR_BUFFER].format = FORMAT_8;
            } else {
                m_buffers[COLOR_BUFFER].format = FORMAT_16F;
            }

            m_buffers[AUX_COLOR_BUFFER].type = AUX_COLOR_BUFFER;
            m_buffers[AUX_COLOR_BUFFER].actif = False;
            if (profile == PROFILE_LOW) {
                m_buffers[AUX_COLOR_BUFFER].format = FORMAT_8;
            } else {
                m_buffers[AUX_COLOR_BUFFER].format = FORMAT_16F;
            }

            m_buffers[AMBIENT_BUFFER].type = AMBIENT_BUFFER;
            m_buffers[AMBIENT_BUFFER].actif = True;
            m_buffers[AMBIENT_BUFFER].format = FORMAT_8UI;

            m_buffers[DIFFUSE_BUFFER].type = DIFFUSE_BUFFER;
            m_buffers[DIFFUSE_BUFFER].actif = True;
            m_buffers[DIFFUSE_BUFFER].format = FORMAT_8UI;

            m_buffers[SPECULAR_BUFFER].type = SPECULAR_BUFFER;
            m_buffers[SPECULAR_BUFFER].actif = True;
            m_buffers[SPECULAR_BUFFER].format = FORMAT_8UI;

            m_buffers[NORMAL_BUFFER].type = NORMAL_BUFFER;
            m_buffers[NORMAL_BUFFER].actif = True;
            m_buffers[NORMAL_BUFFER].format = FORMAT_16F;

            m_buffers[DEPTH_BUFFER].type = DEPTH_BUFFER;
            m_buffers[DEPTH_BUFFER].actif = True;
            if (profile == PROFILE_HIGH) {
                m_buffers[DEPTH_BUFFER].format = FORMAT_32F_8;
            } else {
                m_buffers[DEPTH_BUFFER].format = FORMAT_24_8;
            }

            m_buffers[POSITION_BUFFER].type = POSITION_BUFFER;
            m_buffers[POSITION_BUFFER].actif = True;
            if (profile == PROFILE_HIGH) {
                m_buffers[POSITION_BUFFER].format = FORMAT_32F;
            } else {
                m_buffers[POSITION_BUFFER].format = FORMAT_16F;
            }

            return;
        }
    }

    O3D_ERROR(E_InvalidPrecondition("OpenGL version cannot be found or is not supported"));
}

void GBuffer::setBufferUsage(GBuffer::BufferType buffer, Bool state)
{
    if (m_fbo.isValid()) {
        O3D_ERROR(E_InvalidOperation("GBuffer exists. This operation can only be done before its creation."));
    }

    if (buffer >= 0 && buffer < NUM_BUFFERS_TYPE) {
        switch (buffer) {
            case COLOR_BUFFER:
            case AMBIENT_BUFFER:
            case DEPTH_BUFFER:
            case NORMAL_BUFFER:
            case POSITION_BUFFER:
                O3D_ERROR(E_InvalidOperation("It is not allowed to disable this buffer."));
            default:
                break;
        }

        m_buffers[buffer].actif = state;
    } else {
        O3D_ERROR(E_IndexOutOfRange("Buffer format"));
    }
}

Bool GBuffer::getBufferUsage(GBuffer::BufferType buffer) const
{
    if (buffer >= 0 && buffer < NUM_BUFFERS_TYPE) {
        return m_buffers[buffer].actif;
    } else {
        O3D_ERROR(E_IndexOutOfRange("Buffer format"));
    }
}

void GBuffer::create(UInt32 width, UInt32 height, UInt32 samples)
{
    if (m_fbo.isValid()) {
        return;
    }

    // create textures
    for (UInt32 i = 0; i < NUM_BUFFERS_TYPE; ++i) {
        if (m_buffers[i].actif) {
            if (samples > 1) {
                m_buffers[i].texture = new Texture2DMS(this);
                static_cast<Texture2DMS*>(m_buffers[i].texture.get())->create(
                            samples,
                            width,
                            height,
                            bufferFormatToPixelFormat(m_buffers[i].type, m_buffers[i].format));
            } else {
                m_buffers[i].texture = new Texture2D(this);
                static_cast<Texture2D*>(m_buffers[i].texture.get())->create(
                            False,
                            width,
                            height,
                            bufferFormatToPixelFormat(m_buffers[i].type, m_buffers[i].format));
            }

            m_buffers[i].texture->setFiltering(Texture::NO_FILTERING);
        }
    }

    m_fbo.create(width, height, True);

    m_fbo.attachTexture2D(m_buffers[DEPTH_BUFFER].texture.get(), FrameBuffer::DEPTH_ATTACHMENT);
    m_fbo.attachTexture2D(m_buffers[COLOR_BUFFER].texture.get(), FrameBuffer::COLOR_ATTACHMENT0);
    m_fbo.attachTexture2D(m_buffers[AMBIENT_BUFFER].texture.get(), FrameBuffer::COLOR_ATTACHMENT1);
    m_fbo.attachTexture2D(m_buffers[NORMAL_BUFFER].texture.get(), FrameBuffer::COLOR_ATTACHMENT2);
    m_fbo.attachTexture2D(m_buffers[POSITION_BUFFER].texture.get(), FrameBuffer::COLOR_ATTACHMENT3);

    // no color buffer, 0 is ambient, 1 normal+shine
    m_drawBuffers.push(DrawBuffers::COLOR_ATTACHMENT1);
    m_drawBuffers.push(DrawBuffers::COLOR_ATTACHMENT2);
    m_drawBuffers.push(DrawBuffers::COLOR_ATTACHMENT3);

    if (m_buffers[DIFFUSE_BUFFER].actif) {
        // 4 diffuse
        m_drawBuffers.push(DrawBuffers::COLOR_ATTACHMENT4);
        m_fbo.attachTexture2D(m_buffers[DIFFUSE_BUFFER].texture.get(), FrameBuffer::COLOR_ATTACHMENT4);
    }

    if (m_buffers[SPECULAR_BUFFER].actif) {
        // 5 specular
        m_drawBuffers.push(DrawBuffers::COLOR_ATTACHMENT5);
        m_fbo.attachTexture2D(m_buffers[SPECULAR_BUFFER].texture.get(), FrameBuffer::COLOR_ATTACHMENT5);
    }

    if (m_buffers[AUX_COLOR_BUFFER].actif) {
        m_drawBuffers.push(DrawBuffers::COLOR_ATTACHMENT6);
        m_fbo.attachTexture2D(m_buffers[AUX_COLOR_BUFFER].texture.get(), FrameBuffer::COLOR_ATTACHMENT6);
    }

    m_fbo.isCompleteness();
    m_fbo.unbindBuffer();

if (getScene()->getRenderer()->isError()) {
    O3D_MESSAGE(getScene()->getRenderer()->getError() + "<<<<1");
}

    m_numSamples = samples;
}

void GBuffer::release()
{   
    // release FBO and textures
    if (m_fbo.isValid()) {
        m_fbo.release();
    }

    // delete textures
    for (UInt32 i = 0; i < NUM_BUFFERS_TYPE; ++i) {
        if (m_buffers[i].texture.isValid()) {
            m_buffers[i].texture = nullptr;
        }
    }
}

void GBuffer::reshape(const Vector2i &size)
{
    if (!m_fbo.isValid()) {
        O3D_ERROR(E_InvalidOperation("GBuffer must be created"));
    }

    if ((m_fbo.getDimension().x() == (Int32)size.x()) && (m_fbo.getDimension().y() == (Int32)size.y())) {
        return;
    }

if (getScene()->getRenderer()->isError()) {
    O3D_MESSAGE(getScene()->getRenderer()->getError() + "<<<<21");
}

    // resize texture
    for (UInt32 i = 0; i < NUM_BUFFERS_TYPE; ++i) {
        if (m_buffers[i].actif) {
            // depends of the instance
            if (m_buffers[i].texture.get()->getType() == ENGINE_TEXTURE_2D) {
                ((Texture2D*)m_buffers[i].texture.get())->resize(size.x(), size.y());
            } else {
                ((Texture2DMS*)m_buffers[i].texture.get())->resize(size.x(), size.y());
            }
        }
    }

    m_fbo.resize(size.x(), size.y());
}

void GBuffer::clear()
{
    if (!m_fbo.isBound()) {
        O3D_ERROR(E_InvalidOperation("FBO must be bound"));
    }

if (getScene()->getRenderer()->isError()) {
    O3D_MESSAGE(getScene()->getRenderer()->getError() + "<<<<300-21");
}

    // output color buffer
    m_drawBuffers.reset();

if (getScene()->getRenderer()->isError()) {
    O3D_MESSAGE(getScene()->getRenderer()->getError() + "<<<<30--1");
}

    // output with context background color
    glClearBufferfv(GL_COLOR, 0, getScene()->getContext()->getBackgroundColor().getData()); // color

    if (m_drawBuffers.getCount()) {
        m_drawBuffers.apply();

        const GLfloat zeros[] = { 0.f, 0.f, 0.f, 0.f };

        // ambient with context background color
        glClearBufferfv(GL_COLOR, 0, getScene()->getContext()->getBackgroundColor().getData()); // ambient

        // others with zeros
        for (Int32 i = 1; i < (Int32)m_drawBuffers.getCount(); ++i) {
            glClearBufferfv(GL_COLOR, i, zeros);
        }
    }

if (getScene()->getRenderer()->isError()) {
    O3D_MESSAGE(getScene()->getRenderer()->getError() + "<<<<31111");
}

    const GLfloat one = 1.f;
    const GLint zero = 0;

    glClearBufferfv(GL_DEPTH, 0, &one);
    glClearBufferiv(GL_STENCIL, 0, &zero);


if (getScene()->getRenderer()->isError()) {
    O3D_MESSAGE(getScene()->getRenderer()->getError() + "<<<<31221");
}

}

void GBuffer::clearDepth()
{
    if (!m_fbo.isBound()) {
//        if (glClearNamedFramebufferfv) {
//            glClearNamedFramebufferfv() @todo and for colors too
//        } else {
            O3D_ERROR(E_InvalidOperation("FBO must be bound"));
//        }
    } else {
        const GLfloat one = 1.f;
        glClearBufferfv(GL_DEPTH, 0, &one);
    }
}

void GBuffer::clearColors()
{
    if (!m_fbo.isBound()) {
        O3D_ERROR(E_InvalidOperation("FBO must be bound"));
    } else if (m_drawBuffers.getCount()) {
        const GLfloat zeros[] = { 0.f, 0.f, 0.f, 0.f };

        // ambient with context background color
        glClearBufferfv(GL_COLOR, 0, getScene()->getContext()->getBackgroundColor().getData());

        // others with zero
        for (Int32 i = 1; i < (Int32)m_drawBuffers.getCount(); ++i) {
            glClearBufferfv(GL_COLOR, i, zeros);
        }
    }
}

void GBuffer::bind()
{
    m_fbo.bindBuffer();
}

void GBuffer::unbind()
{
    m_fbo.unbindBuffer();
}

void GBuffer::swapColorMap()
{
    if (!m_buffers[AUX_COLOR_BUFFER].actif) {
        O3D_ERROR(E_InvalidOperation("Auxiliary color buffer is not defined"));
    }

    if (!m_fbo.isBound()) {
        O3D_ERROR(E_InvalidOperation("FBO must be bound"));
    }

    // attach the auxiliary color buffer in place of the primary
    if (m_fbo.isValid()) {
        m_fbo.attachTexture2D(m_buffers[AUX_COLOR_BUFFER].texture.get(), FrameBuffer::COLOR_ATTACHMENT0);
        m_fbo.attachTexture2D(m_buffers[COLOR_BUFFER].texture.get(), FrameBuffer::COLOR_ATTACHMENT6);
    }

    // swap the two textures objects
    SmartObject<Texture>::swap(m_buffers[COLOR_BUFFER].texture, m_buffers[AUX_COLOR_BUFFER].texture);
}

void GBuffer::drawAttributes()
{
    m_drawBuffers.apply();
}

void GBuffer::drawLigthing()
{
    m_drawBuffers.reset();
}

void GBuffer::drawStencil()
{
    const GLenum buffers[] = {GL_NONE};
    glDrawBuffers(1, buffers);
}

void GBuffer::resetDrawBuffers()
{
    m_drawBuffers.reset();
}

void GBuffer::draw()
{
    Box2i box(Vector2i(0, 0), m_fbo.getDimension());

    //const GLuint buffers = {GL_COLOR_ATTACHMENT0};
    //glDrawBuffers(1, buffers);

    // default is already to color0
    //m_fbo.bindBuffer();
    //glReadBuffer(GL_COLOR_ATTACHMENT0);
    //m_fbo.unbindBuffer();

    m_fbo.blit(box, box, FrameBuffer::MASK_COLOR | FrameBuffer::MASK_DEPTH, FrameBuffer::FILTER_NEAREST);
}

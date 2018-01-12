/**
 * @file texture2dfeedback.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/texture2dfeedback.h"
#include "o3d/engine/context.h"

using namespace o3d;

Texture2DFeedback::Texture2DFeedback(Context *context) :
    m_context(context),
	m_state(False),
	m_readBuffer(0),
	m_size(0),
	m_next(0),
	m_pixelFormat(PF_RGBA_U8),
	m_format(TF_RGBA),
	m_type(DATA_UNSIGNED_BYTE),
    m_mapped(nullptr)
{
	m_buffersId[0] = 0;
	m_buffersId[1] = 0;
}

Texture2DFeedback::~Texture2DFeedback()
{
	release();
}

void Texture2DFeedback::create(UInt32 readBuffer, PixelFormat pf)
{
    if (m_state) {
		release();
    }

	m_readBuffer = readBuffer;

	// create PBO
    glGenBuffers(2, (GLuint*)m_buffersId);

	O3D_GALLOC(MemoryManager::GPU_PBO, m_buffersId[0], 0);
	m_context->bindPixelPackBuffer(m_buffersId[0]);
	O3D_GALLOC(MemoryManager::GPU_PBO, m_buffersId[1], 0);
	m_context->bindPixelPackBuffer(m_buffersId[1]);

	m_context->bindPixelPackBuffer(0);

    m_mapped = nullptr;

    m_format = GLTexture::getGLFormat(m_context, pf);
    m_type = GLTexture::getGLType(pf);
	m_pixelFormat = pf;

	m_next = 0;
	m_readBuffer = readBuffer;
	m_state = True;
}

void Texture2DFeedback::release()
{
    if (m_buffersId[0] != 0) {
		O3D_GFREE(MemoryManager::GPU_PBO, m_buffersId[0]);

		m_context->deletePixelBuffer(m_buffersId[0]);
		m_buffersId[0] = 0;
	}

    if (m_buffersId[1] != 0) {
		O3D_GFREE(MemoryManager::GPU_PBO, m_buffersId[1]);

		m_context->deletePixelBuffer(m_buffersId[1]);
		m_buffersId[1] = 0;
	}

	m_pixelFormat = PF_RGBA_U8;

	m_format = TF_RGBA;
	m_type = DATA_UNSIGNED_BYTE;

    m_mapped = nullptr;
	m_size = 0;
	m_state = False;
}

void Texture2DFeedback::setBox(const Box2i &box)
{
    if (!m_state) {
		O3D_ERROR(E_InvalidOperation("Texture2DFeedback must be created"));
    }

	m_box = box;

	UInt32 size = (m_box.width() * m_box.height() * GLTexture::getPixelSize(m_pixelFormat)) >> 3;
	UInt32 dbgSize = (m_box.width() * m_box.height() * GLTexture::getInternalPixelSize(m_context, m_pixelFormat)) >> 3;

	// allocate PBO if necessary
    if (size != m_size) {
		m_size = size;

		O3D_GREALLOC(MemoryManager::GPU_PBO, m_buffersId[0], dbgSize);

		m_context->bindPixelPackBuffer(m_buffersId[0]);
        glBufferData(GL_PIXEL_PACK_BUFFER, m_size, nullptr, PixelBuffer::STREAM_READ);

		O3D_GREALLOC(MemoryManager::GPU_PBO, m_buffersId[1], dbgSize);

		m_context->bindPixelPackBuffer(m_buffersId[1]);
        glBufferData(GL_PIXEL_PACK_BUFFER, m_size, nullptr, PixelBuffer::STREAM_READ);

		m_context->bindPixelUnpackBuffer(0);
	}
}

void Texture2DFeedback::update()
{
	UInt32 pbo = m_buffersId[m_next];

    if (!m_state || (m_size == 0)) {
		O3D_ERROR(E_InvalidParameter("Object must be created before and size defined"));
    }

    if (m_next == 0) {
		m_next = 1;
    } else {
		m_next = 0;
    }

	glReadBuffer(m_readBuffer);

    // many solutions : glGetTexImage the less efficient and works only with the full surface
    // and not availables on GLES.
    // @todo uses glGetTextureSubImage if GL 4.5+ the most efficient
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetTextureSubImage.xhtml
    if (glGetTextureSubImage) {
        // @todo uses glGetTextureSubImage if GL 4.5+ the most efficient
//        glGetTextureSubImage(
//                    GL_TEXTURE_2D,
//                    0/*level*/,
//                    0, 0, 0,
//                    m_box.width(), m_box.height(), 0,
//                    m_format, m_type, m_size, nullptr);
    }

	m_context->bindPixelPackBuffer(pbo);

	// update the content of the PBO
	glReadPixels(
		m_box.x(),
		m_box.y(),
		m_box.width(),
		m_box.height(),
		m_format,
		m_type,
        nullptr);

	m_context->bindPixelPackBuffer(0);

	// returns to BACK buffer
	glReadBuffer(GL_BACK);
}

const UInt8* Texture2DFeedback::lock(UInt32 size, UInt32 offset)
{
	UInt32 pbo = m_buffersId[m_next == 0 ? 1 : 0];

    if (!m_state) {
		O3D_ERROR(E_InvalidParameter("Object must be created before"));
    }

    if (size+offset > m_size) {
		O3D_ERROR(E_InvalidParameter("Size+offset must be less or equal to m_size"));
    }

    // not available of GLES
    if (!glMapBuffer) {
        return nullptr;
    }

	m_context->bindPixelPackBuffer(pbo);

    // m_mapped = reinterpret_cast<UInt8*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, PixelBuffer::READ_ONLY)) + offset;
    m_mapped = reinterpret_cast<UInt8*>(glMapBufferRange(GL_PIXEL_PACK_BUFFER, offset, size, PixelBuffer::MAP_READ));

	return m_mapped;
}

const UInt8* Texture2DFeedback::lock(UInt32 size)
{
	UInt32 pbo = m_buffersId[m_next == 0 ? 1 : 0];

    if (!m_state) {
		O3D_ERROR(E_InvalidParameter("Object must be created before"));
    }

    if (size != m_size) {
        O3D_ERROR(E_InvalidParameter("Size must be equal to m_size"));
    }

    // not available of GLES
    if (!glMapBuffer) {
        return nullptr;
    }

	m_context->bindPixelPackBuffer(pbo);

    // m_mapped = reinterpret_cast<UInt8*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, PixelBuffer::READ_ONLY));
    m_mapped = reinterpret_cast<UInt8*>(glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, size, PixelBuffer::MAP_READ));

	return m_mapped;
}

void Texture2DFeedback::unlock()
{
    if (m_mapped != nullptr) {
        // not available of GLES
        if (glUnmapBuffer) {
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        }

		// finally unbind pack buffer
		m_context->bindPixelPackBuffer(0);

        m_mapped = nullptr;
	}
}

void Texture2DFeedback::copyToTexture(Texture2D *texture, UInt32 level)
{
	UInt32 pbo = m_buffersId[m_next == 0 ? 1 : 0];

    if ((texture == nullptr) || !texture->isValid()) {
		O3D_ERROR(E_InvalidParameter("Texture must be valid"));
    }

    if ((level < texture->getMinLevel()) || (level > texture->getMaxLevel())) {
		O3D_ERROR(E_InvalidParameter("Texture level out of boundaries"));
    }

    if ((texture->getWidth() != (UInt32)m_box.width()) || (texture->getHeight() != (UInt32)m_box.height())) {
		O3D_ERROR(E_InvalidParameter("Texture width/height is different from box size"));
    }

    if (texture->getPixelFormat() != m_pixelFormat) {
		O3D_ERROR(E_InvalidParameter("Texture pixel format is different from read buffer"));
    }

	texture->bind();
	m_context->bindPixelUnpackBuffer(pbo);

	// update the content of the texture with one PBO
	glTexSubImage2D(
		TEXTURE_2D,
		level,
		0,
		0,
		m_box.width(),
		m_box.height(),
	    m_format,
	    m_type,
        nullptr);

	m_context->bindPixelUnpackBuffer(0);
	texture->unbind();
}

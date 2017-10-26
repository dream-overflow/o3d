/**
 * @file framebuffer.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/framebuffer.h"

#include "o3d/engine/glextdefines.h"

#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/context.h"

#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/texture2dms.h"

#include "o3d/engine/scene/scene.h"

using namespace o3d;

//! Default constructor.
FrameBuffer::FrameBuffer(Context *context) :
    m_context(context),
	m_bufferId(O3D_UNDEFINED),
	m_colorFormat(PF_RGBA_U8),
	m_internalFormat(0)
{
    O3D_ASSERT(m_context != nullptr);

    m_attachments.resize(NUM_ATTACHMENTS, nullptr);
}

// Bind this buffer
void FrameBuffer::bindBuffer()
{
    m_context->bindFrameBuffer(m_bufferId);
}

// Unbound this buffer
void FrameBuffer::unbindBuffer()
{
    m_context->bindFrameBuffer(0);
}

Bool FrameBuffer::isBound() const
{
    return m_context->getCurrentFrameBuffer() == m_bufferId;
}

// Create the FBO. Need a call to BindBuffer() to really create it after this method.
void FrameBuffer::create(
	UInt32 width,
	UInt32 height,
	PixelFormat colorFormat,
	Bool dontUnbind)
{
	if (m_bufferId != O3D_UNDEFINED)
		release();

	// Generate the buffer identifier (not really created at this time, it need a call to bind)
	glGenFramebuffers(1, (GLuint*)&m_bufferId);

	m_dimension.set(width, height);
	
	m_colorFormat = colorFormat;
    m_internalFormat = GLTexture::getGLInternalFormat(m_context->getRenderer(), m_colorFormat);

    m_context->bindFrameBuffer(m_bufferId);

	if (!dontUnbind)
        m_context->bindFrameBuffer(0);
}

// Create the FBO. Need a call to Bind() to really create it after this method.
void FrameBuffer::create(UInt32 width, UInt32 height, Bool dontUnbind)
{
	if (m_bufferId != O3D_UNDEFINED)
		release();

	// Generate the buffer identifier (not really created at this time, it need a call to bind)
	glGenFramebuffers(1, (GLuint*)&m_bufferId);

	m_dimension.set(width, height);
	
	m_colorFormat = PF_RGBA_U8;
	m_internalFormat = 0;

    m_context->bindFrameBuffer(m_bufferId);

    if (!dontUnbind) {
        m_context->bindFrameBuffer(0);
    }
}

// Destroy this buffer and release memory
void FrameBuffer::release()
{
    if (!isValid()) {
		return;
    }

    UInt32 previousFBO = m_context->getCurrentFrameBuffer();
    if (previousFBO == m_bufferId) {
        O3D_ERROR(E_InvalidOperation("Trying to release the currently bound FBO"));
    }

    // avoid rebind and restoring
    if (previousFBO != m_bufferId) {
        m_context->bindFrameBuffer(m_bufferId);
    } else {
        previousFBO = O3D_UNDEFINED;
    }

	// detach
	for (Int32 i = 0; i < NUM_ATTACHMENTS; ++i)	{
		AttachmentElt *it = m_attachments[i];

        if (it) {
			// texture attachment
            if (it->texture.isValid()) {
                if (it->where == DEPTH_ATTACHMENT) {
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
                } else if (it->where == STENCIL_ATTACHMENT) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
                } else if (it->where == DEPTH_STENCIL_ATTACHMENT) {
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
                } else {
					glFramebufferTexture2D(
					GL_FRAMEBUFFER,
					(GLenum)GL_COLOR_ATTACHMENT0 + it->where,
					GL_TEXTURE_2D,
					0,
					0);
                }
            } else if (it->renderId != O3D_UNDEFINED) {
                // render attachment
				glDeleteRenderbuffers(1, (GLuint*)&it->renderId);
				O3D_GFREE(MemoryManager::GPU_FBO, it->renderId);
			}

			deletePtr(it);
            m_attachments[i] = nullptr;
		}
	}

	m_colorFormat = PF_RGBA_U8;
	m_internalFormat = 0;
	
    m_context->bindFrameBuffer(0);

	glDeleteFramebuffers(1, (GLuint*)&m_bufferId);
	m_bufferId = O3D_UNDEFINED;

    if (previousFBO != O3D_UNDEFINED) {
        m_context->bindFrameBuffer(previousFBO);
    }
}

void FrameBuffer::resize(UInt32 w, UInt32 h)
{
    if ((m_dimension.x() == (Int32)w) && (m_dimension.y() == (Int32)h)) {
		return;
    }

    if (!isValid()) {
		return;
    }

	// TODO

	m_dimension.set(w, h);
}

// Blit to another frame buffer.
void FrameBuffer::blit(
		FrameBuffer &dstFrameBuffer,
		const Box2i &src,
		const Box2i &dst,
        Int32/*BlitMask*/ mask,
		Filter filter)
{
    if (&dstFrameBuffer == this) {
		O3D_ERROR(E_InvalidParameter("Destination frame buffer must be different from source"));
    }

    if (((mask & MASK_DEPTH) == MASK_DEPTH) || ((mask & MASK_STENCIL) == MASK_STENCIL)) {
        if (filter != FILTER_NEAREST) {
			O3D_ERROR(E_InvalidParameter("Only use nearest filtering with stencil/depth mask"));
        }
	}

    if ((src.x() > m_dimension.x()) || (src.x2() > m_dimension.x()) ||
        (src.y() > m_dimension.y()) || (src.y2() > m_dimension.y())) {

        O3D_ERROR(E_InvalidParameter("Source position out of rectangle"));
    }

    if ((dst.x() > dstFrameBuffer.m_dimension.x()) || (dst.x2() > dstFrameBuffer.m_dimension.x()) ||
        (dst.y() > dstFrameBuffer.m_dimension.y()) || (dst.y2() > dstFrameBuffer.m_dimension.y())) {

        O3D_ERROR(E_InvalidParameter("Destination position out of rectangle"));
    }

	UInt32 glMask = 0;

    if ((mask & MASK_COLOR) == MASK_COLOR) {
		glMask |= GL_COLOR_BUFFER_BIT;
    }
    if ((mask & MASK_DEPTH) == MASK_DEPTH) {
		glMask |= GL_DEPTH_BUFFER_BIT;
    }
    if ((mask & MASK_STENCIL) == MASK_STENCIL) {
		glMask |= GL_STENCIL_BUFFER_BIT;
    }

	// Bind the source as GL_READ_FRAMEBUFFER
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_bufferId);

	// Bind the destination as GL_DRAW_FRAMEBUFFER
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFrameBuffer.m_bufferId);

	// Perform the blit operation
	glBlitFramebuffer(
			src.x(), src.y(), src.x2(), src.y2(),
			dst.x(), dst.y(), dst.x2(), dst.y2(),
			glMask,
			filter == FILTER_NEAREST ? GL_NEAREST : GL_LINEAR);

	// Bind current FRAMEBUFFER to 0
    unbindBuffer();

    // Bind current read FRAMEBUFFER to 0
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void FrameBuffer::blit(
        const Box2i &src,
        const Box2i &dst,
        Int32/*BlitMask*/ mask,
        FrameBuffer::Filter filter)
{
    if (((mask & MASK_DEPTH) == MASK_DEPTH) || ((mask & MASK_STENCIL) == MASK_STENCIL)) {
        if (filter != FILTER_NEAREST) {
            O3D_ERROR(E_InvalidParameter("Only use nearest filtering with stencil/depth mask"));
        }
    }

    if ((src.x() > m_dimension.x()) || (src.x2() > m_dimension.x()) ||
        (src.y() > m_dimension.y()) || (src.y2() > m_dimension.y())) {

        O3D_ERROR(E_InvalidParameter("Source position out of rectangle"));
    }

    const Box2i &vp = m_context->getViewPort();

    if ((dst.x() > vp.width()) || (dst.x2() > vp.width()) ||
        (dst.y() > vp.height()) || (dst.y2() > vp.height())) {

        O3D_ERROR(E_InvalidParameter("Destination position out of rectangle"));
    }

    UInt32 glMask = 0;

    if ((mask & MASK_COLOR) == MASK_COLOR) {
        glMask |= GL_COLOR_BUFFER_BIT;
    }
    if ((mask & MASK_DEPTH) == MASK_DEPTH) {
        glMask |= GL_DEPTH_BUFFER_BIT;
    }
    if ((mask & MASK_STENCIL) == MASK_STENCIL) {
        glMask |= GL_STENCIL_BUFFER_BIT;
    }

    // Bind the destination as GL_DRAW_FRAMEBUFFER
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    // GL_DRAW_FRAMEBUFFER is similar as GL_FRAME_BUFFER then :
    m_context->bindFrameBuffer(0);

    // Bind the source as GL_READ_FRAMEBUFFER
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_bufferId);

    // Perform the blit operation
    glBlitFramebuffer(
            src.x(), src.y(), src.x2(), src.y2(),
            dst.x(), dst.y(), dst.x2(), dst.y2(),
            glMask,
            filter == FILTER_NEAREST ? GL_NEAREST : GL_LINEAR);

    // Bind current read FRAMEBUFFER to 0
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

// Attach a texture to the FBO
void FrameBuffer::attachTexture2D(Texture *pTexture, Attachment where)
{
	O3D_ASSERT(isValid());
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

    if (!pTexture) {
		O3D_ERROR(E_InvalidParameter("Texture must be valid"));
    }

    if ((pTexture->getTextureType() != TEXTURE_2D) &&
        (pTexture->getTextureType() != TEXTURE_2D_MULTISAMPLE)) {

        O3D_ERROR(E_InvalidParameter("Supports 2D texture (multi-sampled) only"));
    }

    if ((pTexture->getWidth() != (UInt32)m_dimension.x()) ||
        (pTexture->getHeight() != (UInt32)m_dimension.y())) {

        O3D_ERROR(E_InvalidFormat("Size must be identical"));
    }

    // now it is possible to attach differents pixels formats
//	if( (where < DEPTH_ATTACHMENT) && (pTexture->getPixelFormat() != m_colorFormat)) {
//		O3D_ERROR(E_InvalidFormat("Color attachment must use the same pixel format"));
//  }

    if (where == DEPTH_ATTACHMENT) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pTexture->getTextureType(), pTexture->getTextureId(), 0);
    } else if (where == STENCIL_ATTACHMENT) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, pTexture->getTextureType(), pTexture->getTextureId(), 0);
    } else if (where == DEPTH_STENCIL_ATTACHMENT) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, pTexture->getTextureType(), pTexture->getTextureId(), 0);
    } else {
        if (where >= m_context->getMaxDrawBuffers()) {
            O3D_ERROR(E_InvalidOperation("Max draw buffer size."));
        }

		glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    (GLenum)GL_COLOR_ATTACHMENT0 + where,
                    pTexture->getTextureType(),
                    pTexture->getTextureId(),
                    0);
	}

	// set the attachment
    if (m_attachments[where]) {
        AttachmentElt *attachment = m_attachments[where];
        attachment->texture.set(pTexture->getScene(), pTexture);
        attachment->pixelFormat = pTexture->getPixelFormat();
    } else {
        AttachmentElt *attachment = new AttachmentElt;
        attachment->where = where;
        attachment->texture.set(pTexture->getScene(), pTexture);
        attachment->pixelFormat = pTexture->getPixelFormat();
        m_attachments[where] = attachment;
    }
}

// Detach a texture from the FBO
void FrameBuffer::detach(Attachment where)
{
	O3D_ASSERT(isValid());
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

	// search
    if (m_attachments[where]) {
		AttachmentElt *attachment = m_attachments[where];

		// texture attachment
        if (attachment->texture.isValid()) {
            attachment->texture = nullptr;

            if (where == DEPTH_ATTACHMENT) {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
            } else if (where == STENCIL_ATTACHMENT) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
            } else if (where == DEPTH_STENCIL_ATTACHMENT) {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
            } else {
				glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				(GLenum)GL_COLOR_ATTACHMENT0 + where,
				GL_TEXTURE_2D,
				0,
				0);
            }
        } else if (attachment->renderId != O3D_UNDEFINED) {
            // render attachment
            if (where == DEPTH_ATTACHMENT) {
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
            } else if (where == STENCIL_ATTACHMENT) {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
            } else if (where == DEPTH_STENCIL_ATTACHMENT) {
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
            } else {
				glFramebufferRenderbuffer(
				GL_FRAMEBUFFER,
				(GLenum)GL_COLOR_ATTACHMENT0 + where,
				GL_RENDERBUFFER,
				0);
            }
		}

		deletePtr(attachment);
        m_attachments[where] = nullptr;
	}
}

// Add a color render buffer (FBO must be created an bound)
void FrameBuffer::attachColorRender(Attachment where)
{
	O3D_ASSERT(isValid());
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

	// attachment already used
    if (m_attachments[where]) {
		O3D_ERROR(E_InvalidOperation("Can not attach many objects on the same attachment point"));
    }

	AttachmentElt *attachment = new AttachmentElt;
	attachment->pixelFormat = m_colorFormat;
	attachment->where = where;
	attachment->dataSize = ((m_dimension.x() * m_dimension.y()) *
        GLTexture::getInternalPixelSize(m_context->getRenderer(), m_colorFormat)) >> 3;

	// create a new render buffer
	glGenRenderbuffers(1, &attachment->renderId);
	glBindRenderbuffer(GL_RENDERBUFFER, attachment->renderId);

	glRenderbufferStorage(
		GL_RENDERBUFFER,
		m_internalFormat,
		m_dimension.x(),
		m_dimension.y());

	O3D_GALLOC(MemoryManager::GPU_FBO, attachment->renderId, attachment->dataSize);

	// and attach it to the fbo (current bound fbo)
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER,
		(GLenum)where + GL_COLOR_ATTACHMENT0,
		GL_RENDERBUFFER,
		attachment->renderId);
	
	m_attachments[where] = attachment;
}

// Add a depth/stencil render buffer (FBO must be created an bound)
void FrameBuffer::attachDepthStencilRender(Format format)
{
	O3D_ASSERT(isValid());
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

    Renderer *renderer = m_context->getRenderer();

	Attachment where = DEPTH_ATTACHMENT;

	switch (format)	{
        case DEPTH_U16:
        case DEPTH_U24:
        case DEPTH_U32:
		case DEPTH_F32:
			where = DEPTH_ATTACHMENT;
			break;

        //case STENCIL_1:
        //case STENCIL_4:
        case STENCIL_U8:
        //case STENCIL_16:
            where = STENCIL_ATTACHMENT;
            break;

		case DEPTH_STENCIL_U24_U8:
        case DEPTH_STENCIL_F32_U24_U8:
			where = DEPTH_STENCIL_ATTACHMENT;
			break;
	}

	// attachment already used
    if (m_attachments[where]) {
		O3D_ERROR(E_InvalidOperation("Can not attach many objects on the same attachment point"));
    }

	AttachmentElt *attachment = new AttachmentElt;
	attachment->where = where;

	switch (format)	{
        case DEPTH_U16:
            attachment->pixelFormat = PF_DEPTH_U16;
			break;

        case DEPTH_U24:
            attachment->pixelFormat = PF_DEPTH_U24;
			break;

        case DEPTH_U32:
            attachment->pixelFormat = PF_DEPTH_U32;
			break;

		case DEPTH_F32:
			attachment->pixelFormat = PF_DEPTH_F32;
			break;

        case STENCIL_U8:
            attachment->pixelFormat = PF_STENCIL_U8;
            break;

		case DEPTH_STENCIL_U24_U8:
            attachment->pixelFormat = PF_DEPTH_U24_STENCIL_U8;
			break;

        case DEPTH_STENCIL_F32_U24_U8:
            attachment->pixelFormat = PF_DEPTH_F32_STENCIL_U8;
            break;
	}

	attachment->dataSize = ((m_dimension.x() * m_dimension.y()) *
		GLTexture::getInternalPixelSize(renderer, attachment->pixelFormat)) >> 3;

	// create a new render buffer
	glGenRenderbuffers(1, &attachment->renderId);
	glBindRenderbuffer(GL_RENDERBUFFER, attachment->renderId);

	glRenderbufferStorage(
		GL_RENDERBUFFER,
		GLTexture::getGLInternalFormat(renderer, attachment->pixelFormat),
		m_dimension.x(),
		m_dimension.y());

	O3D_GALLOC(MemoryManager::GPU_FBO, attachment->renderId, attachment->dataSize);

	// and attach it to the fbo (current bound fbo)
    if (where == DEPTH_ATTACHMENT) {
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER,
			attachment->renderId);
    } else if (where == STENCIL_ATTACHMENT) {
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            attachment->renderId);
    } else if (where == DEPTH_STENCIL_ATTACHMENT) {
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER,
			attachment->renderId);
    }

	m_attachments[where] = attachment;
}

// Check for frame buffer completeness
Bool FrameBuffer::isCompleteness() const
{
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (result) {
		case GL_FRAMEBUFFER_COMPLETE:
			return True;
		case GL_FRAMEBUFFER_UNDEFINED:
			O3D_ERROR(E_InvalidParameter("Undefined"));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			O3D_ERROR(E_InvalidParameter("Incomplete attachment"));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			O3D_ERROR(E_InvalidParameter("Missing attachment"));
			break;
		/*case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT:
			O3D_ERROR(O3D_E_InvalidParameter("Duplicate attachment"));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			O3D_ERROR(O3D_E_InvalidParameter("Incomplete dimensions"));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
			O3D_ERROR(O3D_E_InvalidParameter("Incomplete formats"));
			break;*/
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			O3D_ERROR(E_InvalidParameter("Incomplete draw buffer"));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			O3D_ERROR(E_InvalidParameter("Incomplete read buffer"));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			O3D_ERROR(E_InvalidParameter("Incomplete multisample"));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			O3D_ERROR(E_InvalidParameter("Incomplete layer targets"));
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			O3D_ERROR(E_InvalidParameter("Unsupported"));
			break;
		default:
			O3D_ERROR(E_InvalidParameter("Unknown error code"));
			break;
	}

    return False;
}

void FrameBuffer::clearColor()
{
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

    glClear(GL_COLOR_BUFFER_BIT);
}

// Clear the color attachment (once time bound)
void FrameBuffer::clearColor(Attachment drawBuffer, Color color)
{
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

    //glClear(GL_COLOR_BUFFER_BIT);
    glClearBufferfv(GL_COLOR, drawBuffer, color.getData());

    // TODO for GL 4.5 :
    // glClearNamedFramebufferfv(m_bufferId, GL_COLOR, drawBuffer, color.getData());
}

// Clear the depth attachment (once time bound)
void FrameBuffer::clearDepth(Float depth)
{
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

    //glClear(GL_DEPTH_BUFFER_BIT);
    glClearBufferfv(GL_DEPTH, 0, &depth);

    // TODO for GL 4.5 :
    // glClearNamedFramebufferfv(m_bufferId, GL_DEPTH, 0, &depth);
}

// Clear the stencil attachment (once time bound)
void FrameBuffer::clearStencil(Int32 stencil)
{
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

    //glClear(GL_STENCIL_BUFFER_BIT);
    glClearBufferiv(GL_STENCIL, 0, &stencil);

    // TODO for GL 4.5 :
    // glClearNamedFramebufferfv(m_bufferId, GL_STENCIL, 0, &stencil);
}

void FrameBuffer::clearDepthAndStencil(Float depth, Int32 stencil)
{
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

    //glClear(GL_STENCIL_BUFFER_BIT);
    glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);

    // TODO for GL 4.5 :
    // glClearNamedFramebufferfi(m_bufferId, GL_DEPTH_STENCIL, 0, depth, stencil);
}

// Clear all the buffer (once time bound)
void FrameBuffer::clearColorAndDepth()
{
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::clearAllBuffers()
{
    O3D_ASSERT(m_context->getCurrentFrameBuffer() == m_bufferId);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


/**
 * @file pixelbuffer.cpp
 * @brief Implementation of PixelBuffer.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-08-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/pixelbuffer.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/engine/glextensionmanager.h"

using namespace o3d;

PixelBuffer::PixelBuffer(
        Context *context,
        Storage storageType) :
    BufferObject(context),
    m_count(0),
    m_storageType(storageType),
    m_lockCount(0),
    m_lockFlags(0),
    m_mapped(nullptr)
{

}

PixelBuffer::~PixelBuffer()
{
	release();
}

void PixelBuffer::release()
{
    if (m_bufferId != O3D_UNDEFINED) {
		O3D_ASSERT(m_lockCount == 0);

		O3D_GFREE(MemoryManager::GPU_PBO, m_bufferId);

		m_context->deletePixelBuffer(m_bufferId);

		m_bufferId = O3D_UNDEFINED;
		m_count = 0;
	}
}

void PixelBuffer::bindPackBuffer() const
{
	m_context->bindPixelPackBuffer(m_bufferId);
}

void PixelBuffer::bindUnpackBuffer() const
{
	m_context->bindPixelUnpackBuffer(m_bufferId);
}

void PixelBuffer::unbindPackBuffer() const
{
	m_context->bindPixelPackBuffer(0);
}

void PixelBuffer::unbindUnpackBuffer() const
{
	m_context->bindPixelUnpackBuffer(0);
}

void PixelBuffer::create(
	UInt32 count,
	Storage storageType,
	const Float* data,
	Bool dontUnbind)
{
#ifdef O3D_GPU_MEMORY_MANAGER
	Bool realloc = False;

    if (m_bufferId != O3D_UNDEFINED) {
		realloc = True;
    }
#endif

	// Generate the buffer if necessary
    if (m_bufferId == O3D_UNDEFINED) {
        glGenBuffers(1, (GLuint*)&m_bufferId);
    }

	bindPackBuffer();

    // simply perform an update or create
    if ((count == m_count) && (storageType == m_storageType)) {
		glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, m_count*sizeof(Float), data);
    } else {
		// Define global parameters
		m_count = count;
		m_storageType = storageType;

	#ifdef O3D_GPU_MEMORY_MANAGER
        if (realloc) {
			O3D_GREALLOC(MemoryManager::GPU_PBO, m_bufferId, getDataSize());
        } else {
			O3D_GALLOC(MemoryManager::GPU_PBO, m_bufferId, getDataSize());
        }
	#endif

		glBufferData(GL_PIXEL_UNPACK_BUFFER, getDataSize(), data, storageType);
	}

    if (!dontUnbind) {
		unbindPackBuffer();
    }
}

void PixelBuffer::getData(Float *data, UInt32 offset, UInt32 count)
{
	O3D_ASSERT(data);

    if (data) {
		bindUnpackBuffer();
		glGetBufferSubData(GL_PIXEL_UNPACK_BUFFER, offset*sizeof(Float), count*sizeof(Float), data);
	}
}

void PixelBuffer::update(const Float* data, UInt32 offset, UInt32 count)
{
    if (m_lockCount > 0) {
		O3D_ERROR(E_InvalidOperation("Cannot update the content of a locked PBO"));
    }

    if (data) {
		bindPackBuffer();
		glBufferSubData(GL_PIXEL_PACK_BUFFER, offset*sizeof(Float), count*sizeof(Float), data);
	}
}

Float* PixelBuffer::lock(UInt32 offset, UInt32 size, LockFlags flags)
{
    if (size == 0) {
        size = (m_count - offset) * sizeof(Float);
    }

    // already locked ?
    if (m_lockCount > 0) {
        // with the same mode ?
        if (flags == m_lockFlags) {
        ++m_lockCount;
        return m_mapped + offset;
        } else {
        O3D_ERROR(E_InvalidOperation("Cannot lock twice time the same PBO with different policy"));
        }
    } else {
        bindPackBuffer();

        // prefer map buffer range (support offet + size and GL ES compatible)
        m_mapped = reinterpret_cast<Float*>(glMapBufferRange(GL_PIXEL_PACK_BUFFER, offset, size, flags));

        if (m_mapped) {
            m_lockFlags = flags;

            ++m_lockCount;
            return m_mapped;
        }

        return nullptr;
    }
}

void PixelBuffer::unlock()
{
    if (m_lockCount > 0) {
		--m_lockCount;

        if (m_lockCount == 0) {
			m_context->bindPixelUnpackBuffer(m_bufferId);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		}
	}
}

void PixelBuffer::copyToTexture(Texture2D *texture, Bool dontUnbind)
{
    if (m_bufferId == 0) {
		O3D_ERROR(E_InvalidPrecondition("PixelBuffer must be valid"));
    }

    if (texture == nullptr) {
		O3D_ERROR(E_InvalidPrecondition("Texture must be valid"));
    }

	// bind the texture and PBO
	texture->bind();
	m_context->bindPixelUnpackBuffer(m_bufferId);

    UInt32 format = GLTexture::getGLFormat(m_context, texture->getPixelFormat());
    // Int32 internalFormat = GLTexture::getGLInternalFormat(m_context, texture->getPixelFormat());
    DataType type = GLTexture::getGLType(texture->getPixelFormat());

	// copy pixels from PBO to texture object. uses offset instead of pointer
	glTexSubImage2D((GLenum)texture->getTextureType(), 0, 0, 0,
		texture->getWidth(), texture->getHeight(),
        format, type,
	    0);

    if (!dontUnbind) {
		m_context->bindPixelUnpackBuffer(0);
    }
}

Bool PixelBuffer::checkData()
{
	bindUnpackBuffer();

	Int32 bufferSize = 0;
	glGetBufferParameteriv(GL_PIXEL_UNPACK_BUFFER, BUFFER_SIZE, (GLint*)&bufferSize);
	return (bufferSize > 0);
}

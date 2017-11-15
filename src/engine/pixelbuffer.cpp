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
#include "o3d/engine/glextdefines.h"

using namespace o3d;
/*
PixelBuffer::PixelBuffer(
		BaseObject *parent,
		Storage storageType):
		m_parent(parent),
		m_context(NULL),
		m_bufferId(O3D_UNDEFINED),
		m_count(0),
		m_storageType(storageType),
		m_lockCount(0),
		m_lockMode(READ_ONLY),
		m_mapped(NULL)
{
	if (!m_parent)
		O3D_ERROR(E_NullPointer("Parent must be a valid pointer"));

	if (!o3d::typeOf<Scene>(m_parent->getTopLevelParent()))
		O3D_ERROR(E_InvalidParameter("Top level parent must be the scene"));

	m_context = reinterpret_cast<Scene*>(m_parent->getTopLevelParent())->getContext();
}

PixelBuffer::~PixelBuffer()
{
	release();
}

void PixelBuffer::release()
{
	if (m_bufferId != O3D_UNDEFINED)
	{
		O3D_ASSERT(m_lockCount == 0);

		O3D_GFREE(MemoryManager::GPU_PBO, m_bufferId);

		m_context->deletePixelBuffer(m_bufferId);

		m_bufferId = O3D_UNDEFINED;
		m_count = 0;
	}
}

// Get the scene parent.
Scene* PixelBuffer::getScene()
{
	return reinterpret_cast<Scene*>(m_parent->getTopLevelParent());
}

// Get the scene parent (read only).
const Scene* PixelBuffer::getScene() const
{
	return reinterpret_cast<Scene*>(m_parent->getTopLevelParent());
}

// Bind the PBO if necessary.
void PixelBuffer::bindPackBuffer() const
{
	m_context->bindPixelPackBuffer(m_bufferId);
}

// Bind the PBO if necessary.
void PixelBuffer::bindUnpackBuffer() const
{
	m_context->bindPixelUnpackBuffer(m_bufferId);
}

// Unbind the PBO (bind the PBO 0).
void PixelBuffer::unbindPackBuffer() const
{
	m_context->bindPixelPackBuffer(0);
}

// Unbind the PBO (bind the PBO 0).
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

	if (m_bufferId != O3D_UNDEFINED)
		realloc = True;
#endif

	// Generate the buffer if necessary
	if (m_bufferId == O3D_UNDEFINED)
		glGenBuffers(1,(GLuint*)&m_bufferId);

	bindPackBuffer();

	// simply perform an update
	if ((count == m_count) && (storageType == m_storageType))
	{
		glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, m_count*sizeof(Float), data);
	}
	// or a create
	else
	{
		// Define global parameters
		m_count = count;
		m_storageType = storageType;

	#ifdef O3D_GPU_MEMORY_MANAGER
		if (realloc)
			O3D_GREALLOC(MemoryManager::GPU_PBO, m_bufferId, getDataSize());
		else
			O3D_GALLOC(MemoryManager::GPU_PBO, m_bufferId, getDataSize());
	#endif

		glBufferData(GL_PIXEL_UNPACK_BUFFER, getDataSize(), data, storageType);
	}

	if (!dontUnbind)
		unbindPackBuffer();
}

void PixelBuffer::getData(Float *data, UInt32 offset, UInt32 count)
{
	O3D_ASSERT(data);

	if (data)
	{
		bindUnpackBuffer();
		glGetBufferSubData(GL_PIXEL_UNPACK_BUFFER, offset*sizeof(Float), count*sizeof(Float), data);
	}
}

void PixelBuffer::update(const Float* data, UInt32 offset, UInt32 count)
{
	if (m_lockCount > 0)
		O3D_ERROR(E_InvalidOperation("Cannot update the content of a locked PBO"));

	if (data)
	{
		bindPackBuffer();
		glBufferSubData(GL_PIXEL_PACK_BUFFER, offset*sizeof(Float), count*sizeof(Float), data);
	}
}

Float* PixelBuffer::lock(
		UInt32 offset,
		UInt32 size,
		LockMode flags)
{
	// already locked ?
	if (m_lockCount > 0)
	{
		// with the same mode ?
		if (flags == m_lockMode)
		{
			++m_lockCount;
			return m_mapped + offset;
		}
		else
			O3D_ERROR(E_InvalidOperation("Cannot lock twice time the same PBO with different policy"));
	}
	else
	{
		m_context->bindPixelUnpackBuffer(m_bufferId);
		m_mapped = reinterpret_cast<Float*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER, flags));

		// Return NULL if buffer is really NULL
		if (m_mapped)
		{
			m_lockMode = flags;

			++m_lockCount;
			return m_mapped + offset;
		}
		else
			return NULL;
	}
}

void PixelBuffer::unlock()
{
	if (m_lockCount > 0)
	{
		--m_lockCount;

		if (m_lockCount == 0)
		{
			m_context->bindPixelUnpackBuffer(m_bufferId);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		}
	}
}

void PixelBuffer::copyToTexture(Texture *texture, Bool dontUnbind)
{
	if (m_bufferId == 0)
		O3D_ERROR(E_InvalidPrecondition("PixelBuffer must be valid"));

	if (texture == NULL)
		O3D_ERROR(E_InvalidPrecondition("Texture must be valid"));

	// bind the texture and PBO
	texture->bind();
	m_context->bindPixelUnpackBuffer(m_bufferId);

	// copy pixels from PBO to texture object. uses offset instead of pointer
	glTexSubImage2D((GLenum)texture->getTextureType(), 0, 0, 0,
		texture->getWidth(), texture->getHeight(),
	    texture->getGLFormat(), texture->getGLType(),
	    0);

	if (!dontUnbind)
		m_context->bindPixelUnpackBuffer(0);
}

Bool PixelBuffer::checkData()
{
	bindUnpackBuffer();

	Int32 bufferSize = 0;
	glGetBufferParameteriv(GL_PIXEL_UNPACK_BUFFER, BUFFER_SIZE, (GLint*)&bufferSize);
	return (bufferSize > 0);
}*/


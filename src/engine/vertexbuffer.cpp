/**
 * @file vertexbuffer.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/vertexbuffer.h"
#include "o3d/engine/context.h"

using namespace o3d;

// Default constructor.
VertexBuffer::VertexBuffer(Context *context, Storage storageType) :
    BufferObject(context),
	m_count(0),
	m_storageType(storageType),
	m_lockCount(0),
    m_lockFlags(0)
{
}

// Bind the VBO if necessary.
void VertexBuffer::bindArrayBuffer() const
{
	m_context->bindVertexBuffer(m_bufferId);
}

// Unbind the VBO (bind the VBO 0).
void VertexBuffer::unbindArrayBuffer() const
{
	m_context->bindVertexBuffer(0);
}

// Bind the VBO if necessary.
void VertexBuffer::bindElementArrayBuffer() const
{
	m_context->bindIndexBuffer(m_bufferId);
}

// Unbind the VBO (bind the VBO 0).
void VertexBuffer::unbindElementArrayBuffer() const
{
	m_context->bindIndexBuffer(0);
}

// Attribute to a specific vertex array by its index.
void VertexBuffer::attributeVertex(
		UInt32 index,
		UInt32 eltSize,
		UInt32 stride,
		UInt32 offset)
{
	// Bind the VBO if necessary
	m_context->bindVertexBuffer(m_bufferId);
	m_context->vertexAttribArray(index, eltSize, stride, offset);
}

// Delete VBO.
void VertexBuffer::deleteVertexBuffer()
{
    if (m_bufferId != O3D_UNDEFINED) {
		O3D_ASSERT(m_lockCount == 0);

		O3D_GFREE(MemoryManager::GPU_VBO, m_bufferId);

		m_context->deleteVertexBuffer(m_bufferId);

		m_bufferId = O3D_UNDEFINED;
		m_count = 0;
	}
}

// Delete IBO.
void VertexBuffer::deleteElementArrayBuffer()
{
    if (m_bufferId != O3D_UNDEFINED) {
		O3D_ASSERT(m_lockCount == 0);

		O3D_GFREE(MemoryManager::GPU_VBO, m_bufferId);

		m_context->deleteIndexBuffer(m_bufferId);

		m_bufferId = O3D_UNDEFINED;
		m_count = 0;
	}
}

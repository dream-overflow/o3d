/**
 * @file uniformbuffer.h
 * @brief Uniform buffer object management.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-01
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/glextdefines.h"

#include "o3d/engine/uniformbuffer.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/shader/shadable.h"

using namespace o3d;

UniformBuffer::UniformBuffer(Context *context) :
    BufferObject(context),
    m_size(0)
{
    O3D_ASSERT(m_context != nullptr);
}

UniformBuffer::~UniformBuffer()
{
    release();
}

void UniformBuffer::bind()
{
    m_context->bindUniformBuffer(m_bufferId);
}

void UniformBuffer::unbind()
{
    if (m_context->getCurrentUniformBuffer() == m_bufferId) {
        m_context->bindUniformBuffer(0);
    }
}

void UniformBuffer::create(UInt8 *data, UInt32 size, Bool dontUnbind)
{
    if (m_bufferId == O3D_UNDEFINED) {
        glGenBuffers(1, (GLuint*)&m_bufferId);
    }

    m_context->bindUniformBuffer(m_bufferId);

    glBufferData(GL_UNIFORM_BUFFER, size, (const GLvoid*)data, GL_DYNAMIC_DRAW);

    m_size = size;

    // unbind the buffer
    if (!dontUnbind) {
        m_context->bindUniformBuffer(0);
    }
}

void UniformBuffer::release()
{
    if (m_bufferId != O3D_UNDEFINED) {
        glDeleteBuffers(1, (GLuint*)&m_bufferId);

        m_bufferId = O3D_UNDEFINED;
        m_size = 0;
    }
}

void UniformBuffer::update(UInt8 *data)
{
    if (m_context->getCurrentUniformBuffer() != m_bufferId) {
        bind();
    }

    // again we map the buffer to userCounters, but this time for read-only access
    GLuint *ldata = (GLuint*)glMapBufferRange(
                GL_UNIFORM_BUFFER,
                0,
                m_size,
                GL_MAP_WRITE_BIT);

    // copy
    memcpy(ldata, data, m_size);

    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

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
    m_context(context),
    m_id(O3D_UNDEFINED)
{
    O3D_ASSERT(m_context != nullptr);
}

UniformBuffer::~UniformBuffer()
{
    release();
}

void UniformBuffer::create(UInt32 size, Bool dontUnbind)
{
    if (m_id == O3D_UNDEFINED) {
        glGenBuffers(1, (GLuint*)&m_id);
    }

}

void UniformBuffer::release()
{
    if (m_id != O3D_UNDEFINED) {
        glDeleteBuffers(1, (GLuint*)&m_id);
    }
}

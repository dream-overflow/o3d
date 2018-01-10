/**
 * @file atomiccounter.cpp
 * @brief Atomic counter buffer object management.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-10
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/glextdefines.h"

#include "o3d/engine/atomiccounter.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/context.h"

using namespace o3d;

AtomicCounter::AtomicCounter(Context *context) :
    m_context(context),
    m_id(O3D_UNDEFINED),
    m_count(0),
    m_counters(nullptr)
{
    O3D_ASSERT(m_context != nullptr);
}

AtomicCounter::~AtomicCounter()
{
    release();
}

void AtomicCounter::create(UInt32 count, Bool dontUnbind)
{
    if (m_id == O3D_UNDEFINED) {
        glGenBuffers(1, (GLuint*)&m_id);
    }

    m_context->bindAtomicCounter(m_id);

    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * count, nullptr, GL_DYNAMIC_DRAW);

    m_count = count;
    m_counters = new UInt32[count];

    // unbind the buffer
    if (!dontUnbind) {
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    }
}

void AtomicCounter::release()
{
    m_context->deleteAtomicCounter(m_id);
    deleteArray(m_counters);

    m_id = O3D_UNDEFINED;
    m_count = 0;
}

void AtomicCounter::bind()
{
    m_context->bindAtomicCounter(m_id);
}

void AtomicCounter::unbind()
{
    if (m_context->getCurrentAtomicCounter() == m_id) {
        m_context->bindAtomicCounter(0);
    }
}

void AtomicCounter::reset()
{
    O3D_ASSERT(m_context->getCurrentAtomicCounter() == m_id);

    memset(m_counters, 0, m_count * sizeof(UInt32));
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0 , sizeof(GLuint) * m_count, m_counters);
}

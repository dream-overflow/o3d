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
    BufferObject(context),
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
    if (m_bufferId == O3D_UNDEFINED) {
        glGenBuffers(1, (GLuint*)&m_bufferId);
    }

    m_context->bindAtomicCounter(m_bufferId);

    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * count, nullptr, GL_DYNAMIC_DRAW);

    m_count = count;
    m_counters = new UInt32[count];

    // unbind the buffer
    if (!dontUnbind) {
        m_context->bindAtomicCounter(0);
    }
}

void AtomicCounter::release()
{
    if (m_bufferId != O3D_UNDEFINED) {
        m_context->deleteAtomicCounter(m_bufferId);
        deleteArray(m_counters);

        m_bufferId = O3D_UNDEFINED;
        m_count = 0;
    }
}

void AtomicCounter::bind()
{
    m_context->bindAtomicCounter(m_bufferId);
}

void AtomicCounter::unbind()
{
    if (m_context->getCurrentAtomicCounter() == m_bufferId) {
        m_context->bindAtomicCounter(0);
    }
}

void AtomicCounter::enable(UInt32 bindingPoint)
{
    m_context->bindAtomicCounterBase(m_bufferId, bindingPoint);
}

void AtomicCounter::disable(UInt32 bindingPoint)
{
    m_context->bindAtomicCounterBase(0, bindingPoint);
}

void AtomicCounter::reset()
{
    if (m_context->getCurrentAtomicCounter() != m_bufferId) {
        bind();
    }

    // memset(m_counters, 0, m_count * sizeof(UInt32));
    // glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0 , sizeof(GLuint) * m_count, m_counters);

    // again we map the buffer to userCounters, but this time for read-only access
    GLuint *userCounters = (GLuint*)glMapBufferRange(
                GL_ATOMIC_COUNTER_BUFFER,
                0,
                sizeof(GLuint) * m_count,
                MAP_WRITE | MAP_INVALIDATE_BUFFER | MAP_UNSYNCHRONIZED);

    memset(userCounters, 0, m_count * sizeof(GLuint));

    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
}

void AtomicCounter::update(Bool barrier)
{
    if (m_context->getCurrentAtomicCounter() != m_bufferId) {
        bind();
    }

    if (barrier) {
        // wait for finished operation before update value
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
    }

    // again we map the buffer to userCounters, but this time for read-only access
    GLuint *userCounters = (GLuint*)glMapBufferRange(
                GL_ATOMIC_COUNTER_BUFFER,
                0,
                sizeof(GLuint) * m_count,
                MAP_READ);

    // copy locally
    memcpy(m_counters, userCounters, sizeof(GLuint) * m_count);

    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

    if (barrier) {
        glMemoryBarrier(0);
    }
}

UInt32 AtomicCounter::getCounter(UInt32 index) const
{
    if (index < m_count) {
        return m_counters[index];
    } else {
        O3D_ERROR(E_IndexOutOfRange("Atomic counter index"));
    }
}

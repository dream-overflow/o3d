/**
 * @file atomiccounter.h
 * @brief Atomic counter buffer object management.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-10
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_ATOMICCOUNTER_H
#define _O3D_ATOMICCOUNTER_H

#include "o3d/core/debug.h"
#include "o3d/core/memorydbg.h"

#include <vector>

#include "enginetype.h"

namespace o3d {

class Context;

/**
 * @brief Atomic counter object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-10
 */
class O3D_API AtomicCounter
{
public:

    enum Function
    {
        ATOMIC_INC = 0,
        ATOMIC_DEC,
    };

    /**
     * @brief AtomicCounter
     * @param context
     */
    AtomicCounter(Context *context);

    ~AtomicCounter();

    /**
     * @brief create
     * @param Number of counter (32 bits integers).
     * @param dontUnbint Keep bound is True, else unbind.
     */
    void create(UInt32 count, Bool dontUnbind = False);

    /**
     * @brief Destroy the atomic counter object.
     */
    void release();

    void bind();
    void unbind();

    /**
     * @brief Rest the atomics counters.
     * @note Must be bound.
     */
    void reset();

private:

    Context *m_context;
    Int32 m_id;

    UInt32 m_count;
    UInt32 *m_counters;
};

} // namespace o3d

#endif // _O3D_ATOMICCOUNTER_H

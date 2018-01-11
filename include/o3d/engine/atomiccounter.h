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

#include "bufferobject.h"
#include "enginetype.h"

namespace o3d {

class Context;

/**
 * @brief Atomic counter object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-01-10
 */
class O3D_API AtomicCounter : public BufferObject
{
public:

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
     * @brief Bind the atomics counters at binding point.
     * @param bindingPoint Valid binding point index.
     */
    void enable(UInt32 bindingPoint);

    /**
     * @brief Unbind the atomics counters from binding point.
     * @param bindingPoint Valid binding point index.
     */
    void disable(UInt32 bindingPoint);

    /**
     * @brief Rest the atomics counters.
     * @note Must be bound or it is bound automatically.
     */
    void reset();

    /**
     * @brief Read the values before access to the counters getter.
     * @note Must be bound or it is bound automatically.
     */
    void update();

    /**
     * @brief Get a specific counter value.
     * @param index Between 0 and count-1.
     * @return Value of the last update.
     */
    UInt32 getCounter(UInt32 index) const;

private:

    UInt32 m_count;
    UInt32 *m_counters;
};

} // namespace o3d

#endif // _O3D_ATOMICCOUNTER_H

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

    // @todo
};

} // namespace o3d

#endif // _O3D_ATOMICCOUNTER_H

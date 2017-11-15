/**
 * @file closable.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CLOSABLE_H
#define _O3D_CLOSABLE_H

#include "base.h"

namespace o3d {

/**
 * @brief Interface for closable objects.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-12-05
 */
class O3D_API Closable
{
public:

    virtual ~Closable() = 0;

    virtual void close() = 0;
};

} // namespace o3d

#endif // _O3D_CLOSABLE_H


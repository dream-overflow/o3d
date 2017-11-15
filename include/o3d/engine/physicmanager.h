/**
 * @file physicmanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PHYSICMANAGER_H
#define _O3D_PHYSICMANAGER_H

#include "o3d/core/base.h"

namespace o3d {

/**
 * @brief Physic manager interface.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-01-30
 */
class O3D_API PhysicManager
{
public:

    virtual ~PhysicManager() = 0;

    virtual void update() = 0;
};

} // namespace o3d

#endif // _O3D_PHYSICMANAGER_H


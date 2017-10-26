/**
 * @file movable.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MOVABLE_H
#define _O3D_MOVABLE_H

#include "o3d/core/baseobject.h"

namespace o3d {

/**
 * @brief Movable object interface. Specify if the object is static or dynamic.
 * @date 2008-07-02
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 */
class O3D_API Movable
{
public:

	O3D_DECLARE_INTERFACE(Movable)

	//! Is the object is static (unmovable) or dynamic (movable)
	virtual Bool isMovable() const = 0;
};

} // namespace o3d

#endif // _O3D_MOVABLE_H


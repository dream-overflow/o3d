/**
 * @file updatable.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_UPDATABLE_H
#define _O3D_UPDATABLE_H

#include "o3d/core/classinfo.h"

namespace o3d {

/**
 * @brief Interface providing an update method.
 * @date 2007-12-09
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API Updatable
{
public:

	O3D_DECLARE_INTERFACE(Updatable)

	//! The update is processed one time per frame. It must so update all dynamic
	//! content of an object, depend of its parents if there have some, and must
	//! respect the context for the update of its children.
	//! hasUpdated() must return True if a change has occurs during the update. If its
	//! parent update return true, children will to have return true too, because it
	//! use of a hierarchical update, so it affect all child nodes.
	virtual void update() = 0;

	//! Check if it has been modified since its last update
	virtual Bool hasUpdated() const = 0;
};

} // namespace o3d

#endif // _O3D_UPDATABLE_H


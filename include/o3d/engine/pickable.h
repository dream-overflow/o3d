/**
 * @file pickable.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PICKABLE_H
#define _O3D_PICKABLE_H

#include "o3d/image/color.h"
#include "o3d/core/classinfo.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Pickable object interface.
 * @date 2005-10-22
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API Pickable
{
public:

	O3D_DECLARE_INTERFACE(Pickable)

	//! Enable the picking processing of the object
	virtual void enablePicking() = 0;

	//! Disable the picking processing of the object
	virtual void disablePicking() = 0;

	//! Is the picking processing is enabled
	virtual Bool isPicking() const = 0;

    //! get pickable uint32 unique identifier
    virtual UInt32 getPickableId() = 0;
};

} // namespace o3d

#endif // _O3D_PICKABLE_H


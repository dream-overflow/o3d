/**
 * @file drawable.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DRAWABLE_H
#define _O3D_DRAWABLE_H

#include "drawinfo.h"
#include "o3d/geom/geometry.h"

namespace o3d {

class Frustum;

/**
 * @brief Interface providing a draw method.
 * @date 2007-12-09
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 */
class O3D_API Drawable
{
public:

	O3D_DECLARE_INTERFACE(Drawable)

	//! Draw the object.
	virtual void draw(const DrawInfo &drawInfo) = 0;

	//! Get the object draw scene draw type. It define an Scene compatible object draw type.
	//! With that, he scene can override the drawing state for a type of an object.
	virtual UInt32 getDrawType() const = 0;

	//! Is the object is visible
	virtual Bool getVisibility() const = 0;

	//! Enable the visibility state
	virtual void enableVisibility() = 0;

	//! Disable the visibility state
	virtual void disableVisibility() = 0;

    //! Is need a draw.
    virtual Bool isNeedDraw() const = 0;

	//! Check the global bounding volume with an AABBox.
	//! The outside is pointed by the normal direction of the six planes.
	//! @return Clipping_Inside if the plane clip back with the object bounding,
	//!         Clipping_Interstect if the plane clip front and back,
	//!         Clipping_Outside otherwise.
	virtual Geometry::Clipping checkBounding(const AABBox &bbox) const = 0;

	//! Check the global bounding volume with an infinite plane
	//! The outside is pointed by the normal direction of the plane.
	//! @return Clipping_Inside if the plane clip back with the object bounding,
	//!         Clipping_Interstect if the plane clip front and back,
	//!         Clipping_Outside otherwise.
	virtual Geometry::Clipping checkBounding(const Plane &plane) const = 0;

	//! Check the global bounding volume with the frustum.
	//! @return Clipping_Inside if the plane clip back with the object bounding,
	//!         Clipping_Interstect if the plane clip front and back,
	//!         Clipping_Outside otherwise.
	virtual Geometry::Clipping checkFrustum(const Frustum &frustum) const = 0;
};

} // namespace o3d

#endif // _O3D_DRAWABLE_H


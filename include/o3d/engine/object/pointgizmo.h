/**
 * @file pointgizmo.h
 * @brief Point gizmo object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_POINTGIZMO_H
#define _O3D_POINTGIZMO_H

#include "gizmo.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class PointGizmo
//-------------------------------------------------------------------------------------
//! Point gizmo:
//!  - dim[X] = nothing
//!	 - dim[Y] = nothing
//!  - dim[Z] = nothing
//--------------------------------------------------------------------------------------
class O3D_API PointGizmo : public Gizmo
{
public:

	O3D_DECLARE_CLASS(PointGizmo)

	//! Default constructor
	PointGizmo(BaseObject *pParent);

	//! Get the drawing type
	virtual UInt32 getDrawType() const;


	// test if a point is inside the gizmo
	virtual Bool isInside(Vector3 pos);
	// test if the gizmo is inside the frustrum view
	virtual Bool frustrumClip();

	virtual void update() {}

	// draw the gizmo
	virtual void draw(const DrawInfo &drawInfo);
};

} // namespace o3d

#endif // _O3D_POINTGIZMO_H


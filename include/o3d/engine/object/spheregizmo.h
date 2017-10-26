/**
 * @file spheregizmo.h
 * @brief Sphere gizmo
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SPHEREGIZMO_H
#define _O3D_SPHEREGIZMO_H

#include "gizmo.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SphereGizmo
//-------------------------------------------------------------------------------------
//! Sphere gizmo:
//!   - dim[X] = radius on X
//!   - dim[Y] = radius on Y
//!   - dim[Z] = radius on Z
//! The sphere is in fact an elipsoide.
//--------------------------------------------------------------------------------------
class O3D_API SphereGizmo : public Gizmo
{
public:

	O3D_DECLARE_CLASS(SphereGizmo)

	//! Default constructor
	SphereGizmo(
		BaseObject *pParent,
		Vector3 vX = Vector3(1.f,0.f,0.f),
		Vector3 vY = Vector3(0.f,1.f,0.f),
		Vector3 vZ = Vector3(0.f,0.f,1.f),
		Vector3 dim = Vector3(1.f,1.f,1.f));

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

#endif // _O3D_SPHEREGIZMO_H


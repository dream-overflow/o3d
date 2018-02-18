/**
 * @file squaregizmo.h
 * @brief Gizmo, a 3d helper object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SQUAREGIZMO_H
#define _O3D_SQUAREGIZMO_H

#include "gizmo.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SquareGizmo
//-------------------------------------------------------------------------------------
//! Square gizmo (normal is Z aligned):
//!   - dim[X] = 1/2 X
//!   - dim[Y] = 1/2 Y
//!   - dim[Z] = nothing
//--------------------------------------------------------------------------------------
class O3D_API SquareGizmo : public Gizmo
{
public:

	O3D_DECLARE_CLASS(SquareGizmo)

	//! Default constructor
	SquareGizmo(
		BaseObject *pParent,
		Vector3 vX = Vector3(1.f,0.f,0.f),
		Vector3 vY = Vector3(0.f,1.f,0.f),
		Vector3 vZ = Vector3(0.f,0.f,1.f),
		Vector3 dim = Vector3(1.f,1.f,1.f));

	//! Get the drawing type
    virtual UInt32 getDrawType() const override;

	// test if a point is inside the gizmo
    virtual Bool isInside(Vector3 pos) override;
	// test if the gizmo is inside the frustrum view
    virtual Bool frustrumClip() override;

    virtual void update() override {}

	// draw the gizmo
    virtual void draw(const DrawInfo &drawInfo) override;
};

} // namespace o3d

#endif // _O3D_SQUAREGIZMO_H

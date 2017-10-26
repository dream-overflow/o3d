/**
 * @file cylindergizmo.h
 * @brief Cylinder gizmo.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CYLINDERGIZMO_H
#define _O3D_CYLINDERGIZMO_H

#include "gizmo.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class CylinderGizmo
//-------------------------------------------------------------------------------------
//! A cube gizmo object
//--------------------------------------------------------------------------------------
class O3D_API CylinderGizmo : public Gizmo
{
public:

	O3D_DECLARE_CLASS(CylinderGizmo)

	// Constructor
	CylinderGizmo(
		BaseObject *pParent,
		Vector3 vX = Vector3(1.f,0.f,0.f),
		Vector3 vY = Vector3(0.f,1.f,0.f),
		Vector3 vZ = Vector3(0.f,0.f,1.f),
		Vector3 dim = Vector3(1.f,1.f,1.f));

	virtual ~CylinderGizmo(void);

	//! Get the drawing type
	virtual UInt32 getDrawType() const;


	// test if a point is inside the gizmo
	virtual Bool isInside(Vector3 pos);
	// test if the gizmo is inside the frustrum view
	virtual Bool frustrumClip();

	virtual void update() {}

	// draw the gizmo
	virtual void draw(const DrawInfo &drawInfo);

	// compute the value to add to the cell
	virtual Float metaCellValue(Vector3 pos);
};

} // namespace o3d

#endif // _O3D_CYLINDERGIZMO_H


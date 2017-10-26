/**
 * @file vectorgizmo.h
 * @brief Square Gizmo.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-04-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VECTORGIZMO_H
#define _O3D_VECTORGIZMO_H

#include "gizmo.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Gizmo
//-------------------------------------------------------------------------------------
//! Square gizmo (Z aligned):
//!   - dim[X] = nothing
//!   - dim[Y] = nothing
//!   - dim[Z] = lenght
//--------------------------------------------------------------------------------------
class O3D_API VectorGizmo : public Gizmo
{
public:

	O3D_DECLARE_CLASS(VectorGizmo)

	// Constructor
	VectorGizmo(
		BaseObject *pParent,
		Vector3 vZ = Vector3(1.f,0.f,0.f),
		Float dim = 1.f);

	//! Get the drawing type
	virtual UInt32 getDrawType() const;

	virtual void update() {}

	// drawn the gizmo
	virtual void draw(const DrawInfo &drawInfo);
};

} // namespace o3d

#endif // _O3D_VECTORGIZMO_H


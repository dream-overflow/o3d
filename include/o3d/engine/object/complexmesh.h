/**
 * @file complexmesh.h
 * @brief large memory for store data.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-05-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_COMPLEXMESH_H
#define _O3D_COMPLEXMESH_H

#include "o3d/core/file.h"
#include "o3d/core/memorydbg.h"
#include "o3d/core/templatemanager.h"
#include "o3d/engine/material/material.h"
#include "shadableobject.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class ComplexMesh
//-------------------------------------------------------------------------------------
//! Complex mesh is a special mesh with dynamic polygons and whoose will need extra 
//! large memory for store data. Then data are not stored, but only generated face by 
//! face. Nurbs and beziers surfaces are ComplexMesh.
//---------------------------------------------------------------------------------------
class O3D_API ComplexMesh : public ShadableObject
{
public:

	O3D_DECLARE_CLASS(ComplexMesh)

	//! Default constructor
	ComplexMesh(BaseObject *pParent) :
		ShadableObject(pParent)
	{}

	//! Destructor
	virtual ~ComplexMesh() {}


	//-----------------------------------------------------------------------------------
	// Settings
	//-----------------------------------------------------------------------------------


	// serialisation
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);
};

} // namespace o3d

#endif // _O3D_COMPLEXMESH_H


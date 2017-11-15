/**
 * @file beziersurface.h
 * @brief A computed surface with bezier method.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-04-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BEZIERSURFACE_H
#define _O3D_BEZIERSURFACE_H

#include "complexmesh.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class BezierSurface
//-------------------------------------------------------------------------------------
//! A computed surface with bezier method.
//---------------------------------------------------------------------------------------
class O3D_API BezierSurface : public ComplexMesh
{
public:

	O3D_DECLARE_CLASS(BezierSurface)

	// constructor
	BezierSurface(BaseObject *pParent) :
		ComplexMesh(pParent)
	{}

	// destructor
	virtual ~BezierSurface() {}

	//! Get the drawing type
	virtual UInt32 getDrawType() const;

	virtual void update() {}
	virtual void draw(const DrawInfo &drawInfo) {}

	// Serialization
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);
};

} // namespace o3d

#endif // _O3D_BEZIERSURFACE_H


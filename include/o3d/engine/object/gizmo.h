/**
 * @file gizmo.h
 * @brief Gizmo, a 3d helper object.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2003-03-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GIZMO_H
#define _O3D_GIZMO_H

#include "../scene/sceneobject.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Gizmo
//-------------------------------------------------------------------------------------
//! A gizmo in an invisible object used for :
//!  - hierarchy
//!  - clipping or frustum viewing
//!  - object and particule collider
//!  - reflector/deflector for particles and objects
//!  - use for complex object like metaballs
//! @todo See for the m_matrix (for Draw()) and maybe put gizmo as node...
//--------------------------------------------------------------------------------------
class O3D_API Gizmo : public SceneObject
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(Gizmo)

	//! Default constructor
	Gizmo(
		BaseObject *pParent,
		Vector3 vX = Vector3(1.f,0.f,0.f),
		Vector3 vY = Vector3(0.f,1.f,0.f),
		Vector3 vZ = Vector3(0.f,0.f,1.f),
		Vector3 dim = Vector3(1.f,1.f,1.f));

	//! set/get the dimensions of the gizmo
	inline void setDim(Vector3 dim) { m_dim = dim; }
	inline const Vector3& getDim()const { return m_dim; }
	inline Vector3& getDim() { return m_dim; }

	//! set/get the intensity of the meta-object
    inline void setMetaIntensity(Float intensity) { m_metaIntensity = intensity; }
	//!void autoSetMetaIntensity();
    inline Float getMetaIntensity()const { return m_metaIntensity; }

	//! set/get the radius of influence
    inline void setInfluenceRadius(Float influence) { m_influenceRadius = influence; }
    inline Float getInfluenceRadius()const { return m_influenceRadius; }

	//! set the axes of the gizmo
	void setAxe(Vector3 vX,Vector3 vY,Vector3 vZ);

	//! set an axis of the gizmo
	void setAxis(UInt32 axis, Vector3 v);

	//! get an axis of the gizmo
	Vector3 getAxis(UInt32 axis);

	//! test if a point is inside the gizmo
	virtual Bool isInside(Vector3 pos);
	//! test if the gizmo is inside the frustrum view
	virtual Bool frustrumClip();

	// serialisation
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

    Matrix4 m_axis;             //!< gizmo matrix

    Vector3 m_dim;				//!< dimensions fo the gizmo
    Float m_metaIntensity;      //!< intensity of the meta-object
    Float m_influenceRadius;    //!< radius of influence
};

} // namespace o3d

#endif // _O3D_GIZMO_H


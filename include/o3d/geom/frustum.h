/**
 * @file frustum.h
 * @brief Compute a frustum, and check for point,sphere or box if its in or not.
 * @author Frederic SCHERMA
 * @date 2005-08-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FRUSTUM_H
#define _O3D_FRUSTUM_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/math.h"
#include "plane.h"

namespace o3d {

class AABBox;
class AABBoxExt;
class OBBox;
class OBBoxExt;
class BCone;
class Matrix4;

//---------------------------------------------------------------------------------------
//! @class Frustum
//-------------------------------------------------------------------------------------
//! Compute a frustum, and check for point,sphere or box if its in or not.
//---------------------------------------------------------------------------------------
class O3D_API Frustum
{
public:

	//! Plane frustum.
	enum Planes
	{
		RIGHT_PLANE = 0,
		LEFT_PLANE,
		BOTTOM_PLANE,
		TOP_PLANE,
		FAR_PLANE,
		NEAR_PLANE
	/*	LEFT = 0,
		RIGHT,
		BOTTOM,
		TOP,
		NEAR,
		FAR*/
	};

	//! Call this every time the camera moves to update the frustum.
	void computeFrustum(const Matrix4 &projection, const Matrix4 &modelview);

	//! Get one a of the six planes of the frustum (read only).
	inline const Plane& getPlane(Planes plane) { return m_planes[plane]; }

	//! Takes a 3D point and returns true if it's inside of the frustum.
	Geometry::Clipping pointInFrustum(const Vector3& point) const;

	//! Takes a 3D point and returns true if it's inside of the frustum (without near and far).
	Geometry::Clipping pointInFrustumLight(const Vector3& point) const;

	//! Takes a 3D point and a radius and returns true if the sphere is inside of the frustum.
	Geometry::Clipping sphereInFrustum(const Vector3& point, Float radius) const;

	//! Takes a 3D point and a radius and returns true if the sphere is inside of the frustum (without near and far).
	Geometry::Clipping sphereInFrustumLight(const Vector3& point, Float radius) const;

	//! Takes the center and half the length of the cube.
	Geometry::Clipping boxInFrustum(const AABBox &box) const;

	//! Takes the center and half the length of the cube.
	Geometry::Clipping boxInFrustumLight(const AABBox &box) const;

	//! Takes the center and half the length of the cube.
	Geometry::Clipping boxInFrustum(const AABBoxExt &box) const;

	//! Takes the center and half the length of the cube.
	Geometry::Clipping boxInFrustumLight(const AABBoxExt &box) const;

	//! Returns the clip result between the frustum and a cone.
	Geometry::Clipping coneInFrustum(const BCone &cone) const;

	//! Returns the clip result between the frustum and a cone (without near and far).
	Geometry::Clipping coneInFrustumLight(const BCone &cone) const;

	//! Return the clip result between the frustum and a oriented bounding box.
	Geometry::Clipping boxInFrustum(const OBBox &box) const;

	//! Return the clip result between the frustum and a oriented bounding box (without near and far).
	Geometry::Clipping boxInFrustumLight(const OBBox &box) const;

	//! Return the clip result between the frustum and a oriented bounding box ext.
	Geometry::Clipping boxInFrustum(const OBBoxExt &box) const;

	//! Return the clip result between the frustum and a oriented bounding box ext (without near and far).
	Geometry::Clipping boxInFrustumLight(const OBBoxExt &box) const;

private:

	Plane m_planes[6];      //!< Six planes of the frustum.
};

} // namespace o3d

#endif // _O3D_FRUSTUM_H


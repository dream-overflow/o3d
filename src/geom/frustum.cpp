/**
 * @file frustum.cpp
 * @brief Implementation of Frustum.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-08-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/geom/frustum.h"

#include "o3d/geom/aabbox.h"
#include "o3d/geom/obbox.h"
#include "o3d/geom/bcone.h"
#include "o3d/core/matrix4.h"
#include "o3d/core/vector3.h"

using namespace o3d;

void Frustum::computeFrustum(
		const Matrix4 & _projectionMatrix,
		const Matrix4 & _modelViewMatrix)
{
    // extract frustum planes

#if (1)
    // standard version.
	Matrix4 clip = _projectionMatrix * _modelViewMatrix;
    // clip.transpose();
#else
    // optimized version, doesn't works with rotated and ortho projection matrices.
    Matrix4 clip;

	clip.setData(0, 0, _modelViewMatrix.m11() * _projectionMatrix.m11());
	clip.setData(1, 0, _modelViewMatrix.m21() * _projectionMatrix.m22());
	clip.setData(2, 0, _modelViewMatrix.m31() * _projectionMatrix.m33());
	clip.setData(3, 0, - _modelViewMatrix.m31());

	clip.setData(0, 1, _modelViewMatrix.m12() * _projectionMatrix.m11());
	clip.setData(1, 1, _modelViewMatrix.m22() * _projectionMatrix.m22());
	clip.setData(2, 1, _modelViewMatrix.m32() * _projectionMatrix.m33());
	clip.setData(3, 1, - _modelViewMatrix.m32());

	clip.setData(0, 2, _modelViewMatrix.m13() * _projectionMatrix.m11());
	clip.setData(1, 2, _modelViewMatrix.m23() * _projectionMatrix.m22());
	clip.setData(2, 2, _modelViewMatrix.m33() * _projectionMatrix.m33());
	clip.setData(3, 2, - _modelViewMatrix.m33());

	clip.setData(0, 3, _modelViewMatrix.m14() * _projectionMatrix.m11());
	clip.setData(1, 3, _modelViewMatrix.m24() * _projectionMatrix.m22());
	clip.setData(2, 3, _modelViewMatrix.m34() * _projectionMatrix.m33() + _projectionMatrix.m34());
	clip.setData(3, 3, - _modelViewMatrix.m34());
#endif

	Vector4 m1(clip.getRow1());
	Vector4 m2(clip.getRow2());
	Vector4 m3(clip.getRow3());
	Vector4 m4(clip.getRow4());

	Vector4 plane;

	// right plane
	plane = m4 - m1;
	m_planes[RIGHT_PLANE] = Plane(plane.x(), plane.y(), plane.z(), plane.w());
	m_planes[RIGHT_PLANE].normalize();

	// left plane
	plane = m4 + m1;
	m_planes[LEFT_PLANE] = Plane(plane.x(), plane.y(), plane.z(), plane.w());
	m_planes[LEFT_PLANE].normalize();

	// bottom plane
	plane = m4 + m2;
	m_planes[BOTTOM_PLANE] = Plane(plane.x(), plane.y(), plane.z(), plane.w());
	m_planes[BOTTOM_PLANE].normalize();

	// top plane
	plane = m4 - m2;
	m_planes[TOP_PLANE] = Plane(plane.x(), plane.y(), plane.z(), plane.w());
	m_planes[TOP_PLANE].normalize();

	// near plane
	plane = m4 + m3;
	m_planes[NEAR_PLANE] = Plane(plane.x(), plane.y(), plane.z(), plane.w());
	m_planes[NEAR_PLANE].normalize();

	// far plane
	plane = m4 - m3;
	m_planes[FAR_PLANE] = Plane(plane.x(), plane.y(), plane.z(), plane.w());
	m_planes[FAR_PLANE].normalize();
}

Geometry::Clipping Frustum::pointInFrustum(const Vector3& point) const
{
	Float distance;
	
	Geometry::Clipping result = Geometry::CLIP_INSIDE;

	// for the 6 sides
    for (UInt32 i = 0 ; i < 6 ; ++i) {
		// Compute the plane equation and check if the point is behind a side of the frustum
		distance = m_planes[i] * point;

        if (distance < 0.0f) {
			// The point was behind a side, so it's not in the frustum
			return Geometry::CLIP_OUTSIDE;
        } else if (distance == 0) {
			// intersection point/plane
			result = Geometry::CLIP_INTERSECT;
		}
	}
	// inside
	return result;
}

Geometry::Clipping Frustum::pointInFrustumLight(const Vector3& point) const
{
	Float distance;
	
	Geometry::Clipping result = Geometry::CLIP_INSIDE;

	// for the 6 sides
    for (UInt32 i = 0 ; i < 4 ; ++i) {
		// Compute the plane equation and check if the point is behind a side of the frustum
		distance = m_planes[i] * point;

        if (distance < 0.0f) {
			// The point was behind a side, so it's not in the frustum
			return Geometry::CLIP_OUTSIDE;
        } else if (distance == 0) {
			// intersection point/plane
			result = Geometry::CLIP_INTERSECT;
		}
	}
	// inside
	return result;
}

Geometry::Clipping Frustum::sphereInFrustum(const Vector3& point, Float radius) const
{
	Float distance;

	Geometry::Clipping result = Geometry::CLIP_INSIDE;

	// for the 6 sides
    for (UInt32 i = 0 ; i < 6 ; ++i) {
		// If the center of the sphere is farther away from the plane than the radius
		distance = m_planes[i] * point;

		// if this distance is < -sphere_radius
        if (distance < -radius) {
			// The point was behind a side, so it's not in the frustum
			return Geometry::CLIP_OUTSIDE;
		}
		
		// else if the distance is between +- radius
        if (fabs(distance) < radius) {
			// intersection sphere/plane
			result = Geometry::CLIP_INTERSECT;
		}
	}
	// inside
	return result;
}

Geometry::Clipping Frustum::sphereInFrustumLight(const Vector3& point, Float radius) const
{
	Float distance;

	Geometry::Clipping result = Geometry::CLIP_INSIDE;

	// for the 4 sides
    for (UInt32 i = 0 ; i < 4 ; ++i) {
		// If the center of the sphere is farther away from the plane than the radius
		distance = m_planes[i] * point;

		// if this distance is < -sphere_radius
        if (distance < -radius) {
			// The point was behind a side, so it's not in the frustum
			return Geometry::CLIP_OUTSIDE;
		}
		
		// else if the distance is between +- radius
        if (fabs(distance) < radius) {
			// intersection sphere/plane
			result = Geometry::CLIP_INTERSECT;
		}
	}
	// inside
	return result;
}

// Takes the center and half the length of the cube
Geometry::Clipping Frustum::boxInFrustum(const AABBox &box) const
{
	Float distance;
	/*Int32 OutPoint;
	Bool Out = False;

	// recherche des 8 angles de la box
	Vector3 corners[8];
	box.getCorners(corners);

	// for the 6 sides 
	for (UInt32 i = 0 ; i < 6 ; ++i)
	{
		OutPoint = 0;
		// Pour chaque angle de la box
		for (UInt32 j = 0 ; j < 8 ; ++j)
		{
			// Distance entre le point et le plan
			distance = m_planes[i] * corners[j];

			if (distance < 0.0f)
			{
				// On est derriere le plan
				OutPoint++;
				Out = True;
			}
		}

		// Tous les points sont en dehors
		if (OutPoint == 8)
			return O3DMath::ClipOutside;
	}

	if (Out)
		return Geometry::ClipIntersect;

	return Geometry::ClipInside;*/

	// optimized version for axis aligned box
	Vector3 min = box.getMin();
	Vector3 max = box.getMax();

	Vector3 p, n;
	Vector3 normal;

	Geometry::Clipping result = Geometry::CLIP_INSIDE;

	// for the 6 sides
    for (UInt32 i = 0 ; i < 6 ; ++i) {
		normal = m_planes[i].getNormal();

		p = min;
		if (normal.x() >= 0.f)
			p.x() = max.x();
		if (normal.y() >= 0.f)
			p.y() = max.y();
		if (normal.z() >= 0.f)
			p.z() = max.z();

		n = max;
		if (normal.x() >= 0.f)
			n.x() = min.x();
		if (normal.y() >= 0.f)
			n.y() = min.y();
		if (normal.z() >= 0.f)
			n.z() = min.z();

		// Distance entre le point et le plan
		distance = m_planes[i] * p;
		if (distance < 0.0f)
			return Geometry::CLIP_OUTSIDE;

		distance = m_planes[i] * n;
		if (distance < 0.0f)
			result = Geometry::CLIP_INTERSECT;
	}

	return result;
}

Geometry::Clipping Frustum::boxInFrustumLight(const AABBox &box) const
{
	Float distance;
	/*Int32 OutPoint;
	Bool Out = False;
	
	// recherche des 8 angles de la box
	O3DVector3 corners[8];
	box.getCorners(corners);
	
	// for the 6 sides 
    for (UInt32 i = 0 ; i < 4 ; ++i) {
		OutPoint = 0;
		// Pour chaque angle de la box
        for (UInt32 j = 0 ; j < 8 ; ++j) {
			// Distance entre le point et le plan
			distance = m_planes[i] * corners[j];

            if (distance < 0.0f) {
				// On est derriere le plan
				OutPoint++;
				Out = True;
			}
		}
		
		// Tous les points sont en dehors
		if (OutPoint == 8)
			return O3DMath::ClipOutside;
	}
	
	if (Out)
		return O3DMath::ClipIntersect;
	
	return O3DMath::ClipInside;*/

	// optimized version for axis aligned box
	Vector3 min = box.getMin();
	Vector3 max = box.getMax();

	Vector3 p, n;
	Vector3 normal;

	Geometry::Clipping result = Geometry::CLIP_INSIDE;

    // for the 4 sides
    for (UInt32 i = 0 ; i < 4 ; ++i) {
		normal = m_planes[i].getNormal();

		p = min;
		if (normal.x() >= 0.f)
			p.x() = max.x();
		if (normal.y() >= 0.f)
			p.y() = max.y();
		if (normal.z() >= 0.f)
			p.z() = max.z();

		n = max;
		if (normal.x() >= 0.f)
			n.x() = min.x();
		if (normal.y() >= 0.f)
			n.y() = min.y();
		if (normal.z() >= 0.f)
			n.z() = min.z();

		// Distance entre le point et le plan
		distance = m_planes[i] * p;
		if (distance < 0.0f)
			return Geometry::CLIP_OUTSIDE;

		distance = m_planes[i] * n;
		if (distance < 0.0f)
			result = Geometry::CLIP_INTERSECT;
	}

	return result;
}

Geometry::Clipping Frustum::boxInFrustum(const AABBoxExt &box) const
{
	// First check if the bounding sphere is out the frustum
    if (sphereInFrustum(box.getCenter(), box.getRadius()) == Geometry::CLIP_OUTSIDE) {
		return Geometry::CLIP_OUTSIDE;
    }
	
	Float distance;
	/*Int32 OutPoint;
	Bool Out = False;
	
	// recherche des 8 angles de la box
	O3DVector3 corners[8];
	box.getCorners(corners);
	
	// for the 6 sides 
    for (UInt32 i = 0 ; i < 6 ; ++i) {
		OutPoint = 0;
		// Pour chaque angle de la box
		for (UInt32 j = 0 ; j < 8 ; ++j)
		{
			// Distance entre le point et le plan
			distance = m_planes[i] * corners[j];
			
			if (distance < 0.0f)
			{
				// On est derriere le plan
				OutPoint++;
				Out = True;
			}
		}
		
		// Tous les points sont en dehors
		if (OutPoint == 8)
			return O3DClipping_Outside;
	}
	
	if (Out)
		return O3DClipping_Intersect;
	
	return O3DClipping_Inside;*/

	// optimized version for axis aligned box
	Vector3 min = box.getMin();
	Vector3 max = box.getMax();

	Vector3 p, n;
	Vector3 normal;

	Geometry::Clipping result = Geometry::CLIP_INSIDE;

	// for the 6 sides
    for (UInt32 i = 0 ; i < 6 ; ++i) {
		normal = m_planes[i].getNormal();

		p = min;
		if (normal.x() >= 0.f)
			p.x() = max.x();
		if (normal.y() >= 0.f)
			p.y() = max.y();
		if (normal.z() >= 0.f)
			p.z() = max.z();

		n = max;
		if (normal.x() >= 0.f)
			n.x() = min.x();
		if (normal.y() >= 0.f)
			n.y() = min.y();
		if (normal.z() >= 0.f)
			n.z() = min.z();

		// Distance entre le point et le plan
		distance = m_planes[i] * p;
		if (distance < 0.0f)
			return Geometry::CLIP_OUTSIDE;

		distance = m_planes[i] * n;
		if (distance < 0.0f)
			result = Geometry::CLIP_INTERSECT;
	}

	return result;
}

Geometry::Clipping Frustum::boxInFrustumLight(const AABBoxExt &box) const
{
	// First check if the bounding sphere is out the frustum
    if (sphereInFrustumLight(box.getCenter(), box.getRadius()) == Geometry::CLIP_OUTSIDE) {
		return Geometry::CLIP_OUTSIDE;
    }
	
	Float distance;
	/*Int32 OutPoint;
	Bool Out = False;
	
	// recherche des 8 angles de la box
	O3DVector3 corners[8];
	box.getCorners(corners);
	
	// for the 4 sides 
    for (UInt32 i = 0 ; i < 4 ; ++i) {
		OutPoint = 0;
		// Pour chaque angle de la box
        for (UInt32 j = 0 ; j < 8 ; ++j) {
			// Distance entre le point et le plan
			distance = m_planes[i] * corners[j];

            if (distance < 0.0f) {
				// On est derriere le plan
				OutPoint++;
				Out = True;
			}
		}
		
		// Tous les points sont en dehors
		if (OutPoint == 8)
			return O3DMath::ClipOutside;
	}
	
	if (Out)
		return O3DMath::ClipIntersect;

	return O3DClipping_Inside;*/

	// optimized version for axis aligned box
	Vector3 min = box.getMin();
	Vector3 max = box.getMax();

	Vector3 p, n;
	Vector3 normal;

	Geometry::Clipping result = Geometry::CLIP_INSIDE;

    // for the 4 sides
    for (UInt32 i = 0 ; i < 4 ; ++i) {
		normal = m_planes[i].getNormal();

		p = min;
		if (normal.x() >= 0.f)
			p.x() = max.x();
		if (normal.y() >= 0.f)
			p.y() = max.y();
		if (normal.z() >= 0.f)
			p.z() = max.z();

		n = max;
		if (normal.x() >= 0.f)
			n.x() = min.x();
		if (normal.y() >= 0.f)
			n.y() = min.y();
		if (normal.z() >= 0.f)
			n.z() = min.z();

		// Distance entre le point et le plan
		distance = m_planes[i] * p;
		if (distance < 0.0f)
			return Geometry::CLIP_OUTSIDE;

		distance = m_planes[i] * n;
		if (distance < 0.0f)
			result = Geometry::CLIP_INTERSECT;
	}

	return result;
}

//! returns the clip result between the frustum and a cone
Geometry::Clipping Frustum::coneInFrustum(const BCone &cone) const
{
	Geometry::Clipping lResult;

    if ((lResult = sphereInFrustum(cone.getOutBSphere().getCenter(),
                                   cone.getOutBSphere().getRadius())) != Geometry::CLIP_INTERSECT) {
		return lResult;
    }

	lResult = Geometry::CLIP_INSIDE;

    for (Int32 i = 0 ; i < 6 ; ++i) {
        switch(m_planes[i].clip(cone)) {
            case Geometry::CLIP_OUTSIDE:
                return Geometry::CLIP_OUTSIDE;
            case Geometry::CLIP_INTERSECT:
                lResult = Geometry::CLIP_INTERSECT;
                break;
            default:
                break;
		}
	}

	return lResult;
}

//! returns the clip result between the frustum and a cone (without near and far)
Geometry::Clipping Frustum::coneInFrustumLight(const BCone &cone) const
{
	Geometry::Clipping lResult;

    if ((lResult = sphereInFrustum(cone.getOutBSphere().getCenter(),
                                   cone.getOutBSphere().getRadius())) != Geometry::CLIP_INTERSECT) {
		return lResult;
    }

	lResult = Geometry::CLIP_INSIDE;

    for (Int32 i = 0 ; i < 4 ; ++i) {
        switch(m_planes[i].clip(cone)) {
            case Geometry::CLIP_OUTSIDE:
                return Geometry::CLIP_OUTSIDE;
            case Geometry::CLIP_INTERSECT:
                lResult = Geometry::CLIP_INTERSECT;
                break;
            default:
                break;
		}
	}

	return lResult;
}

// Clip with oriented bounding box
Geometry::Clipping Frustum::boxInFrustum(const OBBox &box) const
{
    // @todo
	return Geometry::CLIP_INSIDE;
}

Geometry::Clipping Frustum::boxInFrustumLight(const OBBox &box) const
{
    // @todo
	return Geometry::CLIP_INSIDE;
}

Geometry::Clipping Frustum::boxInFrustum(const OBBoxExt &box) const
{
	// First check if the bounding sphere is out the frustum
    if (sphereInFrustum(box.getCenter(), box.getRadius()) == Geometry::CLIP_OUTSIDE) {
		return Geometry::CLIP_OUTSIDE;
    }

    // @todo
	return Geometry::CLIP_INSIDE;
}

Geometry::Clipping Frustum::boxInFrustumLight(const OBBoxExt &box) const
{
	// First check if the bounding sphere is out the frustum
    if (sphereInFrustumLight(box.getCenter(), box.getRadius()) == Geometry::CLIP_OUTSIDE) {
		return Geometry::CLIP_OUTSIDE;
    }

    // @todo
	return Geometry::CLIP_INSIDE;
}

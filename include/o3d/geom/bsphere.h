/**
 * @file bsphere.h
 * @brief Bounding Sphere
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author  Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2005-08-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BSPHERE_H
#define _O3D_BSPHERE_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/vector3.h"
#include "geometry.h"

namespace o3d {

class Plane;
class BCone;
class AABBox;
class Matrix4;

/**
 * @brief A definition of a bounding sphere.
 */
class O3D_API BSphere
{
public:

	Vector3 m_center;
	Float m_radius;

public:

	//-----------------------------------------------------------------------------------
	// Constructors
	//-----------------------------------------------------------------------------------

	//! Default constructor.
	BSphere() {}

	//! Copy from another sphere.
	BSphere(const BSphere &refSphere) :
		m_center(refSphere.m_center),
		m_radius(refSphere.m_radius)
	{}

	//! Construct from a center and radius
	BSphere(const Vector3 &refCenter, Float radius) :
		m_center(refCenter),
		m_radius(radius)
	{}

	//! Construct from a cone
	//! @param refCenter the position of the origin of the cone
	//! @param dir the direction of the cone
	//! @param cutoff the cutoff angle in radian (in [0;pi/2[ if spheric is false, in [0;pi] otherwise)
	//! @param depth the length of the cone
	//! @param spheric defines the type of cone (spherical (spotlight) or plane (pyramid) base)
	//! @param inside means that this function will returns the included sphere (if true), or the bounding sphere (if false)
	BSphere(	const Vector3 &refCenter,
				const Vector3 &dir,
				Float cutoff,
				Float depth,
				Bool spheric,
				Bool inside = False);

	//-----------------------------------------------------------------------------------
	// Global parameters
	//-----------------------------------------------------------------------------------

	//! Get the center.
	const Vector3& getCenter() const { return m_center; }

	//! Get the radius.
	Float getRadius() const { return m_radius; }

	//! Set the center.
	void setCenter(const Vector3 &refCenter) { m_center = refCenter; }

	//! Set the radius.
	void setRadius(Float radius) { m_radius = radius; }

	//-----------------------------------------------------------------------------------
	// Transform
	//-----------------------------------------------------------------------------------

	//! Compute res= mat * this. NB: radius is maximized, taking max of the 3 axis of the matrix.
	//! @note This may be false if the matrix is not orthogonal.
	void applyTransform(const Matrix4 &mat, BSphere &res) const;

	//-----------------------------------------------------------------------------------
	// Clipping
	//-----------------------------------------------------------------------------------

	//! Is the bsphere partially in front of the plane??  p MUST be normalized.
	Bool clipFront(const Plane &p) const;

	//! Is the bsphere partially in back of the plane??  p MUST be normalized.
	Bool clipBack(const Plane &p) const;

	//! Does the sphere include this point?
	Bool include(const Vector3 &p) const;

	//! Does the sphere include TOTALY this sphere?
	Bool include(const BSphere &s) const;

	//! Does the sphere include TOTALY a box
	Bool include(const AABBox &b) const;

	//! Does the sphere intersect the other?
	Bool intersect(const BSphere &s) const;

	//! Does the sphere intersect the box
	//! Since 'Intersection' is commutative, this function is equivalent to
	//! O3DAABBox::intersect(const O3DBSphere &) const;
	Bool intersect(const AABBox &b) const;

	//! Does the sphere intersect the plane
	//! Since 'Intersection' is commutative, this function is equivalent to
	//! O3DAABBox::intersect(const O3DBSphere &) const;
	Bool intersect(const Plane &b) const;

	//! Does the sphere intersect the plane
	//! Since 'Intersection' is commutative, this function is equivalent to
	//! O3DAABBox::intersect(const O3DBSphere &) const;
	Bool intersect(const BCone &cone) const;

	//! Clip a plane
	//! Warning: this operation is not commutative: X.clip(Y) != Y.clip(X)
	//! @return CLIP_INSIDE means the plane is inside the sphere (IMPOSSIBLE case !!!)
	//!         CLIP_OUTSIDE means the plane (in argument) is outside the sphere (not even in contact)
	//!         CLIP_INTERSECT all the others cases
	Geometry::Clipping clip(const Plane & plane) const;

	//! Clip a sphere
	//! Warning: this operation is not commutative: X.clip(Y) != Y.clip(X)
	//! @return CLIP_INSIDE means the sphere (in argument) is inside the sphere (or in contact)
	//!         CLIP_OUTSIDE means the sphere (in argument) is outside the sphere (not even in contact)
	//!         CLIP_INTERSECT all the others cases
	Geometry::Clipping clip(const BSphere & sphere) const;

	//! @brief Does the sphere intersect the cone
	//! Warning: this operation is not commutative: X.clip(Y) != Y.clip(X)
	//! @param cone the cone
	//! @return CLIP_INSIDE means the cone is inside the sphere (or in contact)
	//!         CLIP_OUTSIDE means the cone is outside the sphere (not even in contact)
	//!         CLIP_INTERSECT all the others cases
	Geometry::Clipping clip(const BCone &) const;

	//! Build the union of the 2 sphere and set to *this. work if this==s1 || this==s2.
	void setUnion(const BSphere &sa, const BSphere &sb);

	//-------------------------------------------------------------------------------
	// Serialization
	//-------------------------------------------------------------------------------

	Bool writeToFile(OutStream &os) const;
	Bool readFromFile(InStream &is);
};

} // namespace o3d

#endif // _O3D_BSPHERE_H

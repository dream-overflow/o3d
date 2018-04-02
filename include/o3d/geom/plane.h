/**
 * @file plane.h
 * @brief 3D Plane
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author  Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2005-08-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PLANE_H
#define _O3D_PLANE_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/vector3.h"
#include "geometry.h"

namespace o3d {

class BSphere;
class BCone;

/**
 * @brief Definition of a plane function. Useful for compute intersection.
 * A vector v is said "front" of a plane p if p*v>0.
 */
class O3D_API Plane
{
public:

	//-----------------------------------------------------------------------------------
	// Constructor
	//-----------------------------------------------------------------------------------

	//! Default constructor.
	Plane();

	//! Construct from a normal and position.
	Plane(const Vector3 &normal, const Vector3 &position);

	//! Construct with a vector normal representing a,b,c and a float d.
	Plane(const Vector3 &refNormal, Float d);

	//! Construct with a 4 float representing a,b,c and d.
	Plane(Float a, Float b, Float c, Float d);

	//! Copy from another O3DPlane.
	Plane(const Plane &refP);

	//-----------------------------------------------------------------------------------
	// Plane construction method / General parameters
	//-----------------------------------------------------------------------------------

	//! Make a plane with a normal and a vertex.
	//! The normal is normalized by Make().
	void make(const Vector3 &normal, const Vector3 &pos);

	//! Make a plane with 3 vertices.
	//! @note the plane normal is normalized by Make().
	void make(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2);

	//! Return the normal vector of the plane.
	//! Since the normal of the plane may not be normalized (if setup without make()), the returned normal.
	//! may NOT be normalized.
	const Vector3& getNormal() const;

	//! Returns a point from the plane
	const Vector3 getPoint() const;

	//! Normalize the plane, such that getNormal() return a normalized vector.
	void normalize();

	//! Return the normalized version of a plane. @see Normalize().
	Plane normed() const;

	//! Get the inverted version of this plane (same position, but inverted normal).
	Plane inverted() const;

	//! Invert this plane (same position, but inverted normal).
	void invert();

	//-----------------------------------------------------------------------------------
	// Projections / Clipping
	//-----------------------------------------------------------------------------------

	//! Return the distance of p from the plane. Hence, the result is >=0.
	//! since the plane normal may not be normalized, distance() compute the distance with the normalized normal
	//! of plane. If you are sure that your plane has a normalized normal, it is much faster to do a \c fabs(p*v).
	Float distance(const Vector3 &p) const;

	//! Intersect a line onto a plane. p1 is returned if line // to plane.
	Vector3 intersect(const Vector3 &P1,const Vector3 &P2) const;

	//! Intersect a line onto a plane. p1 is returned if line // to plane.
	Vector3 intersect(const Vector3 &P1, const Vector3 &P2, Float ray) const;

	//! Intersects with a sphere
	//! Equivalent to (this.clip(sphere) = CLIP_INSIDE) || (this.clip(sphere) = CLIP_INTERSECT)
	//! but in general, the function 'Intersect' would be more efficient.
	//! If you wish to know if the intersection of the sphere and the plane is not null, you
    //! must use BSphere::intersect(const O3DPlane &) const.
	//! @return True if the sphere intersects the plane or if the sphere is inside the plane,
    //!         that is to say behind the plane (compared to its normal)
    //! @see BSphere::intersect(const O3DPlane &) const
	Bool intersect(const BSphere & sphere) const;

	//! Intersects with a cone
	//! Equivalent to (this.clip(cone) = CLIP_INSIDE) || (this.clip(cone) = CLIP_INTERSECT)
	//! but in general, the function 'Intersect' would be more efficient.
	//! Warning : this function is not equivalent to O3DBCone::intersect(const O3DPlane &).
	//! If you wish to know if the intersection of the cone and the plane is not null, you
    //! must use BCone::intersect(const O3DPlane &) const.
	//! @return True if the cone intersects the plane or if the cone is inside the plane,
    //!         that is to say behind the plane (compared to its normal)
    //! @see BCone::intersect(const O3DPlane &) const
	Bool intersect(const BCone & cone) const;

	//! Project a point onto a plane.
	Vector3 project(const Vector3 &p0) const;

	//! Clip a segment onto a plane.
	//! The "back segment" is written in (p1,p2). If segment is entirely "front", (p1,p2) is not modified.
	//! @return false if segment entirely front, or true.
	Bool clipSegmentBack(Vector3 &p0, Vector3 &p1) const;

	//! The "front segment" is written in (p1,p2). If segment is entirely "back", (p1,p2) is not modified.
	//! @return false if segment entirely back, or true.
	Bool clipSegmentFront(Vector3 &p0, Vector3 &p1) const;

	//! Clip a polygon by a plane. The "back polygon" is returned.
	//! @note Out must be allocated to nIn+1 (at less).
	//! @return the number of vertices of out. 0 is returned if In polygon entirely front, or if nIn<=2.
	Int32 clipPolygonBack(const Vector3 *inPoly, Vector3 *outPoly, Int32 numVertices) const;

	//! Clip a polygon by a plane. The "front polygon" is returned.
	//! @note Out must be allocated to nIn+1 (at less).
	//! @return the number of vertices of out. 0 is returned if In polygon entirely back, or if nIn<=2.
	Int32 clipPolygonFront(const Vector3 *inPoly, Vector3 *outPoly, Int32 numVertices) const;

	//! @brief Clip between the plane and a cone
	//! @param cone the cone
	Geometry::Clipping clip(const BCone & cone) const;

	//! @brief Clip between the plane and a sphere
	//! @param sphere the sphere
	Geometry::Clipping clip(const BSphere & sphere) const;

	//-----------------------------------------------------------------------------------
	// Operators
	//-----------------------------------------------------------------------------------

	//! Return plane*vector.
	inline Float operator*(const Vector3 &p) const
	{
		return m_normal*p + m_d;
	}

	//! Compare two plane using an epsilon.
	inline Bool operator==(const Plane &P) const
	{
        if (P.m_d > m_d + Limits<Float>::epsilon()) {
			return False;
        }
        if (P.m_d < m_d - Limits<Float>::epsilon()) {
			return False;
        }
        if (P.m_normal != m_normal) {
			return False;
        }

		return True;
	}

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	Bool writeToFile(OutStream &os) const;
	Bool readFromFile(InStream &is);

public:

	Vector3 m_normal;
	Float m_d;
};

} // namespace o3d

#endif // _O3D_PLANE_H

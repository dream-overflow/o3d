/**
 * @file aabbox.h
 * @brief Axis Aligned Bounding Box
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-08-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_AABBOX_H
#define _O3D_AABBOX_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/vector3.h"
#include "o3d/core/box.h"
#include "geometry.h"

namespace o3d {

class Plane;
class BSphere;
class BCone;
class Matrix4;

//---------------------------------------------------------------------------------------
//! @class AABBox
//-------------------------------------------------------------------------------------
//! A definition of a Axis Aligned Bounding Box. Useful for compute intersection.
//---------------------------------------------------------------------------------------
class O3D_API AABBox
{
public:

	//-----------------------------------------------------------------------------------
	// Constructors
	//-----------------------------------------------------------------------------------

	//! Default constructor.
	AABBox() {}

	//! Construct given an center and a half size.
	AABBox(const Vector3 &refCenter, const Vector3 &refHalfSize) :
		m_center(refCenter),
		m_halfSize(refHalfSize)
	{}

	//! Construct a box adapted to a cone
	AABBox(const BCone &cone);

	//! Copy from another box.
	AABBox(const AABBox &refBBox) :
		m_center(refBBox.m_center),
		m_halfSize(refBBox.m_halfSize)
	{}

	//-----------------------------------------------------------------------------------
	// Generals Parameters define
	//-----------------------------------------------------------------------------------

	//! Reset value to zero.
	inline void destroy()
	{
		m_center.set();
		m_halfSize.set();
	}

	//! Set the center.
	inline void setCenter(const Vector3 &refCenter) { m_center = refCenter; }

	//! Set the half size.
	inline void setHalfSize(const Vector3 &refHalfSize) { m_halfSize = refHalfSize; }

	//! Set the size.
	inline void setSize(const Vector3 &refSize) { m_halfSize = refSize * 0.5f; }

	//! Construct using min and max.
	inline void setMinMax(const Vector3 &refMin, const Vector3 &refMax)
	{
		m_center = (refMin+refMax) * 0.5f;
		m_halfSize = refMax - m_center;
	}

	//! Extend the bbox so it contains.
	void extend(const Vector3 &refV);

	//-----------------------------------------------------------------------------------
	// Generals Parameters getter
	//-----------------------------------------------------------------------------------

	//! Get the min coordinates
	inline Vector3 getMin() const { return m_center-m_halfSize; }

	//! Get the max coordinates
	inline Vector3 getMax() const { return m_center+m_halfSize; }

	//! Get the center.
	inline const Vector3& getCenter() const { return m_center; }

	//! Get the half size.
	inline const Vector3& getHalfSize() const { return m_halfSize; }

	//! Get the size.
	inline Vector3 getSize() const { return m_halfSize*2.f; }

	//! Get the radius (length of the half size).
	inline Float getRadius() const { return m_halfSize.length(); }

	//-----------------------------------------------------------------------------------
	// Clipping functions
	//-----------------------------------------------------------------------------------

	//! Is the bbox partially in front of the plane??
	Bool clipFront(const Plane &p) const;

	//! Is the bbox partially in back of the plane??
	Bool clipBack(const Plane &p) const;

	//! Does the bbox include a point.
	Bool include(const Vector3 &a) const;

	//! Does the bbox include a 2d box. Compare X and Y, and assumes no Z.
	Bool include(const Box2f &b) const;

	//! Does the bbox include entirely a bbox.
	Bool include(const AABBox &box) const;

	//! Does the bbox include entirely a sphere.
	Bool include(const BSphere &s) const;

	//! Does the bbox intersect the a box.
	Bool intersect(const AABBox &box) const;

	//! Does the bbox intersect the triangle ABC.
	Bool intersect(const Vector3 &a, const Vector3 &b, const Vector3 &c) const;

	//! Does the bbox instersect the sphere s
	Bool intersect(const BSphere &s) const;

	//! Does the bbox instersect the segment AB
	Bool intersect(const Vector3 &a, const Vector3 &b) const;

	//! Does the bbox intersect with this 2d box. Compare X and Y, and assumes no Z.
	Bool intersect(const Box2f &b) const;

	//! @brief Does the bbox intersect a bbox
	Geometry::Clipping clip(const AABBox & box) const;

	//! @brief Does the bbox intersect a sphere
	Geometry::Clipping clip(const BSphere & s) const;

	//! @brief Does the bbox intersect the cone
	//! Prefer clipping with O3DBSphere which is faster.
	//! @return CLIP_INSIDE means the cone (in argument) is inside the box (or in contact)
	//!         CLIP_OUTSIDE means the cone (in argument) is outside the box (not even in contact)
	//!         CLIP_INTERSECT all the others cases
	Geometry::Clipping clip(const BCone & cone) const;

	//! clip the segment by the bbox. return false if don't intersect. a and b are modified.
	Bool clipSegment(Vector3 &a, Vector3 &b) const;

	//-----------------------------------------------------------------------------------
	// Miscs functions
	//-----------------------------------------------------------------------------------

	//! Build the equivalent set of planes (left, right, bottom, top, front, back).
	void toPlanes(Plane planes[6]) const;

	//! Get the 8 corners of this box
	void getCorners(Vector3 (&vects)[8]) const;

	//! Clamp a point coordinate into the bbox.
	Vector3 clamp(const Vector3 &point) const;

	//! Compute the intersection of 2 bboxs.
	//! @note this methods suppose the intersection exist, and doesn't check it (use intersect() to check).
	//! If !intersect, *this is still modified and the result bbox is undefined.
	static AABBox buildIntersection(const AABBox &b1, const AABBox &b2);

	//! Compute the union of 2 bboxs, that is the aabbox that contains the 2 others.
	static AABBox buildUnion(const AABBox &b1, const AABBox &b2);

	//! Apply a matrix transformation on an aabbox
	static AABBox transformAABBox(const Matrix4 &mat, const AABBox &box);

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	Bool writeToFile(OutStream &os) const;
	Bool readFromFile(InStream &is);

protected:

	Vector3 m_center;
	Vector3 m_halfSize;
};

//---------------------------------------------------------------------------------------
//! @class AABBoxExt
//-------------------------------------------------------------------------------------
//! A definition of an extended Axis Aligned Bounding Box.
//! Use a sphere to improve clipping tests
//---------------------------------------------------------------------------------------
class O3D_API AABBoxExt : public AABBox
{
public:

	//! Default constructor.
	AABBoxExt() {}

	//! Construct from a center and a half size.
	AABBoxExt(const Vector3 &refCenter, const Vector3 &refHalfSize) :
		AABBox(refCenter, refHalfSize)
	{
		updateRadius();
	}

	//! Copy from another box.
	AABBoxExt(const AABBox &refBBox) :
		AABBox(refBBox)
	{
		updateRadius();
	}

	//-----------------------------------------------------------------------------------
	// Generals Parameters define
	//-----------------------------------------------------------------------------------

	//! Set the half size.
	inline void setHalfSize(const Vector3 &refHalfSize)
	{
		m_halfSize = refHalfSize;
		updateRadius();
	}

	//! Set the size.
	inline void setSize(const Vector3 &refSize)
	{
		m_halfSize = refSize * 0.5f;
		updateRadius();
	}

	//! Construct using min and max coordinates.
	inline void setMinMax(const Vector3 &refMin, const Vector3 &refMax)
	{
		m_center = (refMin+refMax) * 0.5f;
		m_halfSize = refMax - m_center;
		updateRadius();
	}

	//-----------------------------------------------------------------------------------
	// Generals Parameters getter
	//-----------------------------------------------------------------------------------

	//! Return the STORED radius of the bbox.
	inline Float getRadius() const { return m_radiusMax; }

	//! return a simple Axis Aligned Bounding Box (no radius inside)
	inline AABBox getAABBox() const { return AABBox(m_center, m_halfSize); }

	//! Apply a matrix transformation on an aabbox and return the result as new a bounding box
	AABBoxExt transformTo(const Matrix4 &mat) const;

	//-----------------------------------------------------------------------------------
	// Clipping functions
	//-----------------------------------------------------------------------------------

	//! Is the bbox partially in front of the plane?? p MUST be normalized.
	Bool clipFront(const Plane &p) const;

	//! Is the bbox partially in back of the plane?? p MUST be normalized.
	Bool clipBack(const Plane &p) const;

	//! Does the bbox intersect the bbox box.
	inline Bool intersect(const AABBoxExt &box) const
	{
		return AABBox::intersect(box);
	}

	//! Does the bbox intersect the triangle ABC.
	inline Bool intersect(const Vector3 &a, const Vector3 &b, const Vector3 &c) const
	{
		return AABBox::intersect(a,b,c);
	}

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	Bool writeToFile(OutStream &os) const;
	Bool readFromFile(InStream &is);

protected:

	Float m_radiusMin;
	Float m_radiusMax;

	//! Update the radius of our bbox
	void updateRadius()
	{
		// The bounding sphere.
		m_radiusMax = AABBox::getRadius();

		// The including sphere.
		m_radiusMin = minMin(
				(Float)fabs(m_halfSize[0]),
				(Float)fabs(m_halfSize[1]),
				(Float)fabs(m_halfSize[2]));
	}
};

} // namespace o3d

#endif // _O3D_AABBOX_H


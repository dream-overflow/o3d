/**
 * @file obbox.h
 * @brief Oriented bounding box.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-07-29
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_OBBOX_H
#define _O3D_OBBOX_H

#include "o3d/core/matrix3.h"

namespace o3d {

class Plane;

/**
 * @brief Oriented bounding box.
 */
class O3D_API OBBox
{
public:

	//! Default constructor.
	OBBox() {}

	//! Construct with the 3 direction, center and half size.
	//! @param The first column contain the primary largest axis, the second contain the second largest.
	OBBox(
			const Matrix3 &data,
			const Vector3 &center,
			const Vector3 &halfSize) :
		m_data(data),
		m_center(center),
		m_halfSize(halfSize)
	{
	}

	//! Construct with the 3 direction, center and half size.
	//! @param r Primary largest axis.
	//! @param s Secondary largest axis.
	//! @param t Third largest axis.
	OBBox(
			const Vector3 &r,
			const Vector3 &s,
			const Vector3 &t,
			const Vector3 &center,
			const Vector3 &halfSize) :
		m_data(r, s, t),
		m_center(center),
		m_halfSize(halfSize)
	{
	}

	//! Set the three primaries directions.
	//! @param r Primary largest axis.
	//! @param s Secondary largest axis.
	//! @param t Third largest axis.
	inline void setRST(
			const Vector3 &r,
			const Vector3 &s,
			const Vector3 &t)
	{
		m_data.setRow1(r);
		m_data.setRow2(s);
		m_data.setRow3(t);
	}

	//! Get the primary direction (r).
	inline Vector3 getR() const { return m_data.getRow1(); }

	//! Get the secondary direction (s).
	inline Vector3 getS() const { return m_data.getRow2(); }

	//! Get the third direction (t).
	inline Vector3 getT() const { return m_data.getRow3(); }

	//! Get the 3x3 matrix.
	inline const Matrix3& getData() const { return m_data; }
	//! Set the 3x3 matrix.
	//! @param The first column contain the primary largest axis, the second contain the second largest.
	inline void setData(const Matrix3 &data) { m_data = data; }

	//! Get the center of the box.
	inline const Vector3& getCenter() const { return m_center; }
	//! Set the center of the box.
	inline void setCenter(const Vector3 &center) { m_center = center; }

	//! Get the half size of the box.
	inline const Vector3& getHalfSize() const { return m_halfSize; }
	//! Set the half size of the box.
	inline void setHalfSize(const Vector3 &halfSize) { m_halfSize = halfSize; }

	//! Set the size.
	inline void setSize(const Vector3 &size) { m_halfSize = size * 0.5f; }
	//! Get the size.
	inline Vector3 getSize() const { return m_halfSize*2.f; }

	//! Get the radius (length of the half size).
	inline Float getRadius() const { return m_halfSize.length(); }

	//-----------------------------------------------------------------------------------
	// Miscs functions
	//-----------------------------------------------------------------------------------

	//! Build the equivalent set of planes (left, right, bottom, top, front, back).
	void toPlanes(Plane planes[6]) const;

	// Serialization
    Bool writeToFile(OutStream &os) const;
    Bool readFromFile(InStream &is);

protected:

	Matrix3 m_data;
	Vector3 m_center;
	Vector3 m_halfSize;
};

/**
 * @brief Oriented bounding box extended version with a surrounding sphere.
 */
class O3D_API OBBoxExt : public OBBox
{
public:

	//! Default constructor.
	OBBoxExt() {}

	//! Construct with the 3 direction, center and half size.
	//! @param The first column contain the primary largest axis, the second contain the second largest.
	OBBoxExt(
			const Matrix3 &data,
			const Vector3 &center,
			const Vector3 &halfSize) :
		OBBox(data, center, halfSize)
	{
		updateRadius();
	}

	//! Construct with the 3 direction, center and half size.
	//! @param r Primary largest axis.
	//! @param s Secondary largest axis.
	//! @param t Third largest axis.
	OBBoxExt(
			const Vector3 &r,
			const Vector3 &s,
			const Vector3 &t,
			const Vector3 &center,
			const Vector3 &halfSize) :
		OBBox(r, s, t, center, halfSize)
	{
		updateRadius();
	}

	//! Set the half size.
	inline void setHalfSize(const Vector3 &halfSize)
	{
		m_halfSize = halfSize;
		updateRadius();
	}

	//! Set the size.
	inline void setSize(const Vector3 &size)
	{
		m_halfSize = size * 0.5f;
		updateRadius();
	}

	//! Return the STORED radius of the bbox.
	inline Float getRadius() const { return m_radiusMax; }

	//! Return a simple Oriented Bounding Box (no radius inside).
	inline OBBox getOBBox() const { return OBBox(m_data, m_center, m_halfSize); }

	// Serialization
    Bool writeToFile(OutStream &os) const;
    Bool readFromFile(InStream &is);

protected:

	Float m_radiusMin;
	Float m_radiusMax;

	//! Update the radius of our bbox
	void updateRadius()
	{
		// The bounding sphere.
		m_radiusMax = OBBox::getRadius();

		// The including sphere.
		m_radiusMin = o3d::minMin(
                o3d::abs(m_halfSize[0]),
                o3d::abs(m_halfSize[1]),
                o3d::abs(m_halfSize[2]));
	}
};

} // namespace o3d

#endif // _O3D_OBBOX_H

/**
 * @file bcone.h
 * @brief Bounding Cone
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @author  Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2010-02-18
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BCONE_H
#define _O3D_BCONE_H

#include "o3d/core/memorydbg.h"
#include "o3d/geom/geometry.h"
#include "o3d/geom/bsphere.h"

namespace o3d {

class Vector3;
class Plane;
class BSphere;
class AABBox;

//---------------------------------------------------------------------------------------
//! @class BCone
//-------------------------------------------------------------------------------------
//! A definition of a bounding sphere.
//---------------------------------------------------------------------------------------
class O3D_API BCone
{
protected:

	enum UpToDateData
	{
		UP_TO_DATE = 0,
		OUT_SPHERE = 1,
		IN_SPHERE = 2,
		PRECOMPUTED_0_2 = 4,
		PRECOMPUTED_3_8 = 8,
		PRECOMPUTED_9_11 = 16,
		PRECOMPUTED_12_15 = 32,
		STATE = 64,
		UPDATE_ALL = 0x000000FF
	};

	enum PrecomputedData
	{
		DATA_COS_CUTOFF = 0,
		DATA_SIN_CUTOFF = 1,
		DATA_TAN_CUTOFF = 2,
		DATA_VEC_BASEH_X = 3,
		DATA_VEC_BASEH_Y = 4,
		DATA_VEC_BASEH_Z = 5,
		DATA_VEC_BASEV_X = 6,
		DATA_VEC_BASEV_Y = 7,
		DATA_VEC_BASEV_Z = 8,
		DATA_VEC_TOP_POINT_X = 9,
		DATA_VEC_TOP_POINT_Y = 10,
		DATA_VEC_TOP_POINT_Z = 11,
		DATA_VEC_BASE_POINT_X = 12,
		DATA_VEC_BASE_POINT_Y = 13,
		DATA_VEC_BASE_POINT_Z = 14,
		DATA_COUNT
	};

public:

	//-----------------------------------------------------------------------------------
	// Constructors
	//-----------------------------------------------------------------------------------

	//! Default constructor.
	BCone();

	//! Copy constructor
	BCone(const BCone & cone);

	//! Constructor
	//! @param origin the position of the origin of the cone
	//! @param direction the direction of the cone
	//! @param depth the length of the cone
	//! @param cutoff the cutoff angle in radian (in [0;pi/2[ if spheric is false, in [0;pi] otherwise)
	//! @param spheric defines the type of cone (spherical (spotlight) or plane (pyramid) base)
	BCone(	const Vector3 & origin,
				const Vector3 & direction,
				Float depth,
				Float cutoff,
				Bool spheric);

	//! Assignment operator
	BCone & operator = (const BCone &);

	//! Comparison operator
	Bool operator == (const BCone &) const;
	//! Comparison operator
	Bool operator != (const BCone &) const;

	//! Returns cone state
	//! Ex: bad cutoff value
	Bool isValid() const;

	//-----------------------------------------------------------------------------------
	// Global parameters
	//-----------------------------------------------------------------------------------

	//! Set the origin
	void setOrigin(const Vector3 & origin);
	//! Get the origin
	const Vector3 & getOrigin() const { return m_origin; }

	//! Set the direction
	//! Direction must be normalized.
	void setDirection(const Vector3 & direction);
	//! Get the direction.
	const Vector3 & getDirection() const { return m_direction; }

	//! Set the depth value, ie the radius
	//! Must be > 0
	void setDepth(Float depth);
	//! Get the depth value
    Float getDepth() const { return m_depth; }

	//! Set the depth value, ie the radius
	//! Must be in [0;pi/2] for flat cone, [0;pi] for spheric cone
	void setCutoff(Float cutoff);
	//! Get the depth value
	Float getCutoff() const { return m_cutoff; }

	//! Set the boundary type
	void setSpheric(Bool value);
	//! Get the boundary type
	Bool isSpheric() const { return m_spheric; }

	//-----------------------------------------------------------------------------------
	// Convenient functions internaly used for math computation
	//-----------------------------------------------------------------------------------

	Float getCosCutoff() const;
	Float getSinCutoff() const;
	Float getTanCutoff() const;

	const BSphere & getOutBSphere() const;
	const BSphere & getInBSphere() const;

	const Vector3 & getBaseBasisH() const;
	const Vector3 & getBaseBasisV() const;

	const Vector3 & getTopPoint() const;
	const Vector3 & getBasePoint() const;

	void getAxisAlignedSegment(Int32 i, Vector3 & dir, Vector2f & range) const;
	void getAxisAlignedBase(Int32 i, Vector3 & vec) const;

	Bool doesDirectionLieIn(const Vector3 &) const;
	Bool doesDirectionLieIn(Float x, Float y, Float z) const;

	Bool isPointSeen(const Vector3 &) const;

	//-----------------------------------------------------------------------------------
	// Clipping
	//-----------------------------------------------------------------------------------

	//! Check if the cone intersects a plane
	//! Equivalent to (this.clip(plane) = CLIP_INSIDE) (impossible) or (this.clip(plane) = CLIP_INTERSECT)
	//! Warning: this function is NOT equivalent to O3DPlane::intersect(const O3DBCone &)
	//! @return True if there is a point is common to the plane and to the cone, that is to say,
	//!                  if the sphere 'touches' the plane.
    //! @see Plane::intersect(const BCone &) const
	Bool intersect(const Plane & plane) const;

	//! Check if the cone intersects a sphere
	//! Equivalent to (this.clip(sphere) = CLIP_INSIDE) or (this.clip(sphere) = CLIP_INTERSECT)
	//! This operation is commutative: cone.clip(sphere) = sphere.clip(cone)
	//! @return True if there is a point is common to the sphere and to the cone, that is to say,
	//!                  if the sphere 'touches' the cone.
    //! @see BSphere::intersect(const BCone &) const
	Bool intersect(const BSphere & sphere) const;

	//! Check if the cone intersects a box
	//! Equivalent to (this.clip(box) = CLIP_INSIDE) or (this.clip(box) = CLIP_INTERSECT)
	//! This operation is commutative: cone.clip(box) = box.clip(cone)
	//! @return True if there is a point is common to the sphere and to the box, that is to say,
	//!                  if the box 'touches' the cone.
    //! @see AABBox::intersect(const BCone &) const
	Bool intersect(const AABBox & box) const;

	//! Clip a plane
	//! Warning: this operation is not commutative: cone.clip(plane) != plane.clip(cone)
	//! @return CLIP_INSIDE means the plane is inside the cone (IMPOSSIBLE case !!!)
	//!         CLIP_OUTSIDE means the plane (in argument) is outside the cone (not even in contact)
	//!         CLIP_INTERSECT all the others cases
    //! @see Plane::intersect(const BCone &) const
	Geometry::Clipping clip(const Plane & plane) const;

	//! Clip a sphere
	//! Warning: this operation is not commutative: cone.clip(sphere) != sphere.clip(cone)
	//! @return CLIP_INSIDE means the sphere (in argument) is inside the cone (or in contact)
	//!         CLIP_OUTSIDE means the sphere (in argument) is outside the cone (not even in contact)
	//!         CLIP_INTERSECT all the others cases
    //! @see BSphere::intersect(const BCone &) const
	Geometry::Clipping clip(const BSphere & sphere) const;

	//! Clip a box
	//! Warning: this operation is not commutative: cone.clip(box) != box.clip(cone)
	//! @return CLIP_INSIDE means the box (in argument) is inside the cone (or in contact)
	//!         CLIP_OUTSIDE means the box (in argument) is outside the cone (not even in contact)
	//!         CLIP_INTERSECT all the others cases
    //! @see AABBox::intersect(const BCone &) const
	Geometry::Clipping clip(const AABBox & box) const;

	//-------------------------------------------------------------------------------
	// Serialization
	//-------------------------------------------------------------------------------

	Bool writeToFile(OutStream &os) const;
	Bool readFromFile(InStream &is);

protected:

	Bool isUpToDate(UpToDateData what) const { return ((m_upToDate & what) == 0); }
	Bool update(UpToDateData what) const;

private:

	Vector3 m_origin;
	Vector3 m_direction;

	Float m_depth;
	Float m_cutoff;

	Bool m_spheric;

	mutable Bool m_isValid;
	mutable UInt32 m_upToDate;
	mutable Float m_precomputed[DATA_COUNT];

	mutable BSphere m_outSphere;					//!< Draw the 2 sides.
	mutable BSphere m_inSphere;
};

} // namespace o3d

#endif // _O3D_BCONE_H


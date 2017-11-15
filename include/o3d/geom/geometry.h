/**
 * @file geometry.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GEOMETRY_H
#define _O3D_GEOMETRY_H

#include "o3d/core/base.h"
#include "o3d/core/memorydbg.h"
#include "o3d/core/vector2.h"

#include <vector>

namespace o3d {

class Plane;
class Vector3;

/**
 * @brief Common geometry functions
 * @author Emmanuel Ruffio, Frederic Scherma
 * @date 2005-02-03
 */
class O3D_API Geometry
{
public:

	//! Plane position for comparison.
	enum Position
	{
		POS_FRONT,
		POS_BACK,
		POS_LIE,
		POS_CROSS
	};

	//! Frustum, bounding sphere, box, and plane clipping.
	enum Clipping
	{
		CLIP_OUTSIDE = 0,        //!< Totally outside.
		CLIP_INSIDE,             //!< Totally inside.
		CLIP_INTERSECT           //!< Partially inside, it intersect.
	};

	static Position classify(const Vector3& v, Plane* P);
	static Position classify(const Vector3& v, Plane* P, Float ofs);

	//-----------------------------------------------------------------------------------
	// Intersection
	//-----------------------------------------------------------------------------------

	//! @brief Returns the intersection point between a INFINITE PLANE and a SEGMENT
	//! @param origin an origin of the plane
	//! @param n the third unit vector of the plane basis (the normal oriented into the right direction)
	//! @param lineOrigin a point of the segment
	//! @param lineDir a direction of the segment
	//! @returns a boolean. The boolean states whether or not the intersection exists, in this case, the float
	//!			'result' contains the coordinate along the line direction.
	static Bool intersection(
			const Vector3 & origin,
			const Vector3 & n,
			const Vector3 & lineOrigin,
			const Vector3 & lineDir,
            Float * component = nullptr);

	//! @brief Returns the intersection point between a FINITE PLANE and a SEGMENT
	//! @param origin an origin of the plane
	//! @param u the first unit vector of the plane basis
	//! @param v the second unit vector of the plane basis
	//! @param n the third unit vector of the plane basis (= u ^ v (cross-product))
	//! @param uRange the range along the u-axis
	//! @param vRange the range along the v-axis
	//! @param lineOrigin a point of the segment
	//! @param lineDir a direction of the segment
	//! @param lineRange the range along the segment direction
	//! @returns a boolean. The boolean states whether or not the intersection exists, in this case, the float
	//!			'result' contains the coordinate along the line direction.
	static Bool intersection(
			const Vector3 & origin,
			const Vector3 & u,
			const Vector3 & v,
			const Vector3 & n,
			const Vector2f & uRange,
			const Vector2f & vRange,
			const Vector3 & lineOrigin,
			const Vector3 & lineDir,
			const Vector2f & lineRange,
            Float * component = nullptr);

	//! @brief Returns the intersection points between a SPHERE and a SEGMENT
	//! @param origin the origin of the sphere
	//! @param radius the radius of the sphere
	//! @param lineOrigin a point of the segment
	//! @param lineDir a direction of the segment
	//! @param lineRange the range along the segment direction
	//! @returns the number of intersection points. If > 0, the vector3
	//!			'result1' and 'result2' contain the intersection point.
	static Int32 intersection(
			const Vector3 & origin,
			const Float radius,
			const Vector3 & lineOrigin,
			const Vector3 & lineDir,
			const Vector2f & lineRange,
            Vector3 * result1 = nullptr,
            Vector3 * result2 = nullptr);

	//! @brief Returns the nearest point between a SEGMENT and a POINT
	static Vector3 nearest(
			const Vector3 & lineOrigin,
			const Vector3 & lineDir,
			const Vector2f & lineRange,
			const Vector3 & point,
            Float * component = nullptr);

	//! @brief Returns the nearest point between a FINITE PLANE and a POINT
	static Vector3 nearest(
			const Vector3 & origin,
			const Vector3 & u,
			const Vector3 & v,
			const Vector2f & uRange,
			const Vector2f & vRange,
			const Vector3 & point,
            Float * x = nullptr,
            Float * y = nullptr);

	//! @brief Returns the nearest point between a FINITE PLANE and a SEGMENT
	static Vector3 nearest(
			const Vector3 & origin,
			const Vector3 & u,
			const Vector3 & v,
			const Vector3 & n,
			const Vector2f & uRange,
			const Vector2f & vRange,
			const Vector3 & lineOrigin,
			const Vector3 & lineDir,
			const Vector2f & lineRange,
            Float * component = nullptr);
};
/*
//---------------------------------------------------------------------------------------
//! @class Triangle
//-------------------------------------------------------------------------------------
//! Contain 3 coordinates thats make a triangle surface
//---------------------------------------------------------------------------------------
class O3D_API Triangle
{
public:

	Vector3 A,B,C; //!< the three coordinates
};

//---------------------------------------------------------------------------------------
//! @class SingleFace
//-------------------------------------------------------------------------------------
//! Contain 3 coordinates thats make a triangle surface and a texture coord indices
//---------------------------------------------------------------------------------------
class O3D_API SingleFace
{
public:

	// constructors
	SingleFace() {}
	SingleFace(UInt32 *Struct) { init(Struct); }
	SingleFace(UInt32 *Points,UInt32 tex) { Init(Points,Tex); }
	SingleFace(UInt32 p1,UInt32 P2,UInt32 P3,UInt32 Tex) { Init(P1,P2,P3,Tex); }

	inline void init(UInt32* struct) { memcpy(V,Struct,4*sizeof(UInt32)); }
	inline void init(UInt32* points,UInt32 Tex) { memcpy(V,Points,3*sizeof(UInt32)); TexId = Tex; }
	inline void init(UInt32 P1,UInt32 P2,UInt32 P3,UInt32 Tex) { V[0]=P1; V[1]=P2; V[2]=P3; TexId=Tex; }

	inline UInt32& operator[](UInt32 i) { return V[i]; }
	inline UInt32& id() { return TexId; }

private:

	UInt32 V[3];
	UInt32 TexId;
};
*/
} // namespace o3d

#endif // _O3D_GEOMETRY_H


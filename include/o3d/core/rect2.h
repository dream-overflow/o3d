/**
 * @file rect2.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_RECT2_H
#define _O3D_RECT2_H

#include "vector2.h"

namespace o3d {

template <class T> class Rect2;

// Type definitions
typedef Rect2<Double>	Rect2d;
typedef Rect2<Float>	Rect2f;
typedef Rect2<Int32>	Rect2i;
typedef Rect2<UInt32>	Rect2ui;
typedef Rect2<Int16>	Rect2s;
typedef Rect2<UInt16>	Rect2us;
typedef Rect2<Int8>	    Rect2b;
typedef Rect2<UInt8>	Rect2ub;

/**
 * @brief Rect2 class.
 * Contains a rectangular polygon. Points must be CCW ordered to perform polygonal
 * colisions detections.
 */
template<class T>
class O3D_API_TEMPLATE Rect2
{
public:

	//! Default constructor.
	Rect2()
	{
	}

	//! Construct for 4 points.
	Rect2(const Vector2i &a, const Vector2i &b, const Vector2i &c, const Vector2i &d) :
		A(a),
		B(b),
		C(c),
		D(d)
	{
	}

	//! Copy constructor.
	Rect2(const Rect2<T> &dup) :
		A(dup.A),
		B(dup.B),
		C(dup.C),
		D(dup.D)
	{
	}

	//! Copy operator.
	Rect2<T>& operator=(const Rect2<T> &dup)
	{
		A = dup.A;
		B = dup.B;
		C = dup.C;
		D = dup.D;

		return *this;
	}

	const Vector2<T>& a() const { return A; }
	const Vector2<T>& b() const { return B; }
	const Vector2<T>& c() const { return C; }
	const Vector2<T>& d() const { return D; }

	Vector2<T>& a() { return A; }
	Vector2<T>& b() { return B; }
	Vector2<T>& c() { return C; }
	Vector2<T>& d() { return D; }

	//! Is two rect are the same.
	Bool operator==(const Rect2<T> &who) const
	{
		return (A == who.A && B == who.B && C == who.C && D == who.D);
	}

	//! Is two rect are differents.
	Bool operator!=(const Rect2<T> &who) const
	{
		return (A != who.A || B != who.B || C != who.C || D != who.D);
	}

	/**
	 * @brief intersect of two rect.
	 * @param rect
	 * @return true if intersect
	 */
	Bool intersect(const Rect2 &rect) const
	{
		// 16 intersections to test
		if (lineIntersect(A, B, rect.A, rect.B))
			return True;

		if (lineIntersect(B, C, rect.A, rect.B))
			return True;

		if (lineIntersect(C, D, rect.A, rect.B))
			return True;

		if (lineIntersect(D, A, rect.A, rect.B))
			return True;

		if (lineIntersect(A, B, rect.B, rect.C))
			return True;

		if (lineIntersect(B, C, rect.B, rect.C))
			return True;

		if (lineIntersect(C, D, rect.B, rect.C))
			return True;

		if (lineIntersect(D, A, rect.B, rect.C))
			return True;

		if (lineIntersect(A, B, rect.C, rect.D))
			return True;

		if (lineIntersect(B, C, rect.C, rect.D))
			return True;

		if (lineIntersect(C, D, rect.C, rect.D))
			return True;

		if (lineIntersect(D, A, rect.C, rect.D))
			return True;

		if (lineIntersect(A, B, rect.D, rect.A))
			return True;

		if (lineIntersect(B, C, rect.D, rect.A))
			return True;

		if (lineIntersect(C, D, rect.D, rect.A))
			return True;

		if (lineIntersect(D, A, rect.D, rect.A))
			return True;

		return False;
	}

	/**
	 * @brief intersect2 is a simpler and faster version.
	 * It tests only some lines in the case that the two rect have symmetrics points.
	 * @param rect
	 * @return true if intersect
	 */
	Bool intersect2(const Rect2 &rect) const
	{
		// 8 intersections to test
		if (lineIntersect(B, C, rect.A, rect.B))
			return True;

		if (lineIntersect(D, A, rect.A, rect.B))
			return True;

		if (lineIntersect(A, B, rect.B, rect.C))
			return True;

		if (lineIntersect(C, D, rect.B, rect.C))
			return True;

		if (lineIntersect(B, C, rect.C, rect.D))
			return True;

		if (lineIntersect(D, A, rect.C, rect.D))
			return True;

		if (lineIntersect(A, B, rect.D, rect.A))
			return True;

		if (lineIntersect(C, D, rect.D, rect.A))
			return True;

		return False;
	}

	/**
	 * @brief inside Check if a rect is inside the rect.
	 * @param rect
	 * @return true if the point is inside
	 */
	Bool inside(const Rect2<T> &rect) const
	{
		// test with two triangles (to with triangle/triangle intersection)
		return pointInTriangle(rect.A, A, B, C) || pointInTriangle(rect.A, A, C, D) ||
				pointInTriangle(rect.B, A, B, C) || pointInTriangle(rect.B, A, C, D) ||
				pointInTriangle(rect.C, A, B, C) || pointInTriangle(rect.C, A, C, D) ||
				pointInTriangle(rect.D, A, B, C) || pointInTriangle(rect.D, A, C, D);
	}

	/**
	 * @brief inside Check if a point is inside the rect.
	 * @param point
	 * @return true if the point is inside
	 */
	Bool inside(const Vector2<T> &p) const
	{
		// test with two triangles
		return pointInTriangle(p, A, B, C) || pointInTriangle(p, A, C, D);
	}

	Bool pointInTriangle(
			const Vector2<T> &p,
			const Vector2<T> &_a,
			const Vector2<T> &_b,
			const Vector2<T> &_c) const
	{
		Vector2f v0 = _c - _a;
		Vector2f v1 = _b - _a;
		Vector2f v2 = p - _a;

		Float dot00 = v0 * v0;
		Float dot01 = v0 * v1;
		Float dot02 = v0 * v2;
		Float dot11 = v1 * v1;
		Float dot12 = v1 * v2;

		Float invDenom = 1.f / (dot00 * dot11 - dot01 * dot01);
		Float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		Float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// Check if point is in triangle
		return (u >= 0) && (v >= 0) && (u + v < 1);
	}

	//! convert to a string
	inline operator String() const
	{
		String temp;
		temp << String("{ ") << A << String("| ") << B << String(" | ") <<
								C << String("| ") << D << String(" }");
		return temp;
	}

	// Serialization
	inline Bool readFromFile(InStream &is)
	{
        is   >> A
			 >> B
			 >> C
			 >> D;

		return True;
	}

	inline Bool writeToFile(OutStream &os) const
	{
        os   << A
			 << B
			 << C
			 << D;

		return True;
	}

private:

	Vector2<T> A, B, C, D;

	Bool lineIntersect(
			const Vector2<T> &a1, const Vector2<T> &a2,
			const Vector2<T> &b1, const Vector2<T> &b2) const
	{
		Float adx = a2.x() - a1.x();
		Float ady = a2.y() - a1.y();
		Float bdx = b2.x() - b1.x();
		Float bdy = b2.y() - b1.y();

		// paralleles
		if ((bdx * ady - bdy * adx) == 0)
			return False;

		Float s = (adx * (b1.y() - a1.y()) + ady * (a1.x() - b1.x())) / (bdx * ady - bdy * adx);
		Float t = (bdx * (a1.y() - b1.y()) + bdy * (b1.x() - a1.x())) / (bdy * adx - bdx * ady);

		//if (s >= 0.f && s <= 1.f && t >= 0.f && t <= 1.f)
		//	printf("intersect at %f, %f\n", a1.x() + t * adx, a1.y() + t * ady);

		return (s >= 0.f && s <= 1.f && t >= 0.f && t <= 1.f);
	}
};

template <>
Bool Rect2<Double>::lineIntersect(
			const Vector2<Double> &a1, const Vector2<Double> &a2,
			const Vector2<Double> &b1, const Vector2<Double> &b2) const;

} // namespace o3d

#endif // _O3D_RECT2_H


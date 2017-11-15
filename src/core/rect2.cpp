/**
 * @file rect2.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/rect2.h"

using namespace o3d;

template <>
Bool Rect2<Double>::lineIntersect(
			const Vector2<Double> &a1, const Vector2<Double> &a2,
			const Vector2<Double> &b1, const Vector2<Double> &b2) const
{
	Double adx = a2.x() - a1.x();
	Double ady = a2.y() - a1.y();
	Double bdx = b2.x() - b1.x();
	Double bdy = b2.y() - b1.y();

	// paralleles
	if ((bdx * ady - bdy * adx) == 0)
		return False;

	Float s = (adx * (b1.y() - a1.y()) + ady * (a1.x() - b1.x())) / (bdx * ady - bdy * adx);
	Float t = (bdx * (a1.y() - b1.y()) + bdy * (b1.x() - a1.x())) / (bdy * adx - bdx * ady);

	//if (s >= 0.f && s <= 1.f && t >= 0.f && t <= 1.f)
	//	printf("intersect at %f, %f\n", a1.x() + t * adx, a1.y() + t * ady);

	return (s >= 0.f && s <= 1.f && t >= 0.f && t <= 1.f);
}


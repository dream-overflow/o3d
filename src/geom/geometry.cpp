/**
 * @file geometry.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/geom/geometry.h"

#include "o3d/geom/plane.h"
#include "o3d/core/vector3.h"
#include "o3d/core/matrix3.h"
#include "o3d/core/matrix4.h"
#include "o3d/core/processor.h"

using namespace o3d;

//-----------------------------------------------------------------------------------
// Intersection
//-----------------------------------------------------------------------------------

Bool Geometry::intersection(
		const Vector3 & origin,
		const Vector3 & n,
		const Vector3 & lineOrigin,
		const Vector3 & lineDir,
		Float * component)
{
	Vector3 lPlaneToLine(lineOrigin - origin);

	Float lAlpha = -(lPlaneToLine * n);
	const Float lV = lineDir * n;

	if (o3d::abs<Float>(lV) < o3d::Limits<Float>::epsilon())
	{
		if (lPlaneToLine.normInf() < o3d::Limits<Float>::epsilon())
		{
            if (component != nullptr)
				*component = 0.0f;
			
			return True;
		}
		else
			return False;
	}
	else
	{
        if (component != nullptr)
			*component = lAlpha / lV;

		return True;
	}
}

//! Returns the intersection point between a finite plane and a segment
Bool Geometry::intersection(
		const Vector3 & origin,
		const Vector3 & u,
		const Vector3 & v,
		const Vector3 & n,
		const Vector2f & uRange,
		const Vector2f & vRange,
		const Vector3 & lineOrigin,
		const Vector3 & lineDir,
		const Vector2f & lineRange,
		Float * component)
{
	// Composante selon 'lineDir' du point d'intersection entre la ligne et le plan
	Vector3 lPlaneToLine(lineOrigin - origin);

	Float lAlpha = -(lPlaneToLine * n);

	const Float lV = lineDir * n;

	if (o3d::abs<Float>(lV) < o3d::Limits<Float>::epsilon()) // Plan parallele a la ligne
	{
		if (o3d::abs<Float>(lPlaneToLine * n) < o3d::Limits<Float>::epsilon()) // Line et plan confondu
		{
			Vector3 lPlanePoint, lPoint;

			// Cas le plus couteux mais excessivement rare. Pour chaque sommet du plan, on calcule le point
			// le plus pres du segment et on test si ce point est dans le plan
			for (Int32 i = 0 ; i < 4 ; ++i)
			{
				switch(i)
				{
				case 0: lPlanePoint = origin + uRange[1] * u + vRange[1] *v; break;
				case 1: lPlanePoint = origin + uRange[0] * u + vRange[1] *v; break;
				case 2: lPlanePoint = origin + uRange[0] * u + vRange[0] *v; break;
				case 3: lPlanePoint = origin + uRange[1] * u + vRange[0] *v; break;
				}

				// lPoint est le point du segment le plus proche du sommet 'PlanePoint'
				lPoint = Geometry::nearest(lineOrigin, lineDir, lineRange, lPlanePoint, component);
				
				if (o3d::liesIn<Float>(lPoint * u, uRange[0], uRange[1]) && o3d::liesIn<Float>(lPoint * v, vRange[0], vRange[1]))
					return True;
			}

			return False;
		}
		else
			return False;
	}
	else
	{
		lAlpha = lAlpha / lV;

		if (o3d::liesIn<Float>(lAlpha, lineRange[0], lineRange[1]))
		{
			const Vector3 lIntersection = lineOrigin + lAlpha * lineDir;
			
			if (o3d::liesIn<Float>(lIntersection * u, uRange[0], uRange[1]) && o3d::liesIn<Float>(lIntersection * v, vRange[0], vRange[1]))
			{
                if (component != nullptr)
					*component = lAlpha;

				return True;
			}
			else
				return False;
		}
		else
			return False;
	}
}

Int32 Geometry::intersection(
		const Vector3 & origin,
		const Float radius,
		const Vector3 & lineOrigin,
		const Vector3 & lineDir,
		const Vector2f & lineRange,
		Vector3 * result1,
		Vector3 * result2)
{
	const Vector3 lLineToSphere(origin - lineOrigin);
	const Vector3 lPoint(Geometry::nearest(lineOrigin, lineDir, lineRange, origin));
	const Vector3 lPointToSphere(origin - lPoint);

	const Float lSqrP2S = lPointToSphere.squareLength();
	const Float lSqrRadius = o3d::sqr<Float>(radius);

	if (lSqrP2S > lSqrRadius)
		return 0;

	//const Float a = 1.0f;
	const Float b = -2.0f * (lLineToSphere * lineDir);
	const Float c = lSqrP2S - lSqrRadius;
	Float delta = b*b - 4.0f * c;

	if (delta < 0.0f) // Roundoff errors
		return 0;
	else
		delta = sqrt(delta);

	// Polynome : 2 solutions
    if (result1 != nullptr)
		*result1 = lineOrigin + (0.5f * (-b - delta)) * lineDir;

    if (result2 != nullptr)
		*result2 = lineOrigin + (0.5f * (-b + delta)) * lineDir;

	return (delta < o3d::Limits<Float>::epsilon() ? 1 : 2);
}

// Returns the nearest point between a segment and a point
Vector3 Geometry::nearest(
		const Vector3 & lineOrigin,
		const Vector3 & lineDir,
		const Vector2f & lineRange,
		const Vector3 & point,
		Float * component)
{
    if (component != nullptr)
	{
		*component = o3d::clamp<Float>(((point - lineOrigin) * lineDir), lineRange[0], lineRange[1]);
		return lineOrigin + (*component) * lineDir;
	}
	else
		return lineOrigin + o3d::clamp<Float>(((point - lineOrigin) * lineDir), lineRange[0], lineRange[1]) * lineDir;
}

// Returns the nearest point between a finite plane and a point
Vector3 Geometry::nearest(
		const Vector3 & origin,
		const Vector3 & u,
		const Vector3 & v,
		const Vector2f & uRange,
		const Vector2f & vRange,
		const Vector3 & point,
		Float * x,
		Float * y)
{
	const Vector3 lOriginToPoint(point - origin);
	Vector2f lProjPoint(lOriginToPoint*u, lOriginToPoint*v);

	lProjPoint.x() = o3d::clamp<Float>(lProjPoint.x(), uRange[0], uRange[1]);
	lProjPoint.y() = o3d::clamp<Float>(lProjPoint.y(), vRange[0], vRange[1]);

    if (x != nullptr)
		*x = lProjPoint.x();

    if (y != nullptr)
		*y = lProjPoint.y();

	return origin + lProjPoint.x() * u + lProjPoint.y() * v;
}

// Returns the nearest point between a finite plane and a segment
Vector3 Geometry::nearest(
		const Vector3 & origin,
		const Vector3 & u,
		const Vector3 & v,
		const Vector3 & n,
		const Vector2f & uRange,
		const Vector2f & vRange,
		const Vector3 & lineOrigin,
		const Vector3 & lineDir,
		const Vector2f & lineRange,
		Float * component)
{
	Vector3 lPoint;
	Float lComp;

	if (intersection(origin, n, lineOrigin, lineDir, &lComp))
	{
		return nearest(lineOrigin, lineDir, lineRange, origin + lComp*lineDir, component);
	}
	else
	{
        if (component != nullptr)
			*component = lComp;

		return lineOrigin;
	}
}

Geometry::Position Geometry::classify(const Vector3 &v, Plane* P)
{
	Float m_distance = (v*P->m_normal)-P->m_d;
	if (m_distance > Limits<Float>::epsilon())
		return POS_FRONT;
	if (m_distance < -Limits<Float>::epsilon())
		return POS_BACK;
	return POS_LIE;
}

Geometry::Position Geometry::classify(const Vector3 &v, Plane* P, Float ofs)
{
	Float m_distance = (v*P->m_normal)-P->m_d-ofs;
	if (m_distance > Limits<Float>::epsilon())
		return POS_FRONT;
	if (m_distance < -Limits<Float>::epsilon())
		return POS_BACK;
	return POS_LIE;
}


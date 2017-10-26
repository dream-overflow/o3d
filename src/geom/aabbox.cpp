/**
 * @file aabbox.cpp
 * @brief Axis Aligned Bounding Box.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-08-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/geom/aabbox.h"

#include "o3d/geom/aabbox.h"
#include "o3d/geom/plane.h"
#include "o3d/geom/bcone.h"
#include "o3d/geom/bsphere.h"
#include "o3d/core/matrix4.h"
#include "o3d/core/vector2.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

//-----------------------------------------------------------------------------------
// class AABBox
//---------------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------------

static inline Float AABBox_Max(
		Float _v1,
		Float _v2,
		Float _v3,
		Float _v4,
		Float _v5,
		Float _v6)
{
	return o3d::maxMax(_v1, _v2, o3d::maxMax(_v3, _v4, o3d::max(_v5, _v6)));
}

static inline Float AABBox_Min(
		Float _v1,
		Float _v2,
		Float _v3,
		Float _v4,
		Float _v5,
		Float _v6)
{
	return o3d::minMin(_v1, _v2, o3d::minMin(_v3, _v4, o3d::min(_v5, _v6)));
}

AABBox::AABBox(const BCone &cone):
	m_center(),
	m_halfSize()
{
	const Vector3 lBaseH = cone.getBaseBasisH();
	const Vector3 lBaseV = cone.getBaseBasisV();

	if (!cone.isValid())
		return;

	if (cone.isSpheric())
	{
		const Float f = cone.getDepth()*cone.getSinCutoff(); // Distance entre les bords du cone et l'axe central
		const Float e = cone.getDepth()*cone.getCosCutoff(); // Distance entre l'origine du cone et la projection du bord sur l'axe

		const Vector3 lBaseCenter(cone.getOrigin() + e*cone.getDirection());		// Centre de la base
		const Vector3 lBaseOrigin(cone.getOrigin() + cone.getDepth()*cone.getDirection());	// pointe du cone

		Vector3 lOrigin(cone.getOrigin());
		Vector3 lBase[4]; // top/bottom/left/right
		lBase[0] = lBaseCenter; lBase[0] += f * lBaseV;
		lBase[1] = lBaseCenter; lBase[1] -= f * lBaseV;
		lBase[2] = lBaseCenter; lBase[2] += f * lBaseH;
		lBase[3] = lBaseCenter; lBase[3] -= f * lBaseH;

		Float lMaxX, lMaxY, lMaxZ, lMinX, lMinY, lMinZ;
		
		if (cone.doesDirectionLieIn(1.0f, 0.0f, 0.0f))
			lMaxX = cone.getOrigin().x() + cone.getDepth();
		else
			lMaxX = 0.5f * AABBox_Max(cone.getOrigin().x(), lBaseOrigin.x(), lBase[0].x(), lBase[1].x(), lBase[2].x(), lBase[3].x());

		if (cone.doesDirectionLieIn(0.0f, 1.0f, 0.0f))
			lMaxY = cone.getOrigin().y() + cone.getDepth();
		else
			lMaxY = 0.5f * AABBox_Max(cone.getOrigin().y(), lBaseOrigin.y(), lBase[0].y(), lBase[1].y(), lBase[2].y(), lBase[3].y());

		if (cone.doesDirectionLieIn(0.0f, 0.0f, 1.0f))
			lMaxZ = cone.getOrigin().z() + cone.getDepth();
		else
			lMaxZ = 0.5f * AABBox_Max(cone.getOrigin().z(), lBaseOrigin.z(), lBase[0].z(), lBase[1].z(), lBase[2].z(), lBase[3].z());

		if (cone.doesDirectionLieIn(-1.0f, 0.0f, 0.0f))
			lMinX = cone.getOrigin().x() - cone.getDepth();
		else
			lMinX = 0.5f * AABBox_Min(cone.getOrigin().x(), lBaseOrigin.x(), lBase[0].x(), lBase[1].x(), lBase[2].x(), lBase[3].x());

		if (cone.doesDirectionLieIn(0.0f, -1.0f, 0.0f))
			lMinY = cone.getOrigin().y() - cone.getDepth();
		else
			lMinY = 0.5f * AABBox_Min(cone.getOrigin().y(), lBaseOrigin.y(), lBase[0].y(), lBase[1].y(), lBase[2].y(), lBase[3].y());

		if (cone.doesDirectionLieIn(0.0f, 0.0f, -1.0f))
			lMinZ = cone.getOrigin().z() - cone.getDepth();
		else
			lMinZ = 0.5f * AABBox_Min(cone.getOrigin().z(), lBaseOrigin.z(), lBase[0].z(), lBase[1].z(), lBase[2].z(), lBase[3].z());

		m_center.set(lMaxX + lMinX, lMaxY + lMinY, lMaxZ + lMinZ);
		m_halfSize.set(lMaxX - lMinX, lMaxY - lMinY, lMaxZ - lMinZ);
	}
	else
	{
		const Vector3 lBaseCenter(cone.getOrigin() + cone.getDepth() * cone.getDirection());	// Centre de la base
		const Float lHalfBase = cone.getDepth() * cone.getTanCutoff();		// Rayon de la base

		Vector3 lOrigin(cone.getOrigin());
		Vector3 lBase[4]; // top/bottom/left/right
		lBase[0] = lBaseCenter; lBase[0] += lHalfBase * lBaseV;
		lBase[1] = lBaseCenter; lBase[1] -= lHalfBase * lBaseV;
		lBase[2] = lBaseCenter; lBase[2] += lHalfBase * lBaseH;
		lBase[3] = lBaseCenter; lBase[3] -= lHalfBase * lBaseH;

		Float lMaxX = 0.5f * AABBox_Max(cone.getOrigin().x(), lBase[0].x(), lBase[1].x(), lBase[2].x(), lBase[3].x(), lBase[3].x());
		Float lMaxY = 0.5f * AABBox_Max(cone.getOrigin().y(), lBase[0].y(), lBase[1].y(), lBase[2].y(), lBase[3].y(), lBase[3].y());
		Float lMaxZ = 0.5f * AABBox_Max(cone.getOrigin().z(), lBase[0].z(), lBase[1].z(), lBase[2].z(), lBase[3].z(), lBase[3].z());

		Float lMinX = 0.5f * AABBox_Min(cone.getOrigin().x(), lBase[0].x(), lBase[1].x(), lBase[2].x(), lBase[3].x(), lBase[3].x());
		Float lMinY = 0.5f * AABBox_Min(cone.getOrigin().y(), lBase[0].y(), lBase[1].y(), lBase[2].y(), lBase[3].y(), lBase[3].y());
		Float lMinZ = 0.5f * AABBox_Min(cone.getOrigin().z(), lBase[0].z(), lBase[1].z(), lBase[2].z(), lBase[3].z(), lBase[3].z());

		m_center.set(lMaxX + lMinX, lMaxY + lMinY, lMaxZ + lMinZ);
		m_halfSize.set(lMaxX - lMinX, lMaxY - lMinY, lMaxZ - lMinZ);
	}
}

//---------------------------------------------------------------------------------------
// Generals Parameters define
//---------------------------------------------------------------------------------------

void AABBox::extend(const Vector3 &refV)
{
	Vector3 vMin = getMin();
	Vector3 vMax = getMax();

	vMin.minOf(vMin, refV);
	vMax.maxOf(vMax, refV);
	setMinMax(vMin, vMax);
}

//---------------------------------------------------------------------------------------
// Clipping functions
//---------------------------------------------------------------------------------------

Bool AABBox::clipFront(const Plane &p) const
{
	Vector3 vSwap;

	// The bbox is front of the plane if only one of his vertex is in front.
	if (p*(m_center + m_halfSize) > 0)	return True;
	if (p*(m_center - m_halfSize) > 0)	return True;
	vSwap.set(-m_halfSize[0], m_halfSize[1], m_halfSize[2]);
	if (p*(m_center + vSwap) > 0)		return True;
	if (p*(m_center - vSwap) > 0)		return True;
	vSwap.set(m_halfSize[0], -m_halfSize[1], m_halfSize[2]);
	if (p*(m_center + vSwap) > 0)		return True;
	if (p*(m_center - vSwap) > 0)		return True;
	vSwap.set(m_halfSize[0], m_halfSize[1], -m_halfSize[2]);
	if (p*(m_center + vSwap) > 0)		return True;
	if (p*(m_center - vSwap) > 0)		return True;

	return False;
}

Bool AABBox::clipBack(const Plane &p) const
{
	Vector3 vSwap;

	// The bbox is back of the plane if only one of his vertex is in back.
	if (p*(m_center + m_halfSize) < 0)	return True;
	if (p*(m_center - m_halfSize) < 0)	return True;
	vSwap.set(-m_halfSize[0], m_halfSize[1], m_halfSize[2]);
	if (p*(m_center + vSwap) < 0)		return True;
	if (p*(m_center - vSwap) < 0)		return True;
	vSwap.set(m_halfSize[0], -m_halfSize[1], m_halfSize[2]);
	if (p*(m_center + vSwap) < 0)		return True;
	if (p*(m_center - vSwap) < 0)		return True;
	vSwap.set(m_halfSize[0], m_halfSize[1], -m_halfSize[2]);
	if (p*(m_center + vSwap) < 0)		return True;
	if (p*(m_center - vSwap) < 0)		return True;

	return False;
}

Bool AABBox::clipSegment(Vector3 &a, Vector3 &b) const
{
	// Trivial test. If both are in, they are unchanged
	if (include(a) && include(b))
		return True;
	// Else, must clip the segment against the pyramid.
	Plane planes[6];
	toPlanes(planes);
	Vector3 p0 = a;
	Vector3 p1 = b;

	// clip the segment against all planes
	for (UInt32 i=0; i<6; i++)
	{
		if (!planes[i].clipSegmentBack(p0, p1))
			return False;
	}
	// get result
	a = p0;
	b = p1;
	return True;
}

Bool AABBox::include(const Vector3 &a) const
{
	if (m_center[0]+m_halfSize[0]<a[0])	return False;
	if (m_center[0]-m_halfSize[0]>a[0])	return False;
	if (m_center[1]+m_halfSize[1]<a[1])	return False;
	if (m_center[1]-m_halfSize[1]>a[1])	return False;
	if (m_center[2]+m_halfSize[2]<a[2])	return False;
	if (m_center[2]-m_halfSize[2]>a[2])	return False;
	return True;
}

Bool AABBox::include(const Box2f &b) const
{
	if (m_center[0]+m_halfSize[0] < b.x2())	return False;
	if (m_center[0]-m_halfSize[0] > b.x())	return False;
	if (m_center[1]+m_halfSize[1] < b.y2())	return False;
	if (m_center[1]-m_halfSize[1] > b.y())	return False;
	return True;
}

Bool AABBox::include(const AABBox &box) const
{
	if (m_center[0]+m_halfSize[0] < box.m_center[0]+box.m_halfSize[0])	return False;
	if (m_center[0]-m_halfSize[0] > box.m_center[0]-box.m_halfSize[0])	return False;
	if (m_center[1]+m_halfSize[1] < box.m_center[1]+box.m_halfSize[1])	return False;
	if (m_center[1]-m_halfSize[1] > box.m_center[1]-box.m_halfSize[1])	return False;
	if (m_center[2]+m_halfSize[2] < box.m_center[2]+box.m_halfSize[2])	return False;
	if (m_center[2]-m_halfSize[2] > box.m_center[2]-box.m_halfSize[2])	return False;
	return True;
}

Bool AABBox::include(const BSphere &s) const
{
	const Vector3 lC2C = s.getCenter() - m_center;  // Center to Center

	if (lC2C.x() + s.getRadius() > m_halfSize[0]) return False;
	if (lC2C.x() - s.getRadius() < -m_halfSize[0]) return False;
	if (lC2C.y() + s.getRadius() > m_halfSize[1]) return False;
	if (lC2C.y() - s.getRadius() < -m_halfSize[1]) return False;
	if (lC2C.z() + s.getRadius() > m_halfSize[2]) return False;
	if (lC2C.z() - s.getRadius() < -m_halfSize[2]) return False;
	return True;
}

Bool AABBox::intersect(const AABBox &box) const
{
	Vector3 vMinA = getMin();
	Vector3 vMaxA = getMax();

	Vector3 vMinB = box.getMin();
	Vector3 vMaxB = box.getMax();

	return ! ( vMinA[0] > vMaxB[0] ||
			   vMinA[1] > vMaxB[1] ||
			   vMinA[2] > vMaxB[2] ||
			   vMinB[0] > vMaxA[0] ||
			   vMinB[1] > vMaxA[1] ||
			   vMinB[2] > vMaxA[2]);
}

Bool AABBox::intersect(const Vector3 &a, const Vector3 &b, const Vector3 &c) const
{
	// Simple test.
	if (include(a) || include(b) || include(c))
		return True;

	// Else, must test if the polygon intersect the pyramid.
	Plane planes[6];
	toPlanes(planes);

	O3D_ASSERT(0);

	// TODO
	/*Polygon poly(a,b,c);
	poly.clip(planes,6);
	if (poly.getNumVertices()==0)
		return False;*/

	return True;
}

Bool AABBox::intersect(const Vector3 &a, const Vector3 &b) const
{
	// Trivial test.
	if (include(a) || include(b))
		return True;

	// Else, must test if the segment intersect the pyramid.
	Plane planes[6];
	toPlanes(planes);
	Vector3 p0 = a;
	Vector3 p1 = b;

	// clip the segment against all planes
	for(UInt32 i=0;i<6;i++)
	{
		if (!planes[i].clipSegmentBack(p0, p1))
			return False;
	}

	return True;
}

Bool AABBox::intersect(const Box2f &b) const
{
	Vector3 vMinA = getMin();
	Vector3 vMaxA = getMax();

	return ! ( vMinA[0] > b.x2() ||
			   vMinA[1] > b.y2() ||
			   b.x() > vMaxA[0] ||
			   b.y() > vMaxA[1]);
}

Bool AABBox::intersect(const BSphere &s) const
{
	if (m_center[0] + m_halfSize[0] < s.m_center[0] - s.m_radius) return False;
	if (m_center[1] + m_halfSize[1] < s.m_center[1] - s.m_radius) return False;
	if (m_center[2] + m_halfSize[2] < s.m_center[2] - s.m_radius) return False;

	if (m_center[0] - m_halfSize[0] > s.m_center[0] + s.m_radius) return False;
	if (m_center[1] - m_halfSize[1] > s.m_center[1] + s.m_radius) return False;
	if (m_center[2] - m_halfSize[2] > s.m_center[2] + s.m_radius) return False;

	const Vector2f lXRange(-m_halfSize.x(), m_halfSize.x()), lYRange(-m_halfSize.y(), m_halfSize.y()), lZRange(-m_halfSize.z(), m_halfSize.z());
	const Float lSqrRadius = o3d::sqr<Float>(s.getRadius());
	const Vector3 lX(1.0f, 0.0f, 0.0f);
	const Vector3 lY(0.0f, 1.0f, 0.0f);
	const Vector3 lZ(0.0f, 0.0f, 1.0f);

	Vector3 lCenter;

	if (m_center.x() <= s.getCenter().x())
	{
		//	Face +X
		lCenter.set(m_center.x() + m_halfSize.x(), m_center.y(), m_center.z());
		if ((s.getCenter() - Geometry::nearest(lCenter, lY, lZ, lYRange, lZRange, s.getCenter())).squareLength() <= lSqrRadius)
			return True;
	}
	else
	{
		//	Face -X
		lCenter.set(m_center.x() - m_halfSize.x(), m_center.y(), m_center.z());
		if ((s.getCenter() - Geometry::nearest(lCenter, lY, lZ, lYRange, lZRange, s.getCenter())).squareLength() <= lSqrRadius)
			return True;
	}

	if (m_center.y() <= s.getCenter().y())
	{
		// Face +Y
		lCenter.set(m_center.x(), m_center.y() + m_halfSize.y(), m_center.z());
		if ((s.getCenter() - Geometry::nearest(lCenter, lX, lZ, lXRange, lZRange, s.getCenter())).squareLength() <= lSqrRadius)
			return True;
	}
	else
	{
		// Face -Y
		lCenter.set(m_center.x(), m_center.y() - m_halfSize.y(), m_center.z());
		if ((s.getCenter() - Geometry::nearest(lCenter, lX, lZ, lXRange, lZRange, s.getCenter())).squareLength() <= lSqrRadius)
			return True;
	}

	if (m_center.z() <= s.getCenter().z())
	{
		// Face +Z
		lCenter.set(m_center.x(), m_center.y(), m_center.z() + m_halfSize.z());
		if ((s.getCenter() - Geometry::nearest(lCenter, lX, lY, lXRange, lYRange, s.getCenter())).squareLength() <= lSqrRadius)
			return True;
	}
	else
	{
		// Face -Z
		lCenter.set(m_center.x(), m_center.y(), m_center.z() - m_halfSize.z());
		if ((s.getCenter() - Geometry::nearest(lCenter, lX, lY, lXRange, lYRange, s.getCenter())).squareLength() <= lSqrRadius)
			return True;
	}

	return False;
}

Geometry::Clipping AABBox::clip(const AABBox & box) const
{
	if (include(box))
		return Geometry::CLIP_INSIDE;
	else if (intersect(box))
		return Geometry::CLIP_INTERSECT;
	else
		return Geometry::CLIP_OUTSIDE;
}

Geometry::Clipping AABBox::clip(const BSphere & s) const
{
	if (include(s))
		return Geometry::CLIP_INSIDE;
	else if (intersect(s))
		return Geometry::CLIP_INTERSECT;
	else
		return Geometry::CLIP_OUTSIDE;
}


Geometry::Clipping AABBox::clip(const BCone &cone) const
{
	if (!cone.isValid())
	{
		O3D_WARNING("Invalid BCone object");
		return Geometry::CLIP_OUTSIDE;
	}

	Geometry::Clipping lClipResult;
	
	// Test "rapide" avec une bounding sphere. La plupart des cas devrait etre regle ici.
	if ((lClipResult = clip(cone.getOutBSphere())) != Geometry::CLIP_INTERSECT)
		return lClipResult;

	// Effectue le test uniquement si la box est petite comparee au cone
	if ((m_halfSize.normInf() < 0.2f * cone.getInBSphere().getRadius()) && (clip(cone.getInBSphere()) != Geometry::CLIP_OUTSIDE))
		return Geometry::CLIP_INTERSECT;

	const Vector3 & lCDirection = cone.getDirection();
	const Vector3 & lCOrigin = cone.getOrigin();
	const Float lCCutoff = cone.getCutoff();
	const Bool lCSpheric = cone.isSpheric();
	const Float lCDepth = cone.getDepth();

	const Float lCosCutoff = cone.getCosCutoff();
	const Float lSinCutoff = cone.getSinCutoff();
	const Float lTanCutoff = cone.getTanCutoff();

	const Vector3 lC2C(lCOrigin - m_center); // Center to center

	// on prend chaque sommet, on change de base leurs coordonnees (on passe dans la base du cone)
	const Vector3 lBaseH = cone.getBaseBasisH();
	const Vector3 lBaseV = cone.getBaseBasisV();

	Vector3 lCorners[8];
	getCorners(lCorners);

	const Float lRange[2] = { lCSpheric ? lCDepth*o3d::min<Float>(lCosCutoff, 0.0f) : 0.0f, lCDepth };
	const Float lTresholdSpheric = lCDepth*lCosCutoff;

	// Ce coefficient permet de calculer la largeur du cone a une abscisse donnee le long de son axe:
	// rayon(x) = lBaseCoef * x; avec x >= 0 pour un cone a base plate
	// Le coefficient est negatif pour un cone a base spherique avec un angle d'ouverture > 90deg
	const Float lBaseCoef = (lCSpheric ? lCDepth*lSinCutoff : lCDepth * lTanCutoff)
						/ (lCSpheric ? lCDepth*lCosCutoff : lCDepth);

	// Carre du rayon max du cone
	const Float lSqrConeWidth = o3d::sqr(lCSpheric ? lCDepth * sin(o3d::min(lCCutoff, o3d::HALF_PI)) : lCDepth * lTanCutoff);

	// Carre de la longueur du cone
	const Float lSqrDepth = o3d::sqr(lCDepth);

	Bool lOutsideFound = False;
	Bool lInsideFound = False;

	Float lSqrDistFromAxe;
	Vector2f lProjOnBase;
	Vector3 lDif;

	for (Int32 i = 0 ; i < 8 ; ++i)
	{
		lDif = lCorners[i]; lDif -= lCOrigin;

		Float lProjOnAxe = lDif * lCDirection;
		Float lProjBase1 = lDif * lBaseH;
		Float lProjBase2 = lDif * lBaseV;

		lSqrDistFromAxe = o3d::sqr<Float>(lProjBase1) + o3d::sqr<Float>(lProjBase2);

		if ((lProjOnAxe < lRange[0]) || (lProjOnAxe > lRange[1]))
			lOutsideFound = True;
		else if (lSqrDistFromAxe > lSqrConeWidth)
			lOutsideFound = True;
		else
		{
			if (lCSpheric) // Un peu plus complexe pour le cone spherique
			{
				if (lDif.squareLength() > lSqrDepth)
					lOutsideFound = True;
				else
				{
					if (lCCutoff > o3d::HALF_PI) // Cas un peu tordu mais possible
					{
						if (lProjOnAxe <= 0.0f)
						{
							if (o3d::sqr<Float>(lBaseCoef * lProjOnAxe) > lSqrDistFromAxe)
								lOutsideFound = True;
							else
								lInsideFound = True;
						}
						else
							lInsideFound = True;
					}
					else
					{
						// Signifie que l'on est dans la phase lineaire
						if (lProjOnAxe <= lTresholdSpheric)
						{
							if (o3d::sqr<Float>(lBaseCoef * lProjOnAxe) >= lSqrDistFromAxe)
								lInsideFound = True;
							else
								lOutsideFound = True;
						}
						else
							lInsideFound = True;
					}
				}
			}
			else
			{
				if (o3d::sqr<Float>(lBaseCoef * lProjOnAxe) >= lSqrDistFromAxe)
					lInsideFound = True;
				else
					lOutsideFound = True;
			}
		}

		// Un point doit etre soit dedans soit dehors...
		O3D_ASSERT(lInsideFound || lOutsideFound);

		if (lInsideFound && lOutsideFound)
			return Geometry::CLIP_INTERSECT;
	}

	if (!lOutsideFound)
		return Geometry::CLIP_INTERSECT;
	else if (!lInsideFound) // Aucun sommet de la box a l'interieur du cone
	{
		// Parametrage des plans de la box pour la suite
		const Vector2f lXRange(-m_halfSize.x(), m_halfSize.x());
		const Vector2f lYRange(-m_halfSize.y(), m_halfSize.y());
		const Vector2f lZRange(-m_halfSize.z(), m_halfSize.z());

		const Vector3 lX(1.0f, 0.0f, 0.0f);
		const Vector3 lY(0.0f, 1.0f, 0.0f);
		const Vector3 lZ(0.0f, 0.0f, 1.0f);

		Bool lInsideFound = False, lOutsideFound = False, lSphereIntersect = False;

		Vector3 lBasePoint[4];
		cone.getAxisAlignedBase(0, lBasePoint[0]);
		cone.getAxisAlignedBase(1, lBasePoint[1]);
		cone.getAxisAlignedBase(2, lBasePoint[2]);
		cone.getAxisAlignedBase(3, lBasePoint[3]);

		if (include(lBasePoint[0]))	lInsideFound = True; else lOutsideFound = True;
		if (include(lBasePoint[1]))	lInsideFound = True; else lOutsideFound = True;
		if (include(lBasePoint[2])) lInsideFound = True; else lOutsideFound = True;
		if (include(lBasePoint[3])) lInsideFound = True; else lOutsideFound = True;
		if (include(cone.getOrigin())) lInsideFound = True; else lOutsideFound = True;

		if (cone.isSpheric())
		{
			Vector3 lNearest, lDir;

			lNearest = Geometry::nearest(m_center + Vector3(+m_halfSize.x(), 0.0f, 0.0f), lY, lZ, lYRange, lZRange, cone.getOrigin());
			lNearest -= cone.getOrigin(); lDir = lNearest; lDir.normalize();
			if (cone.doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);

			lNearest = Geometry::nearest(m_center + Vector3(-m_halfSize.x(), 0.0f, 0.0f), lY, lZ, lYRange, lZRange, cone.getOrigin());
			lNearest -= cone.getOrigin(); lDir = lNearest; lDir.normalize();
			if (cone.doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);

			lNearest = Geometry::nearest(m_center + Vector3(0.0f, +m_halfSize.y(), 0.0f), lX, lZ, lXRange, lZRange, cone.getOrigin());
			lNearest -= cone.getOrigin(); lDir = lNearest; lDir.normalize();
			if (cone.doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);

			lNearest = Geometry::nearest(m_center + Vector3(0.0f, -m_halfSize.y(), 0.0f), lX, lZ, lXRange, lZRange, cone.getOrigin());
			lNearest -= cone.getOrigin(); lDir = lNearest; lDir.normalize();
			if (cone.doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);

			lNearest = Geometry::nearest(m_center + Vector3(0.0f, 0.0f, +m_halfSize.z()), lX, lY, lXRange, lYRange, cone.getOrigin());
			lNearest -= cone.getOrigin(); lDir = lNearest; lDir.normalize();
			if (cone.doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);

			lNearest = Geometry::nearest(m_center + Vector3(0.0f, 0.0f, -m_halfSize.z()), lX, lY, lXRange, lYRange, cone.getOrigin());
			lNearest -= cone.getOrigin(); lDir = lNearest; lDir.normalize();
			if (cone.doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);
		}

		// Un point doit etre soit dedans soit dehors...
		O3D_ASSERT(lInsideFound || lOutsideFound);

		if (lInsideFound && lOutsideFound)
			return Geometry::CLIP_INTERSECT;
		else if (!lOutsideFound && !lSphereIntersect)
			return Geometry::CLIP_INSIDE; // A partir du moment ou SphereIntersect est vrai, on ne peut pas retourner INSIDE ou OUTSIDE
		else if (!lOutsideFound)
			return Geometry::CLIP_INTERSECT;

		// else if (lOutsideFound && !lInsideFound) (dernier cas)
		// Certain points du cone sont dehors de la box, et aucun n'a ete trouve dedans.
		// Alors la, on en sait rien ! on ne peut rien dire... faut continuer les tests.

		// cas le plus couteux mais excessivement rare (normalement)
		// on prend les 4 segments caracteristiques (ceux du bord du cone aligne avec les axes) puis
		// on regarde si il intersecte les faces de la box
		Vector3 lSegmentDir[4];
		Vector2f lSegmentRange[4];

		cone.getAxisAlignedSegment(0, lSegmentDir[0], lSegmentRange[0]);
		cone.getAxisAlignedSegment(1, lSegmentDir[1], lSegmentRange[1]);
		cone.getAxisAlignedSegment(2, lSegmentDir[2], lSegmentRange[2]);
		cone.getAxisAlignedSegment(3, lSegmentDir[3], lSegmentRange[3]);

		// On pourrait optimiser mais comme ce cas ne devrait pas arriver souvent, on va faire les 24 cas
		for (Int32 i = 0 ; i < 4 ; ++i)
		{
			if (Geometry::intersection(m_center + Vector3(+m_halfSize.x(), 0.0f, 0.0f), lY, lZ, lX, lYRange, lZRange, cone.getOrigin(), lSegmentDir[i], lSegmentRange[0]))
				return Geometry::CLIP_INTERSECT;
			if (Geometry::intersection(m_center + Vector3(-m_halfSize.x(), 0.0f, 0.0f), lY, lZ, lX, lYRange, lZRange, cone.getOrigin(), lSegmentDir[i], lSegmentRange[0]))
				return Geometry::CLIP_INTERSECT;
			if (Geometry::intersection(m_center + Vector3(0.0f, +m_halfSize.y(), 0.0f), lZ, lX, lY, lZRange, lXRange, cone.getOrigin(), lSegmentDir[i], lSegmentRange[0]))
				return Geometry::CLIP_INTERSECT;
			if (Geometry::intersection(m_center + Vector3(0.0f, -m_halfSize.y(), 0.0f), lZ, lX, lY, lZRange, lXRange, cone.getOrigin(), lSegmentDir[i], lSegmentRange[0]))
				return Geometry::CLIP_INTERSECT;
			if (Geometry::intersection(m_center + Vector3(0.0f, 0.0f, +m_halfSize.z()), lX, lY, lZ, lXRange, lYRange, cone.getOrigin(), lSegmentDir[i], lSegmentRange[0]))
				return Geometry::CLIP_INTERSECT;
			if (Geometry::intersection(m_center + Vector3(0.0f, 0.0f, -m_halfSize.z()), lX, lY, lZ, lXRange, lYRange, cone.getOrigin(), lSegmentDir[i], lSegmentRange[0]))
				return Geometry::CLIP_INTERSECT;
		}

		// A partir du moment ou SphereIntersect est vrai, on ne peut pas retourner INSIDE ou OUTSIDE
		return (lSphereIntersect ? Geometry::CLIP_INTERSECT : Geometry::CLIP_OUTSIDE);
	}
	else
	{
		O3D_ASSERT(0);
		return Geometry::CLIP_INTERSECT;
	}
}

void AABBox::toPlanes(Plane planes[6]) const
{
	planes[0].make(Vector3(-1,0,0), m_center-m_halfSize);
	planes[1].make(Vector3(+1,0,0), m_center+m_halfSize);
	planes[2].make(Vector3(0,-1,0), m_center-m_halfSize);
	planes[3].make(Vector3(0,+1,0), m_center+m_halfSize);
	planes[4].make(Vector3(0,0,-1), m_center-m_halfSize);
	planes[5].make(Vector3(0,0,+1), m_center+m_halfSize);
}

void AABBox::getCorners(Vector3 (&vects)[8]) const
{
	Float halfSizeX = m_halfSize[0];
	Float halfSizeY = m_halfSize[1];
	Float halfSizeZ = m_halfSize[2];
	Float centerX = m_center[0];
	Float centerY = m_center[1];
	Float centerZ = m_center[2];

	vects[0][0] = centerX-halfSizeX;
	vects[0][1] = centerY-halfSizeY;
	vects[0][2] = centerZ-halfSizeZ;

	vects[1][0] = centerX+halfSizeX;
	vects[1][1] = centerY-halfSizeY;
	vects[1][2] = centerZ-halfSizeZ;

	vects[2][0] = centerX+halfSizeX;
	vects[2][1] = centerY+halfSizeY;
	vects[2][2] = centerZ-halfSizeZ;

	vects[3][0] = centerX-halfSizeX;
	vects[3][1] = centerY+halfSizeY;
	vects[3][2] = centerZ-halfSizeZ;

	vects[4][0] = centerX-halfSizeX;
	vects[4][1] = centerY-halfSizeY;
	vects[4][2] = centerZ+halfSizeZ;

	vects[5][0] = centerX+halfSizeX;
	vects[5][1] = centerY-halfSizeY;
	vects[5][2] = centerZ+halfSizeZ;

	vects[6][0] = centerX+halfSizeX;
	vects[6][1] = centerY+halfSizeY;
	vects[6][2] = centerZ+halfSizeZ;

	vects[7][0] = centerX-halfSizeX;
	vects[7][1] = centerY+halfSizeY;
	vects[7][2] = centerZ+halfSizeZ;
}

// Clamp a point coordinate into the bbox.
Vector3 AABBox::clamp(const Vector3 &point) const
{
	Vector3 min = getMin();
	Vector3 max = getMax();

	return Vector3(
			o3d::clamp(point.x(), min.x(), max.x()),
			o3d::clamp(point.y(), min.y(), max.y()),
			o3d::clamp(point.z(), min.z(), max.z()));
}

AABBox AABBox::buildIntersection(const AABBox &b1, const AABBox &b2)
{
	AABBox result;
	Vector3 vMin;
	Vector3 vMax;
	Vector3 vMin1 = b1.getMin();
	Vector3 vMax1 = b1.getMax();
	Vector3 vMin2 = b2.getMin();
	Vector3 vMax2 = b2.getMax();

	// don't test if intersect or not.
	vMax.maxOf(vMax1, vMax2);
	vMin.minOf(vMin1, vMin2);
	result.setMinMax(vMin, vMax);
	return result;
}

AABBox AABBox::buildUnion(const AABBox &b1, const AABBox &b2)
{
	AABBox result;
	Vector3 vMin;
	Vector3 vMax;
	Vector3 vMin1 = b1.getMin();
	Vector3 vMax1 = b1.getMax();
	Vector3 vMin2 = b2.getMin();
	Vector3 vMax2 = b2.getMax();

	vMax.maxOf(vMax1, vMax2);
	vMin.minOf(vMin1, vMin2);
	result.setMinMax(vMin, vMax);
	return result;
}

AABBox AABBox::transformAABBox(const Matrix4 &mat, const AABBox &box)
{
	// TODO : optimize this a bit if possible...
	AABBox result;
	Vector3 tmp;

	// compute corners.
	Vector3	p[8];
	Vector3	vMin = box.getMin();
	Vector3	vMax = box.getMax();
	p[0].set(vMin[0], vMin[1], vMin[2]);
	p[1].set(vMax[0], vMin[1], vMin[2]);
	p[2].set(vMin[0], vMax[1], vMin[2]);
	p[3].set(vMax[0], vMax[1], vMin[2]);
	p[4].set(vMin[0], vMin[1], vMax[2]);
	p[5].set(vMax[0], vMin[1], vMax[2]);
	p[6].set(vMin[0], vMax[1], vMax[2]);
	p[7].set(vMax[0], vMax[1], vMax[2]);

	vMin = vMax = mat * p[0];
	// transform corners.
	for(UInt32 i=1;i<8;i++)
	{
		tmp = mat * p[i];
		vMin.minOf(vMin, tmp);
		vMax.maxOf(vMax, tmp);
	}

	result.setMinMax(vMin, vMax);

	return result;
}

// Serialization
Bool AABBox::writeToFile(OutStream &os)const
{
    os	<< m_center
        << m_halfSize;

	return True;
}

Bool AABBox::readFromFile(InStream &is)
{
    is	>> m_center
        >> m_halfSize;

	return True;
}

//---------------------------------------------------------------------------------------
// class AABBoxExt
//---------------------------------------------------------------------------------------

// Clipping functions
Bool AABBoxExt::clipFront(const Plane &p) const
{
	// Assume normalized planes.
	Float distance = p*m_center;

	// if( SphereMax OUT )	return false.
	if (distance<-m_radiusMax)
		return False;

	// if( SphereMin IN )	return true;
	if (distance>-m_radiusMin)
		return True;

	// else, standard clip box.
	return AABBox::clipFront(p);
}

Bool AABBoxExt::clipBack(const Plane &p) const
{
	// Assume normalized planes.
	Float distance = p*m_center;

	// if( SphereMax OUT )	return false.
	if (distance<m_radiusMax)
		return False;

	// if( SphereMin IN )	return true;
	if (distance>m_radiusMin)
		return True;

	// else, standard clip box.
	return AABBox::clipBack(p);
}

// Apply a matrix transformation on an AABBoxExt and return the result as new a bounding box
AABBoxExt AABBoxExt::transformTo(const Matrix4 &mat) const
{
	// TODO : optimize this a bit if possible...
	AABBoxExt result;
	Vector3 tmp;

	// compute corners.
	Vector3	p[8];
	Vector3	vMin = getMin();
	Vector3	vMax = getMax();
	p[0].set(vMin[0], vMin[1], vMin[2]);
	p[1].set(vMax[0], vMin[1], vMin[2]);
	p[2].set(vMin[0], vMax[1], vMin[2]);
	p[3].set(vMax[0], vMax[1], vMin[2]);
	p[4].set(vMin[0], vMin[1], vMax[2]);
	p[5].set(vMax[0], vMin[1], vMax[2]);
	p[6].set(vMin[0], vMax[1], vMax[2]);
	p[7].set(vMax[0], vMax[1], vMax[2]);

	vMin = vMax = mat * p[0];
	// transform corners.
	for(UInt32 i=1;i<8;i++)
	{
		tmp = mat * p[i];
		vMin.minOf(vMin, tmp);
		vMax.maxOf(vMax, tmp);
	}

	result.setMinMax(vMin, vMax);

	return result;
}

// Serialization
Bool AABBoxExt::writeToFile(OutStream &os)const
{
    AABBox::writeToFile(os);
	return True;
}

Bool AABBoxExt::readFromFile(InStream &is)
{
    AABBox::readFromFile(is);
	updateRadius();
	return True;
}


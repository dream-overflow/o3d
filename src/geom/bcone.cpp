/**
 * @file bcone.cpp
 * @brief Bounding Cone.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2010-02-18
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/geom/bcone.h"

#include "o3d/core/debug.h"
#include "o3d/core/vector3.h"

#include "o3d/geom/plane.h"
#include "o3d/geom/bsphere.h"
#include "o3d/geom/aabbox.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

//-----------------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------------
BCone::BCone():
	m_origin(),
	m_direction(),
	m_depth(0.0f),
	m_cutoff(0.0f),
	m_spheric(False),
	m_isValid(False),
	m_upToDate(UPDATE_ALL),
	m_outSphere(),
	m_inSphere()
{
	memset(m_precomputed, 0, sizeof(m_precomputed));
}

BCone::BCone(const BCone & cone):
	m_origin(cone.m_origin),
	m_direction(cone.m_direction),
	m_depth(cone.m_depth),
	m_cutoff(cone.m_cutoff),
	m_spheric(cone.m_spheric),
	m_isValid(cone.m_isValid),
	m_upToDate(cone.m_upToDate),
	m_outSphere(),
	m_inSphere()
{
	memcpy(m_precomputed, cone.m_precomputed, sizeof(m_precomputed));
}

BCone::BCone(	const Vector3 & origin,
					const Vector3 & direction,
					Float depth,
					Float cutoff,
					Bool spheric):
	m_origin(origin),
	m_direction(direction),
	m_depth(depth),
	m_cutoff(cutoff),
	m_spheric(spheric),
	m_isValid(False),
	m_upToDate(UPDATE_ALL),
	m_outSphere(),
	m_inSphere()
{
    memset(m_precomputed, 0, sizeof(m_precomputed));
}

BCone & BCone::operator = (const BCone & cone)
{
	if (this == &cone)
		return *this;

	m_origin = cone.m_origin;
	m_direction = cone.m_direction;
	m_depth = cone.m_depth;
	m_cutoff = cone.m_cutoff;
	m_spheric = cone.m_spheric;
	m_isValid = cone.m_isValid;
	m_upToDate = cone.m_upToDate;
	m_outSphere = cone.m_outSphere;
	m_inSphere = cone.m_inSphere;

	memcpy(m_precomputed, cone.m_precomputed, sizeof(m_precomputed));

	return *this;
}

Bool BCone::operator == (const BCone & cone) const
{
	return ((m_origin == cone.m_origin) && 
			(m_direction == cone.m_direction) && 
			(m_depth == cone.m_depth) && 
			(m_cutoff == cone.m_cutoff) && 
			(m_spheric == cone.m_spheric));
}

Bool BCone::operator != (const BCone & cone) const
{
	return !operator == (cone);
}

Bool BCone::isValid() const
{
	update(STATE);

	return m_isValid;
}

//-----------------------------------------------------------------------------------
// Global parameters
//-----------------------------------------------------------------------------------

void BCone::setOrigin(const Vector3 & origin)
{
	if (origin != m_origin)
	{
		m_origin = origin;

		m_upToDate |= (IN_SPHERE | OUT_SPHERE | PRECOMPUTED_9_11 | PRECOMPUTED_12_15);
	}
}

void BCone::setDirection(const Vector3 & direction)
{
	if (m_direction != direction)
	{
		m_direction = direction;

		m_upToDate |= (IN_SPHERE | OUT_SPHERE | STATE | PRECOMPUTED_3_8 | PRECOMPUTED_9_11 | PRECOMPUTED_12_15);
	}
}

void BCone::setDepth(Float depth)
{
	if (m_depth != depth)
	{
        m_depth = depth;

		m_upToDate |= (IN_SPHERE | OUT_SPHERE | STATE | PRECOMPUTED_9_11 | PRECOMPUTED_12_15);
	}
}

void BCone::setCutoff(Float cutoff)
{
	if (m_cutoff != cutoff)
	{
		m_cutoff = cutoff;

		m_upToDate |= (IN_SPHERE | OUT_SPHERE | STATE | PRECOMPUTED_0_2 | PRECOMPUTED_3_8 | PRECOMPUTED_12_15);
	}
}

void BCone::setSpheric(Bool value)
{
	if (m_spheric != value)
	{
		m_spheric = value;

		m_upToDate |= (IN_SPHERE | OUT_SPHERE | STATE | PRECOMPUTED_12_15);
	}
}

//-----------------------------------------------------------------------------------
// Precomputed data
//-----------------------------------------------------------------------------------

Float BCone::getCosCutoff() const
{
	update(PRECOMPUTED_0_2);

	return m_precomputed[DATA_COS_CUTOFF];
}

Float BCone::getSinCutoff() const
{
	update(PRECOMPUTED_0_2);

	return m_precomputed[DATA_SIN_CUTOFF];
}

Float BCone::getTanCutoff() const
{
	update(PRECOMPUTED_0_2);

	return m_precomputed[DATA_TAN_CUTOFF];
}

const BSphere & BCone::getOutBSphere() const
{
	update(OUT_SPHERE);

	return m_outSphere;
}

const BSphere & BCone::getInBSphere() const
{
	update(IN_SPHERE);

	return m_inSphere;
}

const Vector3 & BCone::getBaseBasisH() const
{
	update(PRECOMPUTED_3_8);

	return *(Vector3*)(&m_precomputed[DATA_VEC_BASEH_X]);
}

const Vector3 & BCone::getBaseBasisV() const
{
	update(PRECOMPUTED_3_8);

	return *(Vector3*)(&m_precomputed[DATA_VEC_BASEV_X]);
}

const Vector3 & BCone::getTopPoint() const
{
	update(PRECOMPUTED_9_11);

	return *(Vector3*)(&m_precomputed[DATA_VEC_TOP_POINT_X]);
}

const Vector3 & BCone::getBasePoint() const
{
	update(PRECOMPUTED_12_15);

	return *(Vector3*)(&m_precomputed[DATA_VEC_BASE_POINT_X]);
}

void BCone::getAxisAlignedSegment(Int32 i, Vector3 & dir, Vector2f & range) const
{
	update(PRECOMPUTED_3_8);

	switch(i)
	{
	case 0:
		dir = getBasePoint() - m_origin + (m_spheric ? m_depth*getSinCutoff() : m_depth*getTanCutoff()) * getBaseBasisH();
		range.set(0.0f, m_spheric ? m_depth : m_depth / getCosCutoff());
	break;
	case 1:
		dir = getBasePoint() - m_origin - (m_spheric ? m_depth*getSinCutoff() : m_depth*getTanCutoff()) * getBaseBasisH();
		range.set(0.0f, m_spheric ? m_depth : m_depth / getCosCutoff());
	break;
	case 2:
		dir = getBasePoint() - m_origin + (m_spheric ? m_depth*getSinCutoff() : m_depth*getTanCutoff()) * getBaseBasisV();
		range.set(0.0f, m_spheric ? m_depth : m_depth / getCosCutoff());
	break;
	case 3:
		dir = getBasePoint() - m_origin - (m_spheric ? m_depth*getSinCutoff() : m_depth*getTanCutoff()) * getBaseBasisV();
		range.set(0.0f, m_spheric ? m_depth : m_depth / getCosCutoff());
	break;
	default:
		O3D_ERROR(E_InvalidOperation("Invalid index for axis aligned segment"));
		break;
	}

	dir.normalize();
}

void BCone::getAxisAlignedBase(Int32 i, Vector3 & vec) const
{
	update(PRECOMPUTED_3_8);

	if (m_spheric)
	{
		switch(i)
		{
		case 0:
			(vec = getBasePoint()) += (m_depth*getSinCutoff()) * getBaseBasisH();
		break;
		case 1:
			(vec = getBasePoint()) -= (m_depth*getSinCutoff()) * getBaseBasisH();
		break;
		case 2:
			(vec = getBasePoint()) += (m_depth*getSinCutoff()) * getBaseBasisV();
		break;
		case 3:
			(vec = getBasePoint()) -= (m_depth*getSinCutoff()) * getBaseBasisV();
		break;
		default:
			O3D_ERROR(E_InvalidOperation("Invalid index for axis aligned base vector"));
			break;
		}
	}
	else
	{
		switch(i)
		{
		case 0:
			(vec = getTopPoint()) += (m_depth*getTanCutoff()) * getBaseBasisH();
		break;
		case 1:
			(vec = getTopPoint()) -= (m_depth*getTanCutoff()) * getBaseBasisH();
		break;
		case 2:
			(vec = getTopPoint()) += (m_depth*getTanCutoff()) * getBaseBasisV();
		break;
		case 3:
			(vec = getTopPoint()) -= (m_depth*getTanCutoff()) * getBaseBasisV();
		break;
		default:
			O3D_ERROR(E_InvalidOperation("Invalid index for axis aligned base vector"));
			break;
		}
	}
}

Bool BCone::doesDirectionLieIn(const Vector3 & dir) const
{
	return dir*m_direction >= getCosCutoff();
}

Bool BCone::doesDirectionLieIn(Float x, Float y, Float z) const
{
	return x*m_direction.x() + y*m_direction.y() + z*m_direction.z() >= getCosCutoff();
}

Bool BCone::isPointSeen(const Vector3 & pt) const
{
	const Vector3 lDif(pt - m_origin);

	return lDif*m_direction >= getCosCutoff() * lDif.length();
}

//-----------------------------------------------------------------------------------
// Clipping
//-----------------------------------------------------------------------------------

Bool BCone::intersect(const Plane & plane) const
{
	if (!isValid())
	{
		O3D_WARNING("Invalid BCone object");
		return False;
	}

	// If the bounding sphere does not intersect the plane
	if (!getOutBSphere().intersect(plane))
		return False;

	if (getInBSphere().intersect(plane))
		return True;

	Vector3 lDir((m_direction ^ plane.getNormal()) ^ m_direction); // Vecteur de la base qui va le plus "vite" vers le plan

	if (lDir.normalize() < o3d::Limits<Float>::epsilon())
	{
		lDir = Vector3(1.0f, 0.0f, 0.0f) ^ m_direction;

		if (lDir.normalize() < o3d::Limits<Float>::epsilon())
		{
			lDir = Vector3(0.0f, 1.0f, 0.0f) ^ m_direction;
			lDir.normalize();
		}
	}

	if (!m_spheric)
	{
		// Point de la base le plus proche du plan
		const Vector3 lNearestPoint = m_origin + m_depth * (m_direction + getTanCutoff() * lDir);
		const Float lDist1 = plane.operator *(lNearestPoint);
		const Float lDist2 = plane.operator *(m_origin);

		if (((lDist1 > 0.0f) && (lDist2 > 0.0f)) || ((lDist1 < 0.0f) && (lDist2 < 0.0f)))
			return False;
		else
			return True;
	}
	else
	{
		// On sait deja que le plan intersecte la sphere de rayon "depth"
		const Vector3 lTopPoint = m_origin + m_depth * m_direction;

		// Point le plus pres dans la partie spherique
		Vector3 lNearestPoint = ((plane.getPoint() - m_origin)*plane.getNormal()) * plane.getNormal();
		
		// Point de la base le plus proche du plan
		if (doesDirectionLieIn(lNearestPoint))
		{
			if (lNearestPoint.normalize() > o3d::Limits<Float>::epsilon())
				(lNearestPoint = m_origin) += m_depth*lNearestPoint; // En realite, ce n'est pas toujours le point le plus pres
			else // Means the base center is on the plane
				return True;
		}
		else
			(lNearestPoint = m_origin) += m_depth*(getCosCutoff()*m_direction + getSinCutoff()*lDir);

		const Float lDist1 = plane.operator*(lNearestPoint);
		const Float lDist2 = plane.operator*(m_origin);
		const Float lDist3 = plane.operator*(lTopPoint);

		if (((lDist1 > 0.0f) && (lDist2 > 0.0f) && (lDist3 > 0.0f)) || ((lDist1 < 0.0f) && (lDist2 < 0.0f) && (lDist3 < 0.0f)))
			return False;
		else
			return True;
	}
}

Bool BCone::intersect(const BSphere & sphere) const
{
	return (clip(sphere) != Geometry::CLIP_OUTSIDE);
}

Bool BCone::intersect(const AABBox & box) const
{
	return (clip(box) != Geometry::CLIP_OUTSIDE);
}

Geometry::Clipping BCone::clip(const Plane & plane) const
{
	if (!isValid())
	{
		O3D_WARNING("Invalid BCone object");
		return Geometry::CLIP_OUTSIDE;
	}

	// If the bounding sphere does not intersect the plane
	if (getOutBSphere().clip(plane) == Geometry::CLIP_OUTSIDE)
		return Geometry::CLIP_OUTSIDE;

	if (getInBSphere().clip(plane) != Geometry::CLIP_OUTSIDE)
		return Geometry::CLIP_INTERSECT;

	Vector3 lPlaneToBase(((m_origin + m_depth*m_direction - plane.getPoint())*plane.getNormal())*plane.getNormal());
	lPlaneToBase.normalize();

	Vector3 lDir(m_direction ^ (m_direction ^ lPlaneToBase)); // Vecteur de la base qui va le plus "vite" vers le plan

	if (lDir.normalize() < o3d::Limits<Float>::epsilon())
	{
		lDir = Vector3(1.0f, 0.0f, 0.0f) ^ m_direction;

		if (lDir.normalize() < o3d::Limits<Float>::epsilon())
		{
			lDir = Vector3(0.0f, 1.0f, 0.0f) ^ m_direction;
			lDir.normalize();
		}
	}

	if (!m_spheric)
	{
		// Point de la base le plus proche du plan
		const Vector3 lNearestPoint = m_origin + m_depth * (m_direction + getTanCutoff() * lDir);
		const Vector3 lTopPoint = m_origin + m_depth * m_direction;
		const Float lDist1 = plane.operator *(lNearestPoint);
		const Float lDist2 = plane.operator *(m_origin);
		const Float lDist3 = plane.operator *(lTopPoint);

		if (((lDist1 > 0.0f) && (lDist2 > 0.0f) && (lDist3 > 0.0f)) || ((lDist1 < 0.0f) && (lDist2 < 0.0f) && (lDist3 < 0.0f)))
			return Geometry::CLIP_OUTSIDE;
		else
			return Geometry::CLIP_INTERSECT;
	}
	else
	{
		// On sait deja que le plan intersecte la sphere de rayon "depth"

		const Vector3 lTopPoint = m_origin + m_depth * m_direction;

		// Point le plus pres dans la partie spherique
		Vector3 lNearestPoint = ((plane.getPoint() - m_origin)*plane.getNormal()) * plane.getNormal();
		
		// Point de la base le plus proche du plan
		if (doesDirectionLieIn(lNearestPoint))
		{
			if (lNearestPoint.normalize() > o3d::Limits<Float>::epsilon())
				(lNearestPoint = m_origin) += m_depth*lNearestPoint; // En realite, ce n'est pas toujours le point le plus pres
			else // Means the base center is on the plane
				return Geometry::CLIP_INTERSECT;
		}
		else
			(lNearestPoint = m_origin) += m_depth*(getCosCutoff()*m_direction + getSinCutoff()*lDir);

		const Float lDist1 = plane.operator*(lNearestPoint);
		const Float lDist2 = plane.operator*(m_origin);
		const Float lDist3 = plane.operator*(lTopPoint);

		if (((lDist1 > 0.0f) && (lDist2 > 0.0f) && (lDist3 > 0.0f)) || ((lDist1 < 0.0f) && (lDist2 < 0.0f) && (lDist3 < 0.0f)))
			return Geometry::CLIP_OUTSIDE;
		else
			return Geometry::CLIP_INTERSECT;
	}
}

Geometry::Clipping BCone::clip(const BSphere & sphere) const
{
	if (!isValid())
	{
		O3D_WARNING("Invalid BCone object");
		return Geometry::CLIP_OUTSIDE;
	}

	// Si la sphere interieure du cone contient 'sphere'
	if (getInBSphere().clip(sphere) == Geometry::CLIP_INSIDE)
		return Geometry::CLIP_INSIDE;

	// Si la sphere englobante du cone n'intersecte pas 'sphere'
	if (getOutBSphere().clip(sphere) == Geometry::CLIP_OUTSIDE)
		return Geometry::CLIP_OUTSIDE;

	// Note: Globalement, le sens Cone.clip(Sphere) est tres voisin de Sphere.clip(Cone)
	// Si Sphere englobe Cone alors Cone intersect sphere.
	// La seule possibilite pour que 'sphere' soit dans 'cone' a ete testee qq lignes au dessus,
	// donc la fonction ne retournera plus CLIP_INSIDE dans la suite.

	const Vector3 & lSCenter = sphere.getCenter();
	const Float lSRadius = sphere.getRadius();
	const Float lSqrSRadius = o3d::sqr<Float>(lSRadius);

	const Float lSinCutoff = getSinCutoff();
	const Float lCosCutoff = getCosCutoff();
	const Float lTanCutoff = getTanCutoff();

	const Vector3 lCenterToOrigin(m_origin - lSCenter);
	Vector3 lVertical(lCenterToOrigin ^ m_direction);

	if (lVertical.normalize() < o3d::Limits<Float>::epsilon())
	{
		lVertical = Vector3(1.0f, 0.0f, 0.0f) ^ m_direction;

		if (lVertical.normalize() < o3d::Limits<Float>::epsilon())
		{
			lVertical = Vector3(0.0f, 1.0f, 0.0f) ^ m_direction;
			lVertical.normalize();
		}
	}

	const Vector3 lAxeToCenter(m_direction ^ lVertical);

	if (m_spheric)
	{
		const Vector3 lTopToCenter = lSCenter - m_origin; // L'origine du repere 2D est differente dans le cas non spheric
		const Float lHalfBase = m_depth*lSinCutoff;

		// Changement de coordonnees. 3D => 2D
		const Vector2f lCoord(o3d::abs<Float>(lTopToCenter * lAxeToCenter), -(lTopToCenter * m_direction));
		const Vector2f lSqrCoord(o3d::sqr<Float>(lCoord.x()), o3d::sqr<Float>(lCoord.y()));

		const Float lF2 = - lCoord.x() / lTanCutoff; // <=> * tan(o3d::HalfPi - cutoff);
		const Float lF3 = lCoord.x() * lTanCutoff;

		Float lT;

		// Test lorsque le centre du cercle est a l'exterieur du triangle
		if (lCoord.y() <= lF2) // Zone 1
		{
			if (lSqrCoord.x() + lSqrCoord.y() > o3d::sqr<Float>(m_depth + lSRadius))
				return Geometry::CLIP_OUTSIDE;
			else
				return Geometry::CLIP_INTERSECT;
		}
		else if ((m_cutoff < o3d::HALF_PI) && (lCoord.y() >= 0.0f) && (lCoord.y() >= lF3)) // Zone 4
		{
			if (lSqrCoord.x() + lSqrCoord.y() > lSqrSRadius)
				return Geometry::CLIP_OUTSIDE;
			else
				return Geometry::CLIP_INTERSECT;
		}
		else if ((lCoord.y() >= lF2) && ((lT = (lCoord.x() - lHalfBase)*lSinCutoff + (lCoord.y() + m_depth*lCosCutoff)*(-lCosCutoff)) <= 0.0f)) // Zone 3
		{
			if (lCoord.y() > lF2 + lSRadius/lSinCutoff)
				return Geometry::CLIP_OUTSIDE;
			else
				return Geometry::CLIP_INTERSECT;
		}
		// Calcul de la distance par rapport a la droite f1
		else if ((lT = (lCoord.x() - lHalfBase)*lSinCutoff + (lCoord.y() + m_depth*lCosCutoff)*(-lCosCutoff)) >= 0.0f) // Zone 2
		{
			if (lT > lSRadius)
				return Geometry::CLIP_OUTSIDE;
			else if (o3d::sqr<Float>(lCoord.x() - lHalfBase) + o3d::sqr<Float>(lCoord.y() + m_depth*lCosCutoff) > lSqrSRadius)
				return Geometry::CLIP_OUTSIDE;
			else
				return Geometry::CLIP_INTERSECT;
		}

		// Sinon le centre du cercle est dans le cone
		O3D_ASSERT((lCoord.y() <= lF2) && (lSqrCoord.x() + lSqrCoord.y() <= lSqrSRadius));

		return Geometry::CLIP_INTERSECT;
	}
	else
	{
		const Vector3 lTopToCenter = lSCenter - m_origin - m_depth*m_direction;
		const Float lHalfBase = m_depth * lTanCutoff;

		// Changement de coordonnees. 3D => 2D
		const Vector2f lCoord(o3d::abs<Float>(lTopToCenter * lAxeToCenter), - (lTopToCenter * m_direction));
		const Vector2f lSqrCoord(o3d::sqr<Float>(lCoord.x()), o3d::sqr<Float>(lCoord.y()));

		const Float lF1 = (lCoord.x() - lHalfBase) * lTanCutoff;
		const Float lF2 = m_depth - lCoord.x() / lTanCutoff; // <=> * tan(o3d::HalfPi - cutoff);
		const Float lF3 = m_depth + lCoord.x() * lTanCutoff;

		// Test lorsque le centre du cercle est a l'exterieur du triangle
		if ((lCoord.y() <= 0.0f) && (lCoord.x() <= lHalfBase)) // Zone 1
		{
			if (lCoord.y() < -lSRadius)
				return Geometry::CLIP_OUTSIDE;
			else
				return Geometry::CLIP_INTERSECT;
		}
		else if ((lCoord.x() >= lHalfBase) && (lCoord.y() <= lF1)) // Zone 2
		{
			if (o3d::sqr<Float>(lCoord.x() - lHalfBase) + lSqrCoord.y() > lSqrSRadius)
				return Geometry::CLIP_OUTSIDE;
			else
				return Geometry::CLIP_INTERSECT;
		}
		else if (lCoord.y() >= lF3) // Zone 4
		{
			if (lSqrCoord.x() + o3d::sqr<Float>(lCoord.y() - m_depth) > lSqrSRadius)
				return Geometry::CLIP_OUTSIDE;
			else
				return Geometry::CLIP_INTERSECT;
		}
		else if (lCoord.y() >= lF2) // Zone 3
		{
			if (lCoord.y() > lF2 + lSRadius/lSinCutoff)
				return Geometry::CLIP_OUTSIDE;
			else
				return Geometry::CLIP_INTERSECT;
		}

		// Sinon le centre du cercle est dans le cone
		O3D_ASSERT((lCoord.y() <= m_depth) && (lCoord.y() >= 0.0f) && (lCoord.x() <= lHalfBase));

		return Geometry::CLIP_INTERSECT;
	}
}

Geometry::Clipping BCone::clip(const AABBox & box) const
{
	if (!isValid())
	{
		O3D_WARNING("Invalid BCone object");
		return Geometry::CLIP_OUTSIDE;
	}

	const Vector3 & lBCenter = box.getCenter();

	const Float lCosCutoff = getCosCutoff();
	const Float lSinCutoff = getSinCutoff();
	const Float lTanCutoff = getTanCutoff();

	const Vector3 lC2C(m_origin - lBCenter); // Center to center

	// Test "rapide" avec une bounding sphere
	if (box.clip(getOutBSphere()) == Geometry::CLIP_OUTSIDE)
		return Geometry::CLIP_OUTSIDE;

	// Effectue le test uniquement si la box est petite comparee au cone
	if ((box.getHalfSize().normInf() < 0.2f * getInBSphere().getRadius()) && getInBSphere().include(box))
		return Geometry::CLIP_INSIDE;

	// on prend chaque sommet, on change de base leurs coordonnees (on passe dans la base du cone)
	const Vector3 & lBaseH = getBaseBasisH();
	const Vector3 & lBaseV = getBaseBasisV();

	Vector3 lCorners[8];
	box.getCorners(lCorners);

	const Float lRange[2] = { m_spheric ? m_depth*o3d::min<Float>(lCosCutoff, 0.0f) : 0.0f, m_depth };
	const Float lTresholdSpheric = m_depth*lCosCutoff;

	// Ce coefficient permet de calculer la largeur du cone a une abscisse donnee le long de son axe:
	// rayon(x) = lBaseCoef * x; avec x >= 0 pour un cone a base plate
	// Le coefficient est negatif pour un cone a base spherique avec un angle d'ouverture > 90d
	const Float lBaseCoef = (m_spheric ? m_depth*lSinCutoff : m_depth * lTanCutoff)
						/ (m_spheric ? m_depth*lCosCutoff : m_depth);

	// Carre du rayon max du cone
	const Float lSqrConeWidth = o3d::sqr<Float>(m_spheric ? m_depth * (m_cutoff < o3d::HALF_PI ? lSinCutoff : 1.0f) : m_depth * lTanCutoff);

	// Carre de la longueur du cone
	const Float lSqrDepth = o3d::sqr<Float>(m_depth);

	Bool lOutsideFound = False;
	Bool lInsideFound = False;

	Float lProjOnAxe, lSqrDistFromAxe;
	Vector2f lProjOnBase;
	Vector3 lDif;

	for (Int32 i = 0 ; i < 8 ; ++i)
	{
		lDif = lCorners[i]; lDif -= m_origin;

		lProjOnAxe = lDif * m_direction;
		lProjOnBase.set(lDif * lBaseH, lDif * lBaseV);
		lSqrDistFromAxe = lProjOnBase.squareLength();

		if ((lProjOnAxe < lRange[0]) || (lProjOnAxe > lRange[1]))
			lOutsideFound = True;
		else if (lSqrDistFromAxe > lSqrConeWidth)
			lOutsideFound = True;
		else
		{
			if (m_spheric) // Un peu plus complexe pour le cone sph�rique
			{
				if (lDif.squareLength() > lSqrDepth)
					lOutsideFound = True;
				else
				{
					if (m_cutoff > o3d::HALF_PI) // Cas un peu tordu mais possible
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
						// Signifie que l'on est dans la phase lin�aire
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

		if (lInsideFound && lOutsideFound)
			return Geometry::CLIP_INTERSECT;
	}

	if (!lOutsideFound)
		return Geometry::CLIP_INSIDE;
	else if (!lInsideFound) // Aucun sommet de la box a l'interieur du cone
	{
		// Parametrage des plans de la box pour la suite
		const Vector2f lXRange(-box.getHalfSize().x(), box.getHalfSize().x());
		const Vector2f lYRange(-box.getHalfSize().y(), box.getHalfSize().y());
		const Vector2f lZRange(-box.getHalfSize().z(), box.getHalfSize().z());

		const Vector3 lX(1.0f, 0.0f, 0.0f);
		const Vector3 lY(0.0f, 1.0f, 0.0f);
		const Vector3 lZ(0.0f, 0.0f, 1.0f);

		Bool lInsideFound = False, lOutsideFound = False, lSphereIntersect = False;

		Vector3 lBasePoint[4];
		getAxisAlignedBase(0, lBasePoint[0]);
		getAxisAlignedBase(1, lBasePoint[1]);
		getAxisAlignedBase(2, lBasePoint[2]);
		getAxisAlignedBase(3, lBasePoint[3]);

		if (box.include(lBasePoint[0]))	lInsideFound = True; else lOutsideFound = True;
		if (box.include(lBasePoint[1]))	lInsideFound = True; else lOutsideFound = True;
		if (box.include(lBasePoint[2])) lInsideFound = True; else lOutsideFound = True;
		if (box.include(lBasePoint[3])) lInsideFound = True; else lOutsideFound = True;
		if (box.include(getOrigin())) lInsideFound = True; else lOutsideFound = True;

		if (isSpheric())
		{
			Vector3 lNearest, lDir;

			lNearest = Geometry::nearest(box.getCenter() + Vector3(+box.getHalfSize().x(), 0.0f, 0.0f), lY, lZ, lYRange, lZRange, getOrigin());
			lNearest -= getOrigin(); lDir = lNearest; lDir.normalize();
			if (doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);

			lNearest = Geometry::nearest(box.getCenter() + Vector3(-box.getHalfSize().x(), 0.0f, 0.0f), lY, lZ, lYRange, lZRange, getOrigin());
			lNearest -= getOrigin(); lDir = lNearest; lDir.normalize();
			if (doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);

			lNearest = Geometry::nearest(box.getCenter() + Vector3(0.0f, +box.getHalfSize().y(), 0.0f), lX, lZ, lXRange, lZRange, getOrigin());
			lNearest -= getOrigin(); lDir = lNearest; lDir.normalize();
			if (doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);

			lNearest = Geometry::nearest(box.getCenter() + Vector3(0.0f, -box.getHalfSize().y(), 0.0f), lX, lZ, lXRange, lZRange, getOrigin());
			lNearest -= getOrigin(); lDir = lNearest; lDir.normalize();
			if (doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);

			lNearest = Geometry::nearest(box.getCenter() + Vector3(0.0f, 0.0f, +box.getHalfSize().z()), lX, lY, lXRange, lYRange, getOrigin());
			lNearest -= getOrigin(); lDir = lNearest; lDir.normalize();
			if (doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);

			lNearest = Geometry::nearest(box.getCenter() + Vector3(0.0f, 0.0f, -box.getHalfSize().z()), lX, lY, lXRange, lYRange, getOrigin());
			lNearest -= getOrigin(); lDir = lNearest; lDir.normalize();
			if (doesDirectionLieIn(lDir)) lSphereIntersect |= (lNearest.squareLength() < lSqrDepth);
		}

		// Un point doit etre soit dedans soit dehors...
		O3D_ASSERT(lInsideFound || lOutsideFound);

		if (lInsideFound || lSphereIntersect)
			return Geometry::CLIP_INTERSECT;

		// else if (!lInsideFound && lOutsideFound) (dernier cas)
		// Certain points du cone sont dehors de la box, et aucun n'a �t� trouv� dedans.
		// Alors la, on en sait rien ! on ne peut rien dire... faut continuer les tests.

		// cas le plus couteux mais excessivement rare (normalement)
		// on prend les 4 segments caract�ristiques (ceux du bord du cone align� avec les axes) puis
		// on regarde si il intersecte les faces de la box
		Vector3 lSegmentDir[4];
		Vector2f lSegmentRange[4];

		getAxisAlignedSegment(0, lSegmentDir[0], lSegmentRange[0]);
		getAxisAlignedSegment(1, lSegmentDir[1], lSegmentRange[1]);
		getAxisAlignedSegment(2, lSegmentDir[2], lSegmentRange[2]);
		getAxisAlignedSegment(3, lSegmentDir[3], lSegmentRange[3]);

		// On pourrait optimiser mais comme cas devrait pas arriver souvent, on va faire les 24 cas
		for (Int32 i = 0 ; i < 4 ; ++i)
		{
			if (Geometry::intersection(box.getCenter() + Vector3(+box.getHalfSize().x(), 0.0f, 0.0f), lY, lZ, lX, lYRange, lZRange, getOrigin(), lSegmentDir[i], lSegmentRange[0]))
				return Geometry::CLIP_INTERSECT;
			if (Geometry::intersection(box.getCenter() + Vector3(-box.getHalfSize().x(), 0.0f, 0.0f), lY, lZ, lX, lYRange, lZRange, getOrigin(), lSegmentDir[i], lSegmentRange[0]))
				return Geometry::CLIP_INTERSECT;
			if (Geometry::intersection(box.getCenter() + Vector3(0.0f, +box.getHalfSize().y(), 0.0f), lZ, lX, lY, lZRange, lXRange, getOrigin(), lSegmentDir[i], lSegmentRange[0]))
				return Geometry::CLIP_INTERSECT;
			if (Geometry::intersection(box.getCenter() + Vector3(0.0f, -box.getHalfSize().y(), 0.0f), lZ, lX, lY, lZRange, lXRange, getOrigin(), lSegmentDir[i], lSegmentRange[0]))
				return Geometry::CLIP_INTERSECT;
			if (Geometry::intersection(box.getCenter() + Vector3(0.0f, 0.0f, +box.getHalfSize().z()), lX, lY, lZ, lXRange, lYRange, getOrigin(), lSegmentDir[i], lSegmentRange[0]))
				return Geometry::CLIP_INTERSECT;
			if (Geometry::intersection(box.getCenter() + Vector3(0.0f, 0.0f, -box.getHalfSize().z()), lX, lY, lZ, lXRange, lYRange, getOrigin(), lSegmentDir[i], lSegmentRange[0]))
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

/*---------------------------------------------------------------------------------------
  Serialization
---------------------------------------------------------------------------------------*/
Bool BCone::writeToFile(OutStream &os) const
{
    os	<< m_origin
        << m_direction
        << m_depth
        << m_cutoff
        << m_spheric;

	return True;
}
Bool BCone::readFromFile(InStream &is)
{
    is	>> m_origin
        >> m_direction
        >> m_depth
        >> m_cutoff
        >> m_spheric;

	m_upToDate = False;

	return True;
}

Bool BCone::update(UpToDateData what) const
{
	if (isUpToDate(what))
		return True;

	switch(what)
	{
	case OUT_SPHERE:
		m_outSphere = BSphere(m_origin, m_direction, m_cutoff, m_depth, m_spheric, False);
		break;
	case IN_SPHERE:
		m_inSphere = BSphere(m_origin, m_direction, m_cutoff, m_depth, m_spheric, True);
		break;
	case PRECOMPUTED_0_2:
		{
			const Float lT = tan(0.5f*m_cutoff);
			const Float lT2 = o3d::sqr<Float>(lT);
			const Float lInv = 1.0f / (1.0f + lT2);

            m_precomputed[DATA_COS_CUTOFF] = lInv * (1.0f - lT2);
            m_precomputed[DATA_SIN_CUTOFF] = lInv * 2.0f * lT;
            m_precomputed[DATA_TAN_CUTOFF] = m_precomputed[DATA_SIN_CUTOFF] / m_precomputed[DATA_COS_CUTOFF];
		}
		break;
	case PRECOMPUTED_3_8:
		{
			// Si le cone pointe pas trop vers le haut
			if (o3d::abs(m_direction * Vector3(0.0f, 1.0f, 0.0f)) < 0.5f)
			{
				Vector3 lBaseH = Vector3(0.0f, 1.0f, 0.0f) ^ m_direction; // base du plan de la base (vecteur horizontal)
				lBaseH.normalize();
				Vector3 lBaseV = m_direction ^ lBaseH;

				memcpy(&m_precomputed[DATA_VEC_BASEH_X], lBaseH.getData(), 3*sizeof(Float));
				memcpy(&m_precomputed[DATA_VEC_BASEV_X], lBaseV.getData(), 3*sizeof(Float));
			}
			else
			{
                Vector3 lBaseH = Vector3(1.0f, 0.0f, 0.0f) ^ m_direction; // base du plan de la base (vecteur horizontal)
				lBaseH.normalize();
				Vector3 lBaseV = m_direction ^ lBaseH;

				memcpy(&m_precomputed[DATA_VEC_BASEH_X], lBaseH.getData(), 3*sizeof(Float));
				memcpy(&m_precomputed[DATA_VEC_BASEV_X], lBaseV.getData(), 3*sizeof(Float));
			}
		}
		break;
	case PRECOMPUTED_9_11:
		{
			Vector3 lTopPoint(m_origin);
            lTopPoint += m_depth*m_direction;

			memcpy(&m_precomputed[DATA_VEC_TOP_POINT_X], lTopPoint.getData(), 3*sizeof(Float));
		}
		break;
	case PRECOMPUTED_12_15:
		{
			Vector3 lBasePoint(m_origin);
            if (m_spheric)
                lBasePoint += (m_depth*getCosCutoff())*m_direction;
			else
                lBasePoint += m_depth*m_direction;

			memcpy(&m_precomputed[DATA_VEC_BASE_POINT_X], lBasePoint.getData(), 3*sizeof(Float));
		}
		break;
	case STATE:
		{
			m_isValid = True;

			if (m_spheric)
			{
				m_isValid &= (m_cutoff >= 0.0f) && (m_cutoff <= o3d::PI);
				m_isValid &= (o3d::abs<Float>(m_direction.squareLength() - 1.0f) < o3d::Limits<Float>::epsilon());
				m_isValid &= (m_depth >= 0.0f);
			}
            else
			{
				m_isValid &= (m_cutoff >= 0.0f) && (m_cutoff <= o3d::HALF_PI);
				m_isValid &= (o3d::abs<Float>(m_direction.squareLength() - 1.0f) < o3d::Limits<Float>::epsilon());
				m_isValid &= (m_depth >= 0.0f);
			}
		}
		break;
	default:
		O3D_ERROR(E_InvalidOperation("Invalid enum value"));
		return False;
	}

	m_upToDate &= ~what;

	return True;
}


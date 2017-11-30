/**
 * @file bsphere.cpp
 * @brief Bounding Sphere.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2005-08-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/geom/bsphere.h"

#include "o3d/geom/plane.h"
#include "o3d/geom/bcone.h"
#include "o3d/geom/aabbox.h"
#include "o3d/core/matrix4.h"
#include "o3d/core/vector2.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

//-----------------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------------

BSphere::BSphere(
		const Vector3 &refCenter,
		const Vector3 &dir,
		Float cutoff,
		Float depth,
		Bool spheric,
		Bool inside):
	m_center(),
	m_radius()
{
	if (!inside) // Returns the bounding sphere
	{
		if (spheric)
		{
			cutoff = o3d::clamp(cutoff, 0.0f, o3d::PI);

			// Si cutoff > pi/2, le probleme revient a determiner la sphere qui englobe une sphere...
			if (cutoff >= o3d::HALF_PI)
			{
				m_center = refCenter;
				m_radius = depth;
			}
			else
			{
				const Float f = sin(cutoff); 
				const Float e = cos(cutoff); 

				if (f >= e) // tan(cutoff) > 1 <=> cutoff > 45�
				{
					m_radius = depth * f;
					m_center = refCenter + depth * e * dir;
				}
				else
				{
					m_radius = 0.5f * depth / e; // With cutoff in [0;pi/4] => e != 0
					m_center = refCenter + m_radius * dir;
				}
			}
		}
		else
		{
			O3D_ASSERT((cutoff >= 0.0f) && (cutoff < o3d::HALF_PI));
			const Float lTanCutoff = tan(cutoff);

			// Si la demi base est superieure a la profondeur, le rayon de la sphere est la demi base
			if (lTanCutoff > 1.0f)
			{
				m_radius = depth*lTanCutoff;
				m_center = refCenter + depth * dir;
			}
			else // sinon on determine le centre du cercle circonscrit.
			{
				m_radius = 0.5f * depth * (o3d::sqr(lTanCutoff) + 1.0f);
				m_center = refCenter + m_radius * dir;
			}
		}
	}
	else // Returns the sphere included in the cone
	{
		cutoff = o3d::clamp(cutoff, 0.0f, o3d::HALF_PI);

		// Meme calcul que le cone soit spherique ou pas.
		const Float p = depth / (1.0f + sin(cutoff)); // Position de la sphere sur l'axe

		m_radius = p * sin(cutoff);
		m_center = refCenter + p * dir;
	}
}

// Transform
void BSphere::applyTransform(const Matrix4 &mat, BSphere &res) const
{
	Vector3 vScale = mat.getScale();
	Float maxScale;
	res.m_center = mat*m_center;
	
	// Scale the radius according to the matrix
	maxScale = o3d::max(vScale[0], vScale[1]);
	maxScale = o3d::max(maxScale, vScale[2]);

	// result.
	res.m_radius = m_radius * maxScale;
}

//---------------------------------------------------------------------------------------
// Clipping
//---------------------------------------------------------------------------------------

Bool BSphere::clipFront(const Plane &p) const
{
	// assume normalized planes.
	Float d = p*m_center;
    if (d<-m_radius) {
		return False;
    }
	
	return True;
}


Bool BSphere::clipBack(const Plane &p) const
{
	// assume normalized planes.
		Float d = p*m_center;
    if (d>m_radius) {
		return False;
    }

	return True;
}


Bool BSphere::include(const Vector3 &p) const
{
//	Float r2 = (p - m_center).length();
//	return (r2 <= m_radius);
	Float r2 = (p - m_center).squareLength();
	return (r2 <= m_radius * m_radius);
}

Bool BSphere::include(const BSphere &s) const
{
	// if smaller than s, how could we include it???
    if(m_radius <= s.m_radius) {
		return False;
    }

	Float r2 = (s.m_center - m_center).squareLength();
	// Because of prec test, m_radius-s.m_radius>0.
	//Float r2 = (s.m_center - m_center).length();
    //return  (r2 <= Math::sqrt(m_radius-s.m_radius));
	Float radius = m_radius - s.m_radius;
    return (r2 <= radius * radius);
}

//! Does the sphere include TOTALY a box
Bool BSphere::include(const AABBox &b) const
{
	Vector3 lBoxToSphere(m_center - b.getCenter());

    if (lBoxToSphere.x() >= 0.0f) {
        if (lBoxToSphere.y() >= 0.0f) {
            if (lBoxToSphere.z() >= 0.0f) {
				return ((lBoxToSphere += Vector3(-b.getHalfSize().x(), -b.getHalfSize().y(), -b.getHalfSize().z())).squareLength() <= o3d::sqr<Float>(m_radius));
            } else {
				return ((lBoxToSphere += Vector3(-b.getHalfSize().x(), -b.getHalfSize().y(), +b.getHalfSize().z())).squareLength() <= o3d::sqr<Float>(m_radius));
            }
        } else {
            if (lBoxToSphere.z() >= 0.0f) {
				return ((lBoxToSphere += Vector3(-b.getHalfSize().x(), +b.getHalfSize().y(), -b.getHalfSize().z())).squareLength() <= o3d::sqr<Float>(m_radius));
            } else {
				return ((lBoxToSphere += Vector3(-b.getHalfSize().x(), +b.getHalfSize().y(), +b.getHalfSize().z())).squareLength() <= o3d::sqr<Float>(m_radius));
            }
		}
    } else {
        if (lBoxToSphere.y() >= 0.0f) {
            if (lBoxToSphere.z() >= 0.0f) {
				return ((lBoxToSphere += Vector3(+b.getHalfSize().x(), -b.getHalfSize().y(), -b.getHalfSize().z())).squareLength() <= o3d::sqr<Float>(m_radius));
            } else {
				return ((lBoxToSphere += Vector3(+b.getHalfSize().x(), -b.getHalfSize().y(), +b.getHalfSize().z())).squareLength() <= o3d::sqr<Float>(m_radius));
            }
        } else {
            if (lBoxToSphere.z() >= 0.0f) {
				return ((lBoxToSphere += Vector3(+b.getHalfSize().x(), +b.getHalfSize().y(), -b.getHalfSize().z())).squareLength() <= o3d::sqr<Float>(m_radius));
            } else {
				return ((lBoxToSphere += Vector3(+b.getHalfSize().x(), +b.getHalfSize().y(), +b.getHalfSize().z())).squareLength() <= o3d::sqr<Float>(m_radius));
            }
		}
	}
}

Bool BSphere::intersect(const BSphere &s) const
{
//	Float r2 = (s.m_center - m_center).length();
//	return (r2 <= (m_radius+s.m_radius));
	Float r2 = (s.m_center - m_center).squareLength();
	Float radius = m_radius - s.m_radius;
	return (r2 <= radius * radius);
}

Bool BSphere::intersect(const AABBox &b) const
{
	return b.intersect(*this);
}

Bool BSphere::intersect(const Plane &p) const
{
	return (p.distance(m_center) <= m_radius);
}

Bool BSphere::intersect(const BCone &cone) const
{
	return cone.intersect(*this);
}

Geometry::Clipping BSphere::clip(const Plane & plane) const
{
	const Float lDistance = o3d::abs<Float>(plane*m_center);

    if (lDistance > m_radius) {
		return Geometry::CLIP_OUTSIDE;
    } else {
		return Geometry::CLIP_INTERSECT;
    }
}

Geometry::Clipping BSphere::clip(const BSphere & sphere) const
{
	const Float lDist = (sphere.m_center - m_center).length();

    if (lDist > sphere.m_radius + m_radius) {
		return Geometry::CLIP_OUTSIDE;
    } else if (lDist + sphere.m_radius <= m_radius) {
		return Geometry::CLIP_INSIDE;
    } else {
		return Geometry::CLIP_INTERSECT;
    }
}

Geometry::Clipping BSphere::clip(const BCone & cone) const
{
    if (!cone.isValid()) {
		O3D_WARNING("Invalid BCone object");
		return Geometry::CLIP_OUTSIDE;
	}

	Geometry::Clipping lResult;

    if ((lResult = clip(cone.getOutBSphere())) != Geometry::CLIP_INTERSECT) {
		return lResult;
    }

	const Vector3 lCenterToOrigin(cone.getOrigin() - m_center);
	Vector3 lVertical(lCenterToOrigin ^ cone.getDirection());

	const Float lSqrRadius = o3d::sqr<Float>(m_radius);

    if (lVertical.normalize() < o3d::Limits<Float>::epsilon()) {
		lVertical = Vector3(1.0f, 0.0f, 0.0f) ^ cone.getDirection();

        if (lVertical.normalize() < o3d::Limits<Float>::epsilon()) {
			lVertical = Vector3(0.0f, 1.0f, 0.0f) ^ cone.getDirection();
			lVertical.normalize();
		}
	}

	const Vector3 lAxeToCenter(cone.getDirection() ^ lVertical);

    if (cone.isSpheric()) {
		const Vector3 lTopToCenter = m_center - cone.getOrigin() - cone.getDepth()*cone.getDirection(); // L'origine du repere 2D est differente dans le cas non spheric
		const Float lHalfBase = cone.getDepth()*cone.getSinCutoff();

		// Changement de coordonnees. 3D => 2D
		const Vector2f lCoord(o3d::abs<Float>(lTopToCenter * lAxeToCenter), -(lTopToCenter * cone.getDirection()));
		const Vector2f lSqrCoord(o3d::sqr<Float>(lCoord.x()), o3d::sqr<Float>(lCoord.y()));

		const Float lTanCutoff = cone.getTanCutoff();
		const Float lF2 = - lCoord.x() / lTanCutoff; // <=> * tan(o3d::HalfPi - cutoff);
		const Float lF3 = lCoord.x() * lTanCutoff;

		Float lT;

		// Test lorsque le centre du cercle est a l'exterieur du triangle
        if (lCoord.y() <= lF2) {
            // Zone 1
            if (lSqrCoord.x() + lSqrCoord.y() > o3d::sqr<Float>(cone.getDepth() + m_radius)) {
				return Geometry::CLIP_OUTSIDE;
            } else if ((lSqrCoord.x() + lSqrCoord.y() <= lSqrRadius) &&
					(lSqrCoord.x() + o3d::sqr<Float>(lCoord.y() + cone.getDepth()) <= lSqrRadius) &&
                    (o3d::sqr<Float>(lCoord.x() + lHalfBase) + o3d::sqr<Float>(lCoord.y() + cone.getDepth()*cone.getCosCutoff()) <= lSqrRadius)) {
				return Geometry::CLIP_INSIDE;
            } else {
				return Geometry::CLIP_INTERSECT;
            }
        } else if ((cone.getCutoff() < o3d::HALF_PI) && (lCoord.y() >= 0.0f) && (lCoord.y() >= lF3)) {
            // Zone 4
            if (lSqrCoord.x() + lSqrCoord.y() > lSqrRadius) {
				return Geometry::CLIP_OUTSIDE;
            } else if ((lSqrCoord.x() + o3d::sqr<Float>(lCoord.y() + cone.getDepth()) <= lSqrRadius) &&
                    (o3d::sqr<Float>(lCoord.x() + lHalfBase) + o3d::sqr<Float>(lCoord.y() + cone.getDepth()*cone.getCosCutoff()) <= lSqrRadius)) {
				return Geometry::CLIP_INSIDE;
            } else {
				return Geometry::CLIP_INTERSECT;
            }
        } else if ((lCoord.y() >= lF2) && ((lT = (lCoord.x() - lHalfBase)*cone.getSinCutoff() + (lCoord.y() + cone.getDepth()*cone.getCosCutoff())*(-cone.getCosCutoff())) <= 0.0f)) {
            // Zone 3
            if (lCoord.y() > lF2 + m_radius/cone.getSinCutoff()) {
				return Geometry::CLIP_OUTSIDE;
            } else if (Math::sqrt(lSqrCoord.x() + lSqrCoord.y()) + cone.getDepth() <= m_radius) {
                // point symetrique par rapport a l'origine du cone (le plus loin)
				return Geometry::CLIP_INSIDE;
            } else {
				return Geometry::CLIP_INTERSECT;
            }
        } else if ((lT = (lCoord.x() - lHalfBase)*cone.getSinCutoff() + (lCoord.y() + cone.getDepth()*cone.getCosCutoff())*(-cone.getCosCutoff())) >= 0.0f) {
            // Calcul de la distance par rapport a la droite f1
            // Zone 2
            if (lT > m_radius) {
				return Geometry::CLIP_OUTSIDE;
            } else if (o3d::sqr<Float>(lCoord.x() - lHalfBase) + o3d::sqr<Float>(lCoord.y() + cone.getDepth()*cone.getCosCutoff()) > lSqrRadius) {
				return Geometry::CLIP_OUTSIDE;
            } else if ((cone.getCutoff() >= o3d::HALF_PI) && (Math::sqrt(lSqrCoord.x() + lSqrCoord.y()) + cone.getDepth() <= m_radius)) {
                // Distance avec le point le plus loin du cone
				return Geometry::CLIP_INSIDE;
            } else if ((cone.getCutoff() < o3d::HALF_PI) && (o3d::sqr<Float>(lCoord.x() + lHalfBase) + o3d::sqr<Float>(lCoord.y() + cone.getDepth()*cone.getCosCutoff()) <= lSqrRadius) &&
                (lSqrCoord.x() + lSqrCoord.y() <= lSqrRadius)) {
				return Geometry::CLIP_INSIDE;
            } else {
				return Geometry::CLIP_INTERSECT;
            }
		}

		// Sinon le centre du cercle est dans le cone
		O3D_ASSERT((lCoord.y() <= lF2) && (lSqrCoord.x() + lSqrCoord.y() <= lSqrRadius));

		// On test si la sphere englobe le tout cad avec chaque point
        if (cone.getCutoff() >= o3d::HALF_PI) {
            if (Math::sqrt(lSqrCoord.x() + lSqrCoord.y()) + cone.getDepth() <= m_radius) {
				return Geometry::CLIP_INSIDE;
            } else {
				return Geometry::CLIP_INTERSECT;
            }
        } else {
			if ((o3d::sqr<Float>(lCoord.x() + lHalfBase) + o3d::sqr<Float>(lCoord.y() + cone.getDepth()*cone.getCosCutoff()) <= lSqrRadius) &&
                (lSqrCoord.x() + lSqrCoord.y() >= lSqrRadius)) {
				return Geometry::CLIP_INSIDE;
            } else {
				return Geometry::CLIP_INTERSECT;
            }
		}
    } else {
		const Vector3 lTopToCenter = m_center - cone.getOrigin() - cone.getDepth()*cone.getDirection();
		const Float lHalfBase = cone.getDepth() * cone.getTanCutoff();

		// Changement de coordonnees. 3D => 2D
		const Vector2f lCoord(o3d::abs<Float>(lTopToCenter * lAxeToCenter), - (lTopToCenter * cone.getDirection()));
		const Vector2f lSqrCoord(o3d::sqr<Float>(lCoord.x()), o3d::sqr<Float>(lCoord.y()));

		const Float lTanCutoff = cone.getTanCutoff();
		const Float lF1 = (lCoord.x() - lHalfBase) * lTanCutoff;
		const Float lF2 = cone.getDepth() - lCoord.x() / lTanCutoff; // <=> * tan(o3d::HalfPi - cutoff);
		const Float lF3 = cone.getDepth() + lCoord.x() * lTanCutoff;

		// Test lorsque le centre du cercle est a l'exterieur du triangle
        if ((lCoord.y() <= 0.0f) && (lCoord.x() <= lHalfBase)) {
            // Zone 1
            if (lCoord.y() < -m_radius) {
				return Geometry::CLIP_OUTSIDE;
            } else if ((o3d::sqr<Float>(lCoord.x() + lHalfBase) + o3d::sqr<Float>(lCoord.y()) <= lSqrRadius) &&
                    (lSqrCoord.x() + o3d::sqr<Float>(lCoord.y() - cone.getDepth()) <= lSqrRadius)) {
				return Geometry::CLIP_INSIDE;
            } else {
				return Geometry::CLIP_INTERSECT;
            }
        } else if ((lCoord.x() >= lHalfBase) && (lCoord.y() <= lF1)) {
            // Zone 2
            if (o3d::sqr<Float>(lCoord.x() - lHalfBase) + lSqrCoord.y() > lSqrRadius) {
				return Geometry::CLIP_OUTSIDE;
            } else if ((o3d::sqr<Float>(lCoord.x() + lHalfBase) + lSqrCoord.y() <= lSqrRadius) &&
                    (lSqrCoord.x() + o3d::sqr<Float>(lCoord.y() - cone.getDepth()) <= lSqrRadius)) {
				return Geometry::CLIP_INSIDE;
            } else {
				return Geometry::CLIP_INTERSECT;
            }
        } else if (lCoord.y() >= lF3) {
            // Zone 4
            if (lSqrCoord.x() + o3d::sqr<Float>(lCoord.y() - cone.getDepth()) > lSqrRadius) {
				return Geometry::CLIP_OUTSIDE;
            } else if (o3d::sqr<Float>(lCoord.x() + lHalfBase) + lSqrCoord.y() <= lSqrRadius) {
				return Geometry::CLIP_INSIDE;
            } else {
				return Geometry::CLIP_INTERSECT;
            }
        } else if (lCoord.y() >= lF2) {
            // Zone 3
            if (lCoord.y() > lF2 + m_radius/cone.getSinCutoff()) {
                return Geometry::CLIP_OUTSIDE;
            } else if ((o3d::sqr<Float>(lCoord.x() + lHalfBase) + o3d::sqr<Float>(lCoord.y()) <= lSqrRadius) &&
                    (lSqrCoord.x() + o3d::sqr<Float>(lCoord.y() - cone.getDepth()) <= lSqrRadius)) {
				return Geometry::CLIP_INSIDE;
            } else {
				return Geometry::CLIP_INTERSECT;
            }
		}

		O3D_ASSERT((lCoord.y() <= cone.getDepth()) && (lCoord.y() >= 0.0f) && (lCoord.x() <= lHalfBase));

		// On test si la sphere englobe le tout cad avec chaque point
		const Float lSqrDist1 = o3d::sqr<Float>(lCoord.x() + lHalfBase) + lSqrCoord.y();
		const Float lSqrDist2 = lSqrCoord.x() + o3d::sqr<Float>(cone.getDepth() - lCoord.y());

        if ((lSqrDist1 <= lSqrRadius) && (lSqrDist2 <= lSqrRadius)) {
			return Geometry::CLIP_INSIDE;
        } else {
			return Geometry::CLIP_INTERSECT;
        }
	}
}

void BSphere::setUnion(const BSphere &sa, const BSphere &sb)
{
    Float r2 = (sb.m_center-sa.m_center).length();
	
	// Name Sphere 0 the biggest one, and Sphere 1 the other
    const BSphere *s0;
    const BSphere *s1;

    if(sa.m_radius>sb.m_radius) {
		s0= &sa;
		s1= &sb;
    } else {
		s0= &sb;
		s1= &sa;
	}
    Float r0= s0->m_radius;
    Float r1= s1->m_radius;
	
	// If Sphere1 is included into Sphere0, then the union is simply Sphere0
    if (r2<=(r0-r1)) {
		*this= *s0;
    } else {
		/* Compute the Union sphere Diameter. It is D= r0 + r2 + r1 
		do the draw, works for intersect and don't intersect case, 
		acknowledge that Sphere1 not included inton Sphere0
		*/
        Float diameter= r0 + r2 + r1;
		
		// compute dir from big m_center to small one.
		Vector3	dir= s1->m_center - s0->m_center;
		dir.normalize();
		
		// Then finally set m_radius and m_center
		m_center= s0->m_center + dir * (diameter/2 - r0);
		m_radius= diameter/2;
	}
}

/*---------------------------------------------------------------------------------------
  Serialization
---------------------------------------------------------------------------------------*/
Bool BSphere::writeToFile(OutStream &os) const
{
    os	<< m_center
        << m_radius;

	return True;
}
Bool BSphere::readFromFile(InStream &is)
{
    is	>> m_center
        >> m_radius;

	return True;
}

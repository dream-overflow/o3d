/**
 * @file plane.cpp
 * @brief 3D Plane.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2005-08-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/geom/plane.h"

#include "o3d/geom/bsphere.h"
#include "o3d/geom/bcone.h"
#include "o3d/core/debug.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

using namespace o3d;

//-----------------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------------

//! Default constructor.
Plane::Plane()
{
}

//! Construct from a normal and position.
Plane::Plane(const Vector3 &normal, const Vector3 &position)
{
	make(normal, position);
}

//! Construct with a vector normal representing a,b,c and a float d.
Plane::Plane(const Vector3 &refNormal, Float d) :
	m_normal(refNormal),
	m_d(d)
{}

//! Construct with a 4 float representing a,b,c and d.
Plane::Plane(Float a, Float b, Float c, Float d):
	m_normal(a,b,c),
	m_d(d)
{}

//! Copy from another O3DPlane.
Plane::Plane(const Plane &refP) :
	m_normal(refP.m_normal),
	m_d(refP.m_d)
{}

//-----------------------------------------------------------------------------------
// Plane construction method / General parameters
//-----------------------------------------------------------------------------------

//! Make a plane with a normal and a vertex.
//! The normal is normalized by Make().
void Plane::make(const Vector3 &normal, const Vector3 &pos)
{
	m_normal = normal;
	m_normal.normalize();

	m_d = -(m_normal*pos);		// d = -(ax+by+cz).
}

//! Make a plane with 3 vertices.
//! @note the plane normal is normalized by Make().
void Plane::make(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2)
{
	m_normal = (p1-p0)^(p2-p1);
	m_normal.normalize();
	m_d = -(m_normal*p1);		// d = -(ax+by+cz).
}

//! Return the normal vector of the plane.
//! Since the normal of the plane may not be normalized (if setup without make()), the returned normal.
//! may NOT be normalized.
const Vector3& Plane::getNormal() const
{
	return m_normal;
}

//! Returns a point from the plane
const Vector3 Plane::getPoint() const
{
    return (-m_d)*m_normal;
}

//! Normalize the plane, such that getNormal() return a normalized vector.
void Plane::normalize()
{
	Float len = m_normal.length();

	if (len > o3d::Limits<Float>::epsilon())
	{
		len = 1.0f / len;
		m_normal *= len;
		m_d *= len;
	}
}

//! Return the normalized version of a plane. @see Normalize().
Plane Plane::normed() const
{
	Plane normalizedPlane = *this;
	normalizedPlane.normalize();
	return normalizedPlane;
}

//! Get the inverted version of this plane (same position, but inverted normal).
Plane Plane::inverted() const
{
	return Plane(-m_normal, -m_d);
}

//! Invert this plane (same position, but inverted normal).
void Plane::invert()
{
	m_normal = -m_normal;
	m_d = -m_d;
}

//-----------------------------------------------------------------------------------
// Projections / Clipping
//-----------------------------------------------------------------------------------

//! Return the distance of p from the plane. Hence, the result is >=0.
//! since the plane normal may not be normalized, distance() compute the distance with the normalized normal
//! of plane. If you are sure that your plane has a normalized normal, it is much faster to do a \c fabs(p*v).
Float Plane::distance(const Vector3 &p) const
{
	Plane plane = normed();
	return (Float)fabs(plane*p);
}

//! Intersect a line onto a plane. p1 is returned if line // to plane.
Vector3 Plane::intersect(const Vector3 &P1,const Vector3 &P2) const
{
	if (P2-P1 == 0)
		return P1;

	return (P1 + (P2-P1) * (((m_normal*m_d)-P1) * m_normal) / ((P2-P1) * m_normal));
}

//! Intersect a line onto a plane. p1 is returned if line // to plane.
Vector3 Plane::intersect(const Vector3 &P1, const Vector3 &P2, Float ray) const
{
	if (P2-P1 == 0)
		return P1;

	return (P1 + (P2-P1) * (((m_normal*(m_d+ray))-P1) * m_normal) / ((P2-P1) * m_normal));
}

//! Intersects with a sphere
Bool Plane::intersect(const BSphere & sphere) const
{
	return (operator*(sphere.getCenter()) <= sphere.getRadius());
}

//! Intersects with a cone
Bool Plane::intersect(const BCone & cone) const
{
	// TODO: optimize
	return (clip(cone) != Geometry::CLIP_OUTSIDE);
}

//! Project a point onto a plane.
Vector3 Plane::project(const Vector3 &p0) const
{
	return intersect(p0, p0+m_normal);
}

/*-----------------------------------------------------------------------------------
  Projections / Clipping
-----------------------------------------------------------------------------------*/
Bool Plane::clipSegmentBack(Vector3 &p0, Vector3 &p1) const
{
	Float	d0,d1,decal;
	Vector3	proj;
	
	d0 = (*this)*p0;
	d1 = (*this)*p1;
	if (d0<0 && d1<0)
		return True;
	if (d0>=0 && d1>=0)
		return False;
	// Clip line.
	decal = (0-d0) / (d1-d0);
	proj = p0+ (p1-p0)*decal;
	if (d0>=0)
		p0=proj;
	else
		p1=proj;
	return True;
}

Bool Plane::clipSegmentFront(Vector3 &p0, Vector3 &p1) const
{
	Float	d0,d1,decal;
	Vector3	proj;
	
	d0 = (*this)*p0;
	d1 = (*this)*p1;
	if (d0>=0 && d1>=0)
		return True;
	if (d0<0 && d1<0)
		return False;
	// Clip line.
	decal= (0-d0) / (d1-d0);
	proj= p0+ (p1-p0)*decal;
	if (d0<0)
		p0=proj;
	else
		p1=proj;
	return True;
}

Int32 Plane::clipPolygonBack(const Vector3 *inPoly, Vector3 *outPoly, Int32 numVertices) const
{
	Int32 nOut=0,s,p,i;
	if(numVertices<=2) return 0;
	
	s=numVertices-1;
	
	for (i=0;i<numVertices;i++)
	{
		p=i;
		if ( (*this)*inPoly[p] < 0 )
		{
			if ( (*this)*inPoly[s] >= 0 ) 
				outPoly[nOut++]= intersect(inPoly[s],inPoly[p]);
			outPoly[nOut++]=inPoly[p];
		}
		else
		{
			if ( (*this)*inPoly[s] < 0 ) 
				outPoly[nOut++]= intersect(inPoly[s],inPoly[p]);
		}
		s=p;
	}
	
	return nOut;
}


Int32 Plane::clipPolygonFront(const Vector3 *inPoly, Vector3 *outPoly, Int32 numVertices) const
{
	Int32 nOut=0,s,p,i;
	if(numVertices<=2) return 0;
	
	s=numVertices-1;
	
	for (i=0;i<numVertices;i++)
	{
		p=i;
		if ( (*this)*inPoly[p] > 0 )
		{
			if ( (*this)*inPoly[s] <= 0 ) 
				outPoly[nOut++]= intersect(inPoly[s],inPoly[p]);
			outPoly[nOut++]=inPoly[p];
		}
		else
		{
			if ( (*this)*inPoly[s] > 0 ) 
				outPoly[nOut++]= intersect(inPoly[s],inPoly[p]);
		}
		s=p;
	}
	
	return nOut;
}

Geometry::Clipping Plane::clip(const BCone & cone) const
{
	if (!cone.isValid())
	{
		O3D_WARNING("Invalid BCone object");
		return Geometry::CLIP_OUTSIDE;
	}

    if (clip(cone.getOutBSphere()) == Geometry::CLIP_OUTSIDE)
        return Geometry::CLIP_OUTSIDE;

    Vector3 lPlaneToBase(((cone.getOrigin() + cone.getDepth()*cone.getDirection() - getPoint())*m_normal)*m_normal);
	lPlaneToBase.normalize();

    Vector3 lDir(cone.getDirection() ^ (cone.getDirection() ^ lPlaneToBase)); // Vecteur de la base qui va le plus "vite" vers le plan

	if (lDir.normalize() < o3d::Limits<Float>::epsilon())
	{
		lDir = Vector3(1.0f, 0.0f, 0.0f) ^ cone.getDirection();

		if (lDir.normalize() < o3d::Limits<Float>::epsilon())
		{
			lDir = Vector3(0.0f, 1.0f, 0.0f) ^ cone.getDirection();
			lDir.normalize();
		}
	}

    if (!cone.isSpheric())
	{
		// Point de la base le plus proche du plan
        const Vector3 lNearestPoint = cone.getOrigin() + cone.getDepth() * (cone.getDirection() + cone.getTanCutoff() * lDir);
		const Float lDist1 = operator *(lNearestPoint);
		const Float lDist2 = operator *(cone.getOrigin());

		if ((lDist1 > 0.0f)  && (lDist2 > 0.0f))
			return Geometry::CLIP_OUTSIDE;
		else if ((lDist1 < 0.0f) && (lDist2 < 0.0f))
			return Geometry::CLIP_INSIDE;
		else
			return Geometry::CLIP_INTERSECT;
	}
	else
	{
		// On sait deja que le plan intersecte la sphere de rayon "depth"

        const Vector3 lTopPoint = cone.getOrigin() + cone.getDepth() * cone.getDirection();

		// Point le plus pres dans la partie spherique
		Vector3 lNearestPoint = ((getPoint() - cone.getOrigin())*m_normal) * m_normal;

		// Point de la base le plus proche du plan
		if (cone.doesDirectionLieIn(lNearestPoint))
		{
			if (lNearestPoint.normalize() > o3d::Limits<Float>::epsilon())
                (lNearestPoint = cone.getOrigin()) += cone.getDepth()*lNearestPoint; // En realite, ce n'est pas toujours le point le plus pres
			else // Means the base center is on the plane
				return Geometry::CLIP_INTERSECT;
		}
		else
            (lNearestPoint = cone.getOrigin()) += cone.getDepth()*(cone.getCosCutoff()*cone.getDirection() + cone.getSinCutoff()*lDir);

        const Float lDist1 = operator*(lNearestPoint);
        const Float lDist2 = operator*(cone.getOrigin());
        const Float lDist3 = operator*(lTopPoint);

        if ((lDist1 > 0.0f) && (lDist2 > 0.0f) && (lDist3 > 0.0f))
            return Geometry::CLIP_OUTSIDE;
        else if ((lDist1 < 0.0f) && (lDist2 < 0.0f) && (lDist3 < 0.0f))
            return Geometry::CLIP_INSIDE;
        else
            return Geometry::CLIP_INTERSECT;
	}
}

Geometry::Clipping Plane::clip(const BSphere & sphere) const
{
	const Float lDistance = operator*(sphere.getCenter());

	if (lDistance > sphere.getRadius())
		return Geometry::CLIP_OUTSIDE;
	else if (lDistance < -sphere.getRadius())
		return Geometry::CLIP_INSIDE;
	else
		return Geometry::CLIP_INTERSECT;
}

// Serialization
Bool Plane::writeToFile(OutStream &os) const
{
    os	<< m_normal
        << m_d;

	return True;
}
Bool Plane::readFromFile(InStream &is)
{
    is	>> m_normal
        >> m_d;

	return True;
}


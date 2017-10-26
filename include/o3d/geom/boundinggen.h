/**
 * @file boundinggen.h
 * @brief Bounding volume generation.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2010-01-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BOUNDINGGEN_H
#define _O3D_BOUNDINGGEN_H

#include "o3d/core/math.h"
#include "o3d/core/vector3.h"
#include "o3d/core/matrix3.h"

namespace o3d {

class AABBoxExt;
class BSphere;
class AABBox;
class OBBox;

//---------------------------------------------------------------------------------------
//! @class BoundingGen
//-------------------------------------------------------------------------------------
//! Bounding volume generation.
//---------------------------------------------------------------------------------------
class O3D_API BoundingGen
{
public:

	//! Default constructor.
	BoundingGen();

	//! Update the bounding computation (first pass) for each vertex.
	inline void firstPass(Float x, Float y, Float z)
	{
		Vector3 v(x, y, z);

		m_average += v;

		m_minV.minOf(m_minV, v);
		m_maxV.maxOf(m_maxV, v);
	}

	//! Update the bounding computation (first pass) for each vertex.
	inline void firstPass(const Vector3 &v)
	{
		m_average += v;

		m_minV.minOf(m_minV, v);
		m_maxV.maxOf(m_maxV, v);
	}

	//! All vertices have been checked so we finalize the first pass.
	//! Axis-aligned bounding box can be asked after this first flush.
	void flushFirstPass(UInt32 vertices);

	//! Make the second pass to compute R,S,T directions vector.
	inline void secondPass(Float x, Float y, Float z)
	{
		// C = 1/N sum(i=1 to N){ (Pi - m)(Pi - m)T }
		// (where P is the point v, m the average, and T mean transpose)
		Float xi_minus_mx = x - m_average.x();
		Float yi_minus_my = y - m_average.y();
		Float zi_minus_mz = z - m_average.z();

		m_covariance(0,0) = m_covariance(0,0) + (xi_minus_mx * xi_minus_mx);
		m_covariance(1,1) = m_covariance(1,1) + (yi_minus_my * yi_minus_my);
		m_covariance(2,2) = m_covariance(2,2) + (zi_minus_mz * zi_minus_mz);

		m_covariance(0,1) = m_covariance(0,1) + (xi_minus_mx * yi_minus_my);
		m_covariance(0,2) = m_covariance(0,2) + (xi_minus_mx * zi_minus_mz);
		m_covariance(1,2) = m_covariance(1,2) + (yi_minus_my * zi_minus_mz);
	}

	//! Make the second pass to compute R,S,T directions vector.
	inline void secondPass(const Vector3 &v)
	{
		// C = 1/N sum(i=1 to N){ (Pi - m)(Pi - m)T }
		// (where P is the point v, m the average, and T mean transpose)
		Float xi_minus_mx = v.x() - m_average.x();
		Float yi_minus_my = v.y() - m_average.y();
		Float zi_minus_mz = v.z() - m_average.z();

		m_covariance(0,0) = m_covariance(0,0) + (xi_minus_mx * xi_minus_mx);
		m_covariance(1,1) = m_covariance(1,1) + (yi_minus_my * yi_minus_my);
		m_covariance(2,2) = m_covariance(2,2) + (zi_minus_mz * zi_minus_mz);

		m_covariance(0,1) = m_covariance(0,1) + (xi_minus_mx * yi_minus_my);
		m_covariance(0,2) = m_covariance(0,2) + (xi_minus_mx * zi_minus_mz);
		m_covariance(1,2) = m_covariance(1,2) + (yi_minus_my * zi_minus_mz);
	}

	//! Compute the covariance matrix.
	void flushSecondPass(UInt32 vertices);

	//! Update the bounding computation (third pass) for each vertex.
	inline void thirdPass(Float x, Float y, Float z)
	{
		Vector3 v(x,y,z);

		// R
		Float dot = v * m_R;

		if (dot < m_minDotR)
			m_minVertexR = v;

		if (dot > m_maxDotR)
			m_maxVertexR = v;

		m_minDotR = o3d::min(dot,m_minDotR);
		m_maxDotR = o3d::max(dot,m_maxDotR);

		// S
		dot = v * m_S;
		m_minDotS = o3d::min(dot,m_minDotS);
		m_maxDotS = o3d::max(dot,m_maxDotS);

		// T
		dot = v * m_T;
		m_minDotT = o3d::min(dot,m_minDotT);
		m_maxDotT = o3d::max(dot,m_maxDotT);
	}

	//! Update the bounding computation (third pass) for each vertex.
	inline void thirdPass(const Vector3 &v)
	{
		// R
		Float dot = v * m_R;

		if (dot < m_minDotR)
			m_minVertexR = v;

		if (dot > m_maxDotR)
			m_maxVertexR = v;

		m_minDotR = o3d::min(dot,m_minDotR);
		m_maxDotR = o3d::max(dot,m_maxDotR);

		// S
		dot = v * m_S;
		m_minDotS = o3d::min(dot,m_minDotS);
		m_maxDotS = o3d::max(dot,m_maxDotS);

		// T
		dot = v * m_T;
		m_minDotT = o3d::min(dot,m_minDotT);
		m_maxDotT = o3d::max(dot,m_maxDotT);
	}

	//! Compute some variables.
	//! Oriented bounding box can be asked after this third pass.
	void flushThirdPass();

	//! update the bounding computation (fourth pass) for each vertex (used for extend the bsphere)
	inline void fourthPass(Float x, Float y, Float z)
	{
		Vector3 v(x,y,z);
		Vector3 dist = v - m_bSphereCenter;

		// do we need to expand the sphere ?
		if (dist.squareLength() > m_bSphereRadiusSquare)
		{
			// the new sphere is tangent to the previous sphere at point G
			Vector3 G = m_bSphereCenter - (dist / dist.length()) * m_bSphereRadius;

			// the new center is halfway between G and v
			m_bSphereCenter = (G + v) * 0.5f;

			// the new radius is this length
			m_bSphereRadius = (v - m_bSphereCenter).length();
			m_bSphereRadiusSquare = m_bSphereRadius * m_bSphereRadius;
		}
	}

	//! update the bounding computation (fourth pass) for each vertex (used for extend the bsphere)
	inline void fourthPass(const Vector3 &v)
	{
		Vector3 dist = v - m_bSphereCenter;

		// do we need to expand the sphere ?
		if (dist.squareLength() > m_bSphereRadiusSquare)
		{
			// the new sphere is tangent to the previous sphere at point G
			Vector3 G = m_bSphereCenter - (dist / dist.length()) * m_bSphereRadius;

			// the new center is halfway between G and v
			m_bSphereCenter = (G + v) * 0.5f;

			// the new radius is this length
			m_bSphereRadius = (v - m_bSphereCenter).length();
			m_bSphereRadiusSquare = m_bSphereRadius * m_bSphereRadius;
		}
	}

	//! Set a bounding sphere.
	void getBoundingSphere(class BSphere &bSphere);

	//! Set an oriented bounding box.
	void getOrientedBoundingBox(class OBBox &bBox);

	//! Set an axis aligned bounding box.
	void getAxisAlignedBoundingBox(class AABBox &bBox);

	//! Set an axis aligned extended bounding box.
	void getAxisAlignedBoundingBoxExt(class AABBoxExt &bBoxExt);

	//! Get the covariante matrix.
	inline const Matrix3& getCovarianceMatrix() const { return m_covariance; }

private:

	Vector3 m_minV, m_maxV;     //!< min and max on x,y,z.

	Vector3 m_average;          //!< vertices average
	Matrix3 m_covariance;       //!< covariance matrix
	Vector3 m_R,m_S,m_T;        //!< r,s,t eigenvectors from covariante matrix

	Vector3 m_orientedBoxCenter; //!< computed bounding box center

	Vector3 m_bSphereCenter;       //!< computed bounding sphere center
	Float  m_bSphereRadius;       //!< computed and centered radius
	Float  m_bSphereRadiusSquare; //!< r*r

	Vector3 m_minVertexR, m_maxVertexR; //!< minimal and maximal X,Y,Z components from R for bsphere

	Float m_minDotR,m_maxDotR; //!< minimal and maximal dot product from V to R
	Float m_minDotS,m_maxDotS; //!< minimal and maximal dot product from V to S
	Float m_minDotT,m_maxDotT; //!< minimal and maximal dot product from V to T
};

} // namespace o3d

#endif // _O3D_BOUNDINGGEN_H


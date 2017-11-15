/**
 * @file boundinggen.cpp
 * @brief Implementation of BoundingGen.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-01-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/geom/boundinggen.h"

#include "o3d/geom/bsphere.h"
#include "o3d/geom/aabbox.h"
#include "o3d/geom/obbox.h"

using namespace o3d;

// Default ctor
BoundingGen::BoundingGen() :
	m_minV(o3d::Limits<Float>::max(), o3d::Limits<Float>::max(), o3d::Limits<Float>::max()),
	m_maxV(o3d::Limits<Float>::min(), o3d::Limits<Float>::min(), o3d::Limits<Float>::min()),
	m_bSphereRadius(0.f)
{
}

// all vertices have been checked so we finish the computation
void BoundingGen::flushFirstPass(UInt32 vertices)
{
	Float invNumVertices = 1.f / vertices;

	// make the average
	m_average.x() *= invNumVertices;
	m_average.y() *= invNumVertices;
	m_average.z() *= invNumVertices;

	m_covariance.zero();
}

void BoundingGen::flushSecondPass(UInt32 vertices)
{
	Float invNumVertices = 1.f / vertices;

	// make the average
	m_covariance(0,0) = m_covariance(0,0) * invNumVertices;
	m_covariance(1,1) = m_covariance(1,1) * invNumVertices;
	m_covariance(2,2) = m_covariance(2,2) * invNumVertices;

	m_covariance(0,1) = m_covariance(0,1) * invNumVertices;
	m_covariance(0,2) = m_covariance(0,2) * invNumVertices;
	m_covariance(1,2) = m_covariance(1,2) * invNumVertices;

	// and by symmetry we have ...
	m_covariance(1,0) = m_covariance(0,1);
	m_covariance(2,0) = m_covariance(0,2);
	m_covariance(2,1) = m_covariance(1,2);

	// find the three eigen-vectors
	Matrix3 eigenVectors;
	Vector3 eigenValues;

	m_covariance.computeSymmetricEigenSystem(eigenVectors, eigenValues);

	m_R = eigenVectors.getX();
	m_S = eigenVectors.getY();
	m_T = eigenVectors.getZ();
	/*
	System::print(m_covariance, "");
	System::print(eigenVectors,"");
	System::print(String::print("%f %f %f\n", eigenValues[0], eigenValues[1], eigenValues[2]),"");
	*/
	// reset min and max dot
	m_minDotR = m_minDotS = m_minDotT = o3d::Limits<Float>::max();
	m_maxDotR = m_maxDotS = m_maxDotT = o3d::Limits<Float>::min();
}

// Compute some variables
void BoundingGen::flushThirdPass()
{
	// the center and radius of the sphere
	m_bSphereCenter = (m_minVertexR + m_maxVertexR) * 0.5f;
	m_bSphereRadius = (m_minVertexR - m_bSphereCenter).length();

	m_bSphereRadiusSquare = m_bSphereRadius * m_bSphereRadius;

	// the center of the box
	Float a,b,c;

	a = (m_minDotR + m_maxDotR) * 0.5f;
	b = (m_minDotS + m_maxDotS) * 0.5f;
	c = (m_minDotT + m_maxDotT) * 0.5f;

	m_orientedBoxCenter = m_R*a + m_S*b + m_T*c;

	// the six planes of the box
	/*Float xMin, xMax;
	Float yMin, yMax;
	Float zMin, zMax;

	xMin = -m_minDotR;
	yMin = -m_minDotS;
	zMin = -m_minDotT;

	xMax = m_maxDotR;
	yMax = m_maxDotS;
	zMax = m_maxDotT;

	System::print(String::print("xmin max %f %f", xMin, xMax));
	System::print(String::print("ymin max %f %f", yMin, yMax));
	System::print(String::print("zmin max %f %f", zMin, zMax));

	System::print(m_R, "R");
	System::print(m_S, "S");
	System::print(m_T, "T");*/
}

// Set a bounding sphere
void BoundingGen::getBoundingSphere(BSphere& bSphere)
{
	bSphere.setCenter(m_bSphereCenter);
	bSphere.setRadius(m_bSphereRadius);
}

// Set an oriented bounding box.
void BoundingGen::getOrientedBoundingBox(class OBBox &bBox)
{
	bBox.setData(m_covariance);
	bBox.setCenter(m_orientedBoxCenter);
}

// Set a bounding box
void BoundingGen::getAxisAlignedBoundingBox(AABBox& bBox)
{
	bBox.setMinMax(m_minV, m_maxV);
}

// Set an extended bounding box
void BoundingGen::getAxisAlignedBoundingBoxExt(AABBoxExt& bBoxExt)
{
	bBoxExt.setMinMax(m_minV, m_maxV);
/*
	BoundingGen test;
	test.firstPass(-1,-2,1);
	test.firstPass(1,0,2);
	test.firstPass(2,-1,3);
	test.firstPass(2,-1,2);
	test.flushFirstPass(4);

	test.secondPass(-1,-2,1);
	test.secondPass(1,0,2);
	test.secondPass(2,-1,3);
	test.secondPass(2,-1,2);
	test.flushSecondPass(4);

	test.thirdPass(-1,-2,1);
	test.thirdPass(1,0,2);
	test.thirdPass(2,-1,3);
	test.thirdPass(2,-1,2);
	test.flushThirdPass();

	test.fourthPass(-1,-2,1);
	test.fourthPass(1,0,2);
	test.fourthPass(2,-1,3);
	test.fourthPass(2,-1,2);

	AABBox bbox;
	test.getAxisAlignedBoundingBox(bbox);

	System::print(bbox.getHalfSize(), "");
	System::print(bbox.getCenter(), "");*/
}


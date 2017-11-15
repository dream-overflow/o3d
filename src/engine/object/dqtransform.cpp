/**
 * @file dqtransform.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/dqtransform.h"

#include "o3d/core/file.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(DQTransform, ENGINE_DQTRANSFORM, Transform)

//! set to identity the relative matrix
void DQTransform::identity()
{
	// the matrix
	m_matrix4.identity();

	// and its componements
	m_PositionRotation.identity();
	m_Scale.set(1.f,1.f,1.f);

	// Matrix is updated
	m_isDirty = False;
}

// Identity the transform
Bool DQTransform::isIdentity() const
{
	return (m_PositionRotation.isIdentity() &&
			(m_Scale[X] == 1.f) && (m_Scale[Y] == 1.f) && (m_Scale[Z] == 1.f));
}

// set the relative matrix. Used to set the transformation from a matrix.
void DQTransform::setMatrix(const Matrix4& M)
{
	m_matrix4 = M;

	// get the datas
	m_PositionRotation.fromMatrix4(m_matrix4);
	m_Scale = m_matrix4.getScale();

	setClean();
	m_hasUpdated = True;
}

// rotate the quaternion
void DQTransform::rotate(UInt32 axis, Float alpha)
{
	Quaternion q;

	switch (axis)
	{
		case X:
			q.fromAxisAngle3(Vector3(1.f,0.f,0.f),alpha);
			m_PositionRotation *= q;
			break;

		case Y:
			q.fromAxisAngle3(Vector3(0.f,1.f,0.f),alpha);
			m_PositionRotation *= q;
			break;

		case Z:
			q.fromAxisAngle3(Vector3(0.f,0.f,1.f),alpha);
			m_PositionRotation *= q;
			break;

		default:
			break;
	}

	m_PositionRotation.normalize();
	setDirty();
}

// define the rotation
void DQTransform::setRotation(const Quaternion &Quat)
{
	setDirty();
}

// update the matrix value
Bool DQTransform::update()
{
	if (isDirty())
	{
		m_PositionRotation.normalize();

		m_PositionRotation.toMatrix4(m_matrix4);
		m_matrix4.scale(m_Scale);

		setClean();

		m_hasUpdated = True;
		return True;
	}
	else
	{
		return False;
	}
}

// serialisation
Bool DQTransform::writeToFile(OutStream &os)
{
    Transform::writeToFile(os);

/*	os   << m_PositionRotation
		 << m_Scale;
*/
	return True;
}

Bool DQTransform::readFromFile(InStream &is)
{
    Transform::readFromFile(is);

/*	is   >> m_PositionRotation
		 >> m_Scale;
*/
	update();
	return True;
}


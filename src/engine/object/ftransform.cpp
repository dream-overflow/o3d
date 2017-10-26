/**
 * @file ftransform.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/ftransform.h"

#include "o3d/core/file.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(FTransform, ENGINE_FTRANSFORM, Transform)

UInt32 FTransform::getType() const
{
    return ENGINE_FTRANSFORM;
}

// Set to identity the relative matrix
void FTransform::identity()
{
    // the matrix
    m_matrix4.identity();

	// and its components
	m_position.set();
	m_rotation.set();
	m_scale.set(1.f,1.f,1.f);

	// Matrix is updated
	m_isDirty = False;
	m_hasUpdated = True;
}

// Is identity
Bool FTransform::isIdentity() const
{
	return (m_position[X] == 0.f && m_position[Y] == 0.f && m_position[Z] == 0.f &&
			m_rotation[X] == 0.f && m_rotation[Y] == 0.f && m_rotation[Z] == 0.f &&
			   m_scale[X] == 1.f &&    m_scale[Y] == 1.f &&    m_scale[Z] == 1.f);
}

// set the relative matrix. Used to set the transformation from a matrix.
void FTransform::setMatrix(const Matrix4 &m)
{
	m_matrix4 = m;

	// get the data
	Quaternion q;
	q.fromMatrix4(m_matrix4);
	q.toEuler(m_rotation);

	m_position = m_matrix4.getTranslation();
	m_scale = m_matrix4.getScale();

	setClean();
	m_hasUpdated = True;
}

// Translate the position
void FTransform::translate(const Vector3 &v)
{
	Float Sin = sinf(-m_rotation[Y]);
	Float Cos = cosf(-m_rotation[Y]);

	m_position[X] += (Cos*v[X] - Sin*v[Z]);
	m_position[Z] += (Cos*v[Z] + Sin*v[X]);
	m_position[Y] += v[Y];

	setDirty();
}

// rotate the quaternion
void FTransform::rotate(UInt32 axis, Float alpha)
{
	m_rotation[axis] += alpha;

    switch (axis)
	{
		case X: // Pitch
			if (m_rotation[X] > o3d::HALF_PI)
				m_rotation[X] = o3d::HALF_PI;
			else if (m_rotation[X] < -o3d::HALF_PI)
				m_rotation[X]  = -o3d::HALF_PI;
			break;

		case Y: // Yaw
			if (m_rotation[Y] > o3d::TWO_PI)
				m_rotation[Y] -= o3d::TWO_PI;
			else if (m_rotation[Y] < -o3d::TWO_PI)
				m_rotation[Y] += o3d::TWO_PI;
			break;

		case Z: // Roll
			if (m_rotation[Z] > o3d::TWO_PI)
				m_rotation[Z] -= o3d::TWO_PI;
			else if (m_rotation[Z] < -o3d::TWO_PI)
				m_rotation[Z] += o3d::TWO_PI;
			break;
	}

	setDirty();
}

// Rotate the quaternion
void FTransform::rotate(const Quaternion &q)
{
	Quaternion quat;

	quat.fromEuler(m_rotation);
	quat *= q;

	quat.toEuler(m_rotation);

	// limits
	if (m_rotation[X] > o3d::HALF_PI)
		m_rotation[X] = o3d::HALF_PI;
	else if (m_rotation[X] < -o3d::HALF_PI)
		m_rotation[X] = -o3d::HALF_PI;

	if (m_rotation[Y] > o3d::TWO_PI)
		m_rotation[Y] -= o3d::TWO_PI;
	else if (m_rotation[Y] < -o3d::TWO_PI)
		m_rotation[Y] += o3d::TWO_PI;

	if (m_rotation[Z] > o3d::TWO_PI)
		m_rotation[Z] -= o3d::TWO_PI;
	else if (m_rotation[Z] < -o3d::TWO_PI)
		m_rotation[Z] += o3d::TWO_PI;

	setDirty();
}

void FTransform::setPosition(const Vector3 &v)
{
    m_position = v;
    setDirty();
}

void FTransform::setRotation(const Vector3 &v)
{
    m_rotation = v;
    setDirty();
}

// define the rotation
void FTransform::setRotation(const Quaternion &q)
{
    q.toEuler(m_rotation);

	// limits
	if (m_rotation[X] > o3d::HALF_PI)
		m_rotation[X] = o3d::HALF_PI;
	else if (m_rotation[X] < -o3d::HALF_PI)
		m_rotation[X] = -o3d::HALF_PI;

	if (m_rotation[Y] > o3d::TWO_PI)
		m_rotation[Y] -= o3d::TWO_PI;
	else if (m_rotation[Y] < -o3d::TWO_PI)
		m_rotation[Y] += o3d::TWO_PI;

	if (m_rotation[Z] > o3d::TWO_PI)
		m_rotation[Z] -= o3d::TWO_PI;
	else if (m_rotation[Z] < -o3d::TWO_PI)
		m_rotation[Z] += o3d::TWO_PI;

	setDirty();
}

void FTransform::setScale(const Vector3 &v)
{
    m_scale = v;
    setDirty();
}

void FTransform::scale(const Vector3 &v)
{
    m_scale += v;
    setDirty();
}

// Set direction on Z axis
void FTransform::setDirectionZ(const Vector3 &v)
{
	Matrix4 m;
	m.reComputeBasisGivenZ(v);

	Quaternion quat;
	quat.fromMatrix4(m);

	quat.toEuler(m_rotation);

	// limits
	if (m_rotation[X] > o3d::HALF_PI)
		m_rotation[X] = o3d::HALF_PI;
	else if (m_rotation[X] < -o3d::HALF_PI)
		m_rotation[X] = -o3d::HALF_PI;

	if (m_rotation[Y] > o3d::TWO_PI)
		m_rotation[Y] -= o3d::TWO_PI;
	else if (m_rotation[Y] < -o3d::TWO_PI)
		m_rotation[Y] += o3d::TWO_PI;

	if (m_rotation[Z] > o3d::TWO_PI)
		m_rotation[Z] -= o3d::TWO_PI;
	else if (m_rotation[Z] < -o3d::TWO_PI)
		m_rotation[Z] += o3d::TWO_PI;

	setDirty();
}

// update the matrix value
Bool FTransform::update()
{
	if (isDirty())
	{
		Quaternion q;
		q.fromEuler(m_rotation);
		q.normalize();

		q.toMatrix4(m_matrix4);

		m_matrix4.setTranslation(m_position);
		m_matrix4.scale(m_scale);

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
Bool FTransform::writeToFile(OutStream &os)
{
    Transform::writeToFile(os);

    os << m_position
		 << m_rotation
		 << m_scale;

	return True;
}

Bool FTransform::readFromFile(InStream &is)
{
    Transform::readFromFile(is);

    is   >> m_position
		 >> m_rotation
		 >> m_scale;

	update();
	return True;
}


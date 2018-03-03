/**
 * @file mtransform.cpp
 * @brief Helper for object matrix transformation.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/mtransform.h"

#include "o3d/core/file.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Transform, ENGINE_TRANSFORM, BaseObject)

Bool Transform::writeToFile(OutStream &os)
{
    os << m_name;
    return True;
}

Bool Transform::readFromFile(InStream &is)
{
    setDirty();
    is >> m_name;
    return True;
}

O3D_IMPLEMENT_DYNAMIC_CLASS1(MTransform, ENGINE_MTRANSFORM, Transform)

// Set to identity the relative matrix
void MTransform::identity()
{
    // the matrix
    m_matrix4.identity();

	// and its components
	m_position.set();
	m_rotation.identity();
	m_scale.set(1.f,1.f,1.f);

	// Matrix is updated
	m_isDirty = False;
	m_hasUpdated = True;
}

// Is identity
Bool MTransform::isIdentity() const
{
	return (m_position[X] == 0.f && m_position[Y] == 0.f && m_position[Z] == 0.f &&
			m_rotation[X] == 0.f && m_rotation[Y] == 0.f && m_rotation[Z] == 0.f && m_rotation[W] == 1.f &&
			   m_scale[X] == 1.f &&    m_scale[Y] == 1.f &&    m_scale[Z] == 1.f);
}

// set the relative matrix. Used to set the transformation from a matrix.
void MTransform::setMatrix(const Matrix4 &M)
{
	m_matrix4 = M;

	// get the data
	m_rotation.fromMatrix4(m_matrix4);
	m_position = m_matrix4.getTranslation();
	m_scale = m_matrix4.getScale();

	setClean();
	m_hasUpdated = True;
}

void MTransform::translate(const Vector3 &v)
{
    m_position += v;
    setDirty();
}

void MTransform::rotate(const Quaternion &q)
{
    m_rotation *= q;
    m_rotation.normalize();
    setDirty();
}

void MTransform::setPosition(const Vector3 &v)
{
    m_position = v;
    setDirty();
}

void MTransform::setRotation(const Vector3 &v)
{
    m_rotation.fromEuler(v);
    setDirty();
}

void MTransform::setScale(const Vector3 &v)
{
    m_scale = v;
    setDirty();
}

void MTransform::scale(const Vector3 &v)
{
    m_scale += v;
    setDirty();
}

void MTransform::setRotation(const Quaternion &rot)
{
    m_rotation = rot;
    setDirty();
}

// rotate the quaternion
void MTransform::rotate(UInt32 axis, Float alpha)
{
    Quaternion q;

    switch (axis) {
		case X:
			q.fromAxisAngle3(Vector3(1.f,0.f,0.f),alpha);
			m_rotation *= q;
			break;

		case Y:
			q.fromAxisAngle3(Vector3(0.f,1.f,0.f),alpha);
			m_rotation *= q;
			break;

		case Z:
			q.fromAxisAngle3(Vector3(0.f,0.f,1.f),alpha);
			m_rotation *= q;
			break;

		default:
			break;
	}

	m_rotation.normalize();
	setDirty();
}

// Set direction on Z axis
void MTransform::setDirectionZ(const Vector3 &v)
{
	Matrix4 m;
	m.reComputeBasisGivenZ(v);
	m_rotation.fromMatrix4(m);
    setDirty();
}

Vector3 MTransform::getPosition() const
{
    return m_position;
}

Quaternion MTransform::getRotation() const
{
    return m_rotation;
}

Vector3 MTransform::getScale() const
{
    return m_scale;
}

// update the matrix value
Bool MTransform::update()
{
    if (isDirty()) {
		m_rotation.normalize();

		m_rotation.toMatrix4(m_matrix4);
		m_matrix4.setTranslation(m_position);
		m_matrix4.scale(m_scale);

		setClean();

		m_hasUpdated = True;
		return True;
    } else {
		return False;
	}
}

// serialisation
Bool MTransform::writeToFile(OutStream &os)
{
    Transform::writeToFile(os);

    os  << m_position
		<< m_rotation
		<< m_scale;

	return True;
}

Bool MTransform::readFromFile(InStream &is)
{
    Transform::readFromFile(is);

    is  >> m_position
		>> m_rotation
		>> m_scale;

	update();
	return True;
}

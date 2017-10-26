/**
 * @file atransform.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/atransform.h"

#include "o3d/core/file.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ATransform, ENGINE_ATRANSFORM, Transform)

UInt32 ATransform::getType() const
{
    return ENGINE_ATRANSFORM;
}

// Set to identity the relative matrix
void ATransform::identity()
{
    // the matrix
    m_matrix4.identity();

    // and its components
    m_position.set();
    m_rotation.identity();
    m_rot.set(0.f, 0.f, 0.f);
    m_scale.set(1.f, 1.f, 1.f);

    // Matrix is updated
    m_isDirty = False;
    m_hasUpdated = True;
}

// Is identity
Bool ATransform::isIdentity() const
{
    return (m_position[X] == 0.f && m_position[Y] == 0.f && m_position[Z] == 0.f &&
                m_rot.x() == 0.f &&     m_rot.y() == 0.f &&     m_rot.z() == 0.f &&
            m_rotation[X] == 0.f && m_rotation[Y] == 0.f && m_rotation[Z] == 0.f && m_rotation[W] == 1.f &&
               m_scale[X] == 1.f &&    m_scale[Y] == 1.f &&    m_scale[Z] == 1.f);
}

// set the relative matrix. Used to set the transformation from a matrix.
void ATransform::setMatrix(const Matrix4 &M)
{
    m_matrix4 = M;

    // get the data
    m_rotation.fromMatrix4(m_matrix4);
    m_rot.set(0.f, 0.f, 0.f);
    m_position = m_matrix4.getTranslation();
    m_scale = m_matrix4.getScale();

    setClean();
    m_hasUpdated = True;
}

// rotate the quaternion
void ATransform::rotate(UInt32 axis, Float alpha)
{
    O3D_ASSERT(axis <= 3);

    m_rot[axis] += alpha;
    setDirty();
}

void ATransform::setPosition(const Vector3 &v)
{
    m_position = v;
    setDirty();
}

void ATransform::setRotation(const Quaternion &rot)
{
    m_rotation = rot;
    setDirty();
}

void ATransform::setRotation(UInt32 axis, Float alpha)
{
    O3D_ASSERT(axis <= 3);

    m_rot[axis] = alpha;
    setDirty();
}

void ATransform::setRotation(const Vector3 &v)
{
    m_rot = v;
    setDirty();
}

void ATransform::setScale(const Vector3 &v)
{
    m_scale = v;
    setDirty();
}

void ATransform::scale(const Vector3 &v)
{
    m_scale += v;
    setDirty();
}

void ATransform::translate(const Vector3 &v)
{
    m_position += v;
    setDirty();
}

void ATransform::rotate(const Quaternion &q)
{
    m_rotation *= q;
    m_rotation.normalize();
    setDirty();
}

// Set direction on Z axis
void ATransform::setDirectionZ(const Vector3 &v)
{
    Matrix4 m;
    m.reComputeBasisGivenZ(v);
    m_rotation.fromMatrix4(m);
    setDirty();
}

// update the matrix value
Bool ATransform::update()
{
    if (isDirty())
    {
        Quaternion q;
        q.fromEuler(m_rot);

        m_rotation.normalize();
        q *= m_rotation;

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
Bool ATransform::writeToFile(OutStream &os)
{
    Transform::writeToFile(os);

    os  << m_position
        << m_rotation
        << m_rot
        << m_scale;

    return True;
}

Bool ATransform::readFromFile(InStream &is)
{
    Transform::readFromFile(is);

    is  >> m_position
        >> m_rotation
        >> m_rot
        >> m_scale;

    update();
    return True;
}


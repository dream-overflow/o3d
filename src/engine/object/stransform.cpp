/**
 * @file stransform.cpp
 * @brief Additive transformation supporting spacial type transform.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-02-24
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/stransform.h"

#include "o3d/core/file.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(STransform, ENGINE_STRANSFORM, Transform)

// Set to identity the relative matrix
void STransform::identity()
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
Bool STransform::isIdentity() const
{
    return (m_position[X] == 0.f && m_position[Y] == 0.f && m_position[Z] == 0.f &&
            m_rotation[X] == 0.f && m_rotation[Y] == 0.f && m_rotation[Z] == 0.f &&
               m_scale[X] == 1.f &&    m_scale[Y] == 1.f &&    m_scale[Z] == 1.f);
}

// set the relative matrix. Used to set the transformation from a matrix.
void STransform::setMatrix(const Matrix4 &m)
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

// Translate the position according to the current direction
void STransform::translate(const Vector3 &v)
{
    Quaternion q;
    q.fromEuler(m_rotation);

    Vector3 tr(v);
    q.transform(tr);

    m_position += tr;

    setDirty();
}

// rotate the quaternion
void STransform::rotate(UInt32 axis, Float alpha)
{
    m_rotation[axis] += alpha;

    switch (axis) {
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
void STransform::rotate(const Quaternion &q)
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

void STransform::setPosition(const Vector3 &v)
{
    m_position = v;
    setDirty();
}

void STransform::setRotation(const Vector3 &v)
{
    m_rotation = v;
    setDirty();
}

// define the rotation
void STransform::setRotation(const Quaternion &q)
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

void STransform::setScale(const Vector3 &v)
{
    m_scale = v;
    setDirty();
}

void STransform::scale(const Vector3 &v)
{
    m_scale += v;
    setDirty();
}

// Set direction on Z axis
void STransform::setDirectionZ(const Vector3 &v)
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

Vector3 STransform::getPosition() const
{
    return m_position;
}

Quaternion STransform::getRotation() const
{
    Quaternion q;
    q.fromEuler(m_rotation);
    q.normalize();

    return q;
}

Vector3 STransform::getScale() const
{
    return m_scale;
}

// update the matrix value
Bool STransform::update()
{
    if (isDirty()) {
        Quaternion q;
        q.fromEuler(m_rotation);
        q.normalize();

        q.toMatrix4(m_matrix4);

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
Bool STransform::writeToFile(OutStream &os)
{
    Transform::writeToFile(os);

    os << m_position
         << m_rotation
         << m_scale;

    return True;
}

Bool STransform::readFromFile(InStream &is)
{
    Transform::readFromFile(is);

    is   >> m_position
         >> m_rotation
         >> m_scale;

    update();
    return True;
}

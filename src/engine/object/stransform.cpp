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

// Default constructor. No rotation, no translation, and uniform scale of one.
STransform::STransform(BaseObject *parent) :
    Transform(parent),
    m_scale(1.f, 1.f, 1.f)
{

}

// Set to identity the relative matrix
void STransform::identity()
{
    // the matrix
    m_matrix4.identity();

    // and its components
    m_position.set();
    m_rotation.identity();
    m_scale.set(1.f, 1.f, 1.f);
    m_euler.zero();

    // Matrix is updated
    m_isDirty = False;
    m_hasUpdated = True;
}

// Is identity
Bool STransform::isIdentity() const
{
    return (m_position[X] == 0.f && m_position[Y] == 0.f && m_position[Z] == 0.f &&
            m_rotation.isIdentity() &&
            m_scale[X] == 1.f && m_scale[Y] == 1.f && m_scale[Z] == 1.f);
}

// set the relative matrix. Used to set the transformation from a matrix.
void STransform::setMatrix(const Matrix4 &m)
{
    m_matrix4 = m;

    // get the data
    m_rotation.fromMatrix4(m_matrix4);
    m_rotation.toEuler(m_euler);

    m_position = m_matrix4.getTranslation();
    m_scale = m_matrix4.getScale();

    setClean();
    m_hasUpdated = True;
}

// Translate the position according to the current direction
void STransform::translate(const Vector3 &v)
{
    Vector3 tr(v);
    m_rotation.transform(tr);

    m_position += tr;

    setDirty();
}

void STransform::setRoll(Float alpha)
{
    m_euler.z() = alpha;

    // update rotation
    m_rotation.fromEuler(Vector3(m_euler.x(), m_euler.y(), 0));
    m_rotation.normalize();

    Quaternion roll;
    roll.fromAxisAngle3(Vector3(0, 0, 1), alpha);
    m_rotation *= roll;

    setDirty();
}

// rotate the quaternion
void STransform::rotate(UInt32 axis, Float alpha)
{   
    m_euler[axis] += alpha;

//    if (axis == X) {
//        // limit X from [-Pi..Pi]
//        m_euler.x() = clamp(simplifyRadian(m_euler.x()), -o3d::HALF_PI, o3d::HALF_PI);
//    }

    m_rotation.fromEuler(Vector3(m_euler.x(), m_euler.y(), 0));
    m_rotation.normalize();

    Quaternion roll;
    roll.fromAxisAngle3(Vector3(0, 0, 1), m_euler.z());
    m_rotation *= roll;

    setDirty();
}

// Rotate the quaternion
void STransform::rotate(const Quaternion &q)
{
    m_rotation *= q;
    m_rotation.normalize();

    // update euler angles
    m_rotation.toEuler(m_euler);

    // @todo but no limits on Y...

    setDirty();
}

void STransform::setPosition(const Vector3 &v)
{
    m_position = v;
    setDirty();
}

void STransform::setRotation(const Vector3 &v)
{
    m_euler = v;

    // limit X from [-Pi..Pi]
    // m_euler.x() = clamp(simplifyRadian(m_euler.x()), -o3d::HALF_PI, o3d::HALF_PI);

    m_rotation.fromEuler(v);

    setDirty();
}

// define the rotation
void STransform::setRotation(const Quaternion &q)
{
    m_rotation = q;
    m_rotation.toEuler(m_euler);

    // @todo but no limits on Y...

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
    m_rotation.fromMatrix4(m);
    setDirty();

    m_rotation.toEuler(m_euler);

    // @todo but no limits on Y...

    setDirty();
}

Vector3 STransform::getPosition() const
{
    return m_position;
}

Quaternion STransform::getRotation() const
{
    return m_rotation;
}

Vector3 STransform::getScale() const
{
    return m_scale;
}

// update the matrix value
Bool STransform::update()
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
Bool STransform::writeToFile(OutStream &os)
{
    Transform::writeToFile(os);

    os   << m_position
         << m_euler
         << m_rotation
         << m_scale;

    return True;
}

Bool STransform::readFromFile(InStream &is)
{
    Transform::readFromFile(is);

    is   >> m_position
         >> m_euler
         >> m_rotation
         >> m_scale;

    update();
    return True;
}

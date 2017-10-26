/**
 * @file atransform.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ATRANSFORM_H
#define _O3D_ATRANSFORM_H

#include "transform.h"

#include "o3d/core/quaternion.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Animation oriented transform.
 * It contains a SRT tranform with two types for the rotation (euler vector + quaternion).
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2014-03-30
 */
class O3D_API ATransform : public Transform
{
    O3D_DECLARE_DYNAMIC_CLASS_NO_COPY(ATransform)

public:

    friend class Node;

    //! Default constructor. No rotation, no translation, and uniform scale of one.
    ATransform(BaseObject *parent = nullptr) :
        Transform(parent),
        m_scale(1.f,1.f,1.f)
    {}

    //! Get the transform type (useful for casting)
    virtual UInt32 getType() const;

    //! Set to identity the relative matrix
    virtual void identity();

    //! Is identity
    virtual Bool isIdentity() const;

    //! Set the relative matrix. Used to set the transformation from a matrix.
    //! scale, translation and quaternion are extracted from the matrix. This is not
    //! very speed optimal too.
    virtual void setMatrix(const Matrix4 &M);

    //! Translate the position
    virtual void translate(const Vector3 &v);

    //! Rotate the axis by an angle (in euler)
    virtual void rotate(UInt32 axis, Float alpha);

    //! Rotate about an arbitrary axis
    virtual void rotate(const Quaternion &q);

    //! Define the position
    virtual void setPosition(const Vector3 &v);

    //! Define the rotation
    virtual void setRotation(const Quaternion &rot);

    //! Define the rotation as euler for a specified axis
    virtual void setRotation(UInt32 axis, Float alpha);

    //! Define the rotation as euler
    virtual void setRotation(const Vector3 &v);

    //! Define the scale
    virtual void setScale(const Vector3 &v);

    //! Scale
    virtual void scale(const Vector3 &v);

    //! Set direction on Z axis
    virtual void setDirectionZ(const Vector3 &v);


    //! Get the position
    inline const Vector3& getPosition() const { return m_position; }

    //! Get float ptr position
    inline const Float* getPositionPtr() const { return m_position.getData();}

    //! Get the rotation
    inline const Quaternion& getRotation() const { return m_rotation; }

    //! Get the euler rotation part
    inline const Vector3& getEuler() const { return m_rot; }

    //! Get the scale
    inline const Vector3& getScale() const { return m_scale; }


    //! @brief Update the matrix value. this method is performed only if the transform is dirty.
    //! Update perform the matrix computation if dirty, using the SRT (scale/rotate/translate).
    //! That mean the transforms are always additive and not cumulative.
    //! @return True mean the matrix was recomputed.
    virtual Bool update();

    //! Serialisation
    virtual Bool writeToFile(OutStream &os);
    virtual Bool readFromFile(InStream &is);

protected:

    Quaternion m_rotation;  //!< Quaternion rotation
    Vector3 m_rot;          //!< Euler rotation

    Vector3 m_position;     //!< translation
    Vector3 m_scale;        //!< scale
};


} // namespace o3d

#endif // _O3D_ATRANSFORM_H


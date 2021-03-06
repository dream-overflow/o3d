/**
 * @file stransform.h
 * @brief Additive transformation supporting spacial type transform.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2018-02-24
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_STRANSFORM_H
#define _O3D_STRANSFORM_H

#include "transform.h"

#include "o3d/core/quaternion.h"
#include "o3d/core/memorydbg.h"

#include <math.h>

namespace o3d {

/**
 * @brief Additive transformation supporting spacial type transform.
 */
class O3D_API STransform : public Transform
{
    O3D_DECLARE_DYNAMIC_CLASS_NO_COPY(STransform)

public:

    //! Default constructor. No rotation, no translation, and uniform scale of one.
    STransform(BaseObject *parent = nullptr);

    //! Set to identity the relative matrix
    virtual void identity() override;

    //! Is identity
    virtual Bool isIdentity() const override;

    //! Set the relative matrix. Used to set the transformation from a matrix.
    //! scale, translation and quaternion are extracted from the matrix. This is not
    //! very speed optimal too.
    virtual void setMatrix(const Matrix4 &M) override;

    //! Translate the position
    virtual void translate(const Vector3 &v) override;

    //! Set roll angle (rotation on Z axis)
    void setRoll(Float alpha);

    //! Rotate the quaternion
    virtual void rotate(UInt32 axis, Float alpha) override;

    //! Rotate the quaternion
    virtual void rotate(const Quaternion &q) override;

    //! Define the position
    virtual void setPosition(const Vector3 &v) override;

    //! Define the rotation
    virtual void setRotation(const Vector3 &v) override;

    //! Define the rotation
    virtual void setRotation(const Quaternion &q) override;

    //! Define the scale
    virtual void setScale(const Vector3 &v) override;

    //! Scale
    virtual void scale(const Vector3 &v) override;

    //! Set direction on Z axis
    virtual void setDirectionZ(const Vector3 &v) override;

    //! Get the position
    virtual Vector3 getPosition() const override;

    //! Get float ptr position
    inline const Float* getPositionPtr() const { return m_position.getData();}

    //! Get the rotation
    virtual Quaternion getRotation() const override;

    //! Get the scale
    virtual Vector3 getScale() const override;

    //! Get the rotation as euler.
    inline const Vector3& getEuler() const { return m_euler; }

    //! @brief Update the matrix value. this method is performed only if the transform is dirty.
    //! Update perform the matrix computation if dirty, using the SRT (scale/rotate/translate).
    //! That mean the transforms are always additive and not cumulative.
    //! @return True mean the matrix was recomputed.
    virtual Bool update() override;

    // serialisation
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

public:  // public for conveniance, but not for external usage

    Vector3 m_euler;
    Quaternion m_rotation;   //!< quaternion
    Vector3 m_position;      //!< translation
    Vector3 m_scale;         //!< scale
};

} // namespace o3d

#endif // _O3D_STRANSFORM_H

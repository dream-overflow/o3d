/**
 * @file mtransform.h
 * @brief Additive transformation.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MTRANSFORM_H
#define _O3D_MTRANSFORM_H

#include "transform.h"

#include "o3d/core/quaternion.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class MTransform
//-------------------------------------------------------------------------------------
//! MTransform mean Matrix Transform and it offer a structure to manage a 4x4
//! transformation matrix. Transformation use of a triplet {quaternion,translation,scale}
//! where transformation are applied, and an update method to recompute the final
//! 4x4 matrix. Transformation can be done to directly to the matrix if you prefer
//! this way, but prefer usage of the triplet and update method.
//---------------------------------------------------------------------------------------
class O3D_API MTransform : public Transform
{
	O3D_DECLARE_DYNAMIC_CLASS_NO_COPY(MTransform)

public:

	friend class Node;

	//! Default constructor. No rotation, no translation, and uniform scale of one.
    MTransform(BaseObject *parent = nullptr) :
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

	//! Rotate the quaternion
	virtual void rotate(UInt32 axis, Float alpha);

	//! Rotate about an arbitrary axis
    virtual void rotate(const Quaternion &q);

	//! Define the position
    virtual void setPosition(const Vector3 &v);

	//! Define the rotation
    virtual void setRotation(const Quaternion &rot);

	//! Define the rotation
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

	Vector3 m_position;     //!< translation
	Vector3 m_scale;        //!< scale
};

} // namespace o3d

#endif // _O3D_MTRANSFORM_H


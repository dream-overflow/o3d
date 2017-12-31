/**
 * @file dqtransform.h
 * @brief Helper for object dual quaternion transformation.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DQTRANSFORM_H
#define _O3D_DQTRANSFORM_H

#include "transform.h"

#include "o3d/core/dualquaternion.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class DQTransform
//-------------------------------------------------------------------------------------
//! DQTransform mean Dual Quaternion Transform and it offer a structure to manage a dual
//! quaternion transformation, and a conversion to a 4x4 matrix. Transformation use of a
//! 2uplet {dual quaternion,scale}. Scale is not applied with dual quaternion, it can
//! only be processed onto the matrix. If you don't need of the matrix and the scale
//! you can use directly an O3DDualQuaternion into your object.
//---------------------------------------------------------------------------------------
class O3D_API DQTransform : public Transform
{
	O3D_DECLARE_DYNAMIC_CLASS_NO_COPY(DQTransform)

public:

	//! Default constructor. No rotation, no translation, and uniform scale of one.
    DQTransform(BaseObject *parent = nullptr) :
		Transform(parent),
		m_Scale(1.f,1.f,1.f)
	{}

	//! Get the transform type (useful for casting)
	virtual UInt32 getType() const { return ENGINE_DQTRANSFORM; }


	//! set to identity the relative matrix
	virtual void identity();

	//! Identity the transform
	virtual Bool isIdentity() const;


	//! set the relative matrix. Used to set the transformation from a matrix.
	//! scale, translation and quaternion are extracted from the matrix. This is not
	//! very speed optimal too.
	virtual void setMatrix(const Matrix4 &M);

	//! translate the position
	virtual void translate(const Vector3 &v) { setDirty(); }

	//! rotate the quaternion
	virtual void rotate(UInt32 axis, Float alpha);

	//! rotate about an arbitrary axis
	virtual void rotate(const Quaternion &q) { m_PositionRotation *= q; /*m_PositionRotation.normalize();*/ setDirty(); }

	//! transform by another dual quaternion
	inline void transform(const DualQuaternion &q) { m_PositionRotation *= q; /*m_PositionRotation.normalize();*/ setDirty(); }

	//! define the position
	virtual void setPosition(const Vector3 &v) { setDirty(); }

	//! define the scale
	virtual void setScale(const Vector3 &v) { m_Scale = v; setDirty(); }

	//! scale
	virtual void scale(const Vector3 &v) { m_Scale += v; setDirty(); }

	//! define the rotation
	virtual void setRotation(const Quaternion &Quat);

	//! Define the rotation
	virtual void setRotation(const Vector3 &v) { O3D_ASSERT(0); }

	//! Set direction on Z axis
	virtual void setDirectionZ(const Vector3 &v) { O3D_ASSERT(0); }


	//! get the position
	inline Vector3 getPosition() const { return m_PositionRotation.getTranslation(); }

	//! get the rotation
	inline const Quaternion getRotation() const { return m_PositionRotation.getRotation(); }

	//! get the scale
	inline const Vector3& getScale() const { return m_Scale; }


	//! @brief Update the matrix value. this method is performed only if the transform is dirty.
	//! Update perform the matrix computation if dirty, using the SRT (scale/rotate/translate).
	//! That mean the transforms are always additive and not cumulative. To perform cumulative
	//! transform use of the SetMatrix method.
	//! @return True mean the matrix was recomputed.
	virtual Bool update();

	// serialisation
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

public: // public for conveniance, but not for external usage

	DualQuaternion m_PositionRotation;  //!< Dual Quaternion position/rotation
	Vector3 m_Scale;                    //!< 3d scaling
};

} // namespace o3d

#endif // _O3D_DQTRANSFORM_H


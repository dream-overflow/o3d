/**
 * @file transform.h
 * @brief Transform object. Mainly used in node to transform the basis.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-07-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TRANSFORM_H
#define _O3D_TRANSFORM_H

#include "o3d/core/matrix4.h"
#include "o3d/core/quaternion.h"
#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"
#include "o3d/core/baseobject.h"
#include "../enginetype.h"

namespace o3d {

/**
 * @brief Transform object. Mainly used in node to transform the basis.
 */
class O3D_API Transform : public BaseObject
{
	O3D_DECLARE_ABSTRACT_CLASS_NO_COPY(Transform)

public:

	//! Default constructor
    Transform(BaseObject *parent = nullptr) :
		BaseObject(parent),
		m_isDirty(True),
		m_hasUpdated(False) {}

	//! Virtual destructor
	virtual ~Transform() {}

	//-----------------------------------------------------------------------------------
	// Transform
	//-----------------------------------------------------------------------------------

	//! Identity the transform
	virtual void identity() = 0;

	//! Is identity
	virtual Bool isIdentity() const = 0;

	//! Set from a 4x4 matrix
	virtual void setMatrix(const Matrix4 &M) = 0;

	//! Return as O3DMatrix4 (read only).
	//! @note Update is called if dirty.
	inline const Matrix4& getMatrix()
	{
		if (m_isDirty)
			update();

		return m_matrix4;
	}

	//! Translate the position
	virtual void translate(const Vector3 &v) = 0;

	//! Rotate about an arbitrary axis
	virtual void rotate(const Quaternion &q) = 0;

	//! Rotate the quaternion
	virtual void rotate(UInt32 axis, Float alpha) = 0;

	//! Scale
	virtual void scale(const Vector3 &v) = 0;

	//! Define the position
	virtual void setPosition(const Vector3 &v) = 0;

	//! Define the rotation
	virtual void setRotation(const Quaternion &rot) = 0;

	//! Define the rotation
	virtual void setRotation(const Vector3 &v) = 0;

	//! Define the scale
	virtual void setScale(const Vector3 &v) = 0;

	//! Set direction on Z axis
	virtual void setDirectionZ(const Vector3 &v) = 0;


	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! @brief Update the matrix state. This method is performed only if the transform is dirty.
	//! Update perform the matrix computation when dirty, using the virtual method.
	//! @return True mean the matrix was recomputed.
	virtual Bool update() = 0;

	//! Is the transform is dirty. That mean it need an Update.
	inline Bool isDirty() const { return m_isDirty; }

	//! Check if the result matrix was changed. Use ClearUpdated() to reset this flag.
	inline Bool hasUpdated() const { return m_hasUpdated; }

	//! Set the transform 'changed' state as read/acquired (put it to false)
	inline void clearUpdated() { m_hasUpdated = False; }

	//! Force to update the matrix value NOW, even if it is not dirty.
	inline void forceUpdate() { m_isDirty = True; update(); }


	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

    virtual Bool writeToFile(OutStream &os) override
	{
        os << m_name;
		return True;
	}

    virtual Bool readFromFile(InStream &is) override
	{
		setDirty();
        is >> m_name;
		return True;
	}

protected:

	Matrix4 m_matrix4;   //!< matrix 4 version of the transform

	Bool m_isDirty;     //!< true mean an update is needed
	Bool m_hasUpdated;  //!< true mean the transform has updated

	//! Set the matrix as dirty. That mean it need an Update.
	inline void setDirty() { m_isDirty = True; }
	//! Clear the dirty matrix flag. Mean the matrix was updated.
	inline void setClean() { m_isDirty = False; }
};

} // namespace o3d

#endif // _O3D_TRANSFORM_H

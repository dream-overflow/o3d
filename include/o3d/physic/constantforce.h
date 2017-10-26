/**
 * @file constantforce.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CONSTANTFORCE_H
#define _O3D_CONSTANTFORCE_H

#include "o3d/core/memorydbg.h"
#include "o3d/physic/abcforce.h"

namespace o3d {

/**
 * @brief A simple constant force.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-05-31
 */
class O3D_API ConstantForce : public ABCForce
{
public:

	O3D_DECLARE_CLASS(ConstantForce)

	//! default constructor
	ConstantForce(
		BaseObject *pParent,
        const Vector3& Force = Vector3(0.f, -9.81f, 0.f)) :
			ABCForce(pParent),
			m_Force(Force)
	{}

	//! Set/Get the force
	inline void setForce(const Vector3 &force) { m_Force = force; }
	inline const Vector3& getForce()const { return m_Force;	}
	inline       Vector3& getForce()      { return m_Force;	}

	//! Add/Sub force
	inline Vector3& addForce(const Vector3 &force) { return m_Force += force; }
	inline Vector3& subForce(const Vector3 &force) { return m_Force -= force; }

	//! process the force on the object
	virtual void processObject(class RigidBody& RigidBody);

	//! process the force on the particule
	//virtual void processParticule(class Particule& Particule);

	//! serialization
	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	Vector3 m_Force;  //!< the force vector
};

} // namespace o3d

#endif // _O3D_CONSTANTFORCE_H


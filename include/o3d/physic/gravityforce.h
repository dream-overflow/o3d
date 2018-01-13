/**
 * @file gravityforce.h
 * @brief TheGravity Force.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-05-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GRAVITYFORCE_H
#define _O3D_GRAVITYFORCE_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/base.h"
#include "o3d/core/vector3.h"
#include "abcforce.h"

namespace o3d {

/**
 * @brief The gravity can be applied to an object or a particule
 */
class O3D_API GravityForce : public ABCForce
{
public:

	O3D_DECLARE_CLASS(GravityForce)

	//! Constructor
	GravityForce(
		BaseObject *pParent,
        const Vector3& gravity = Vector3(0.f, -9.81f, 0.f)) :
			ABCForce(pParent),
			m_gravity(gravity)
    {
    }

	//! Set/Get the gravity
    inline void setGravity(const Vector3 &force) { m_gravity = force; }
	inline const Vector3& getGravity()const { return m_gravity; }
    inline Vector3& getGravity() { return m_gravity; }

	//! Add/Sub force to the gravity
	inline Vector3& addGravity(const Vector3 &force) { return m_gravity += force; }
	inline Vector3& subGravity(const Vector3 &force) { return m_gravity -= force; }

	//! process the force on the object
    virtual void processObject(class RigidBody& RigidBody) override;

	//! process the force on the particule
    //virtual void processParticule(class Particule& Particule) override;

	//! serialization
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

	Vector3 m_gravity;    //!< gravity vector (F=m*g)
};

} // namespace o3d

#endif // _O3D_GRAVITYFORCE_H

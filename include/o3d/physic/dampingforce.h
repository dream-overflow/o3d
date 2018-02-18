/**
 * @file dampingforce.h
 * @brief A viscous damping force (add viscous drag)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-06-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DAMPINGFORCE_H
#define _O3D_DAMPINGFORCE_H

#include "o3d/core/memorydbg.h"
#include "../core/base.h"
#include "../core/vector3.h"
#include "abcforce.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class DampingForce
//-------------------------------------------------------------------------------------
//! Damping force are opposed to speed via equation :
//!
//!    F=-Kf*Speed
//!
//! where Kf is the damping coefficient.
//---------------------------------------------------------------------------------------
class O3D_API DampingForce : public ABCForce
{
public:

	O3D_DECLARE_CLASS(DampingForce)

	//! default constructor
	DampingForce(
		BaseObject *pParent,
		Float damping = 0.f) :
			ABCForce(pParent),
			m_Damping(damping)
	{}

	//! Set/Get the damping coefficient
	inline void setDamping(Float damping) { m_Damping = damping; }
	inline Float getGravity()const { return m_Damping; }

	//! process the force on the object
    virtual void processObject(class RigidBody& RigidBody) override;

	//! process the force on the particule
    //virtual void processParticule(class Particule& Particule) override;

	// serialization
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

	Float m_Damping;    //!< the damping coefficient
};

} // namespace o3d

#endif // _O3D_DAMPINGFORCE_H

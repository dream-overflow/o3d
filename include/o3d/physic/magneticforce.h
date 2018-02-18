/**
 * @file magneticforce.h
 * @brief A magnetic field force.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-05-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MAGNETICFORCE_H
#define _O3D_MAGNETICFORCE_H

#include "o3d/core/memorydbg.h"
#include "../core/base.h"
#include "../core/vector3.h"
#include "abcforce.h"

namespace o3d {

/**
 * @brief A magnetic force compute by Lorentz law with q=1 :
 *  F = v^B
 * Where B is the magnetic field.
 */
class O3D_API MagneticForce : public ABCForce
{
public:

	O3D_DECLARE_CLASS(MagneticForce)

	//! default constructor
	MagneticForce(
		BaseObject *pParent,
		const Vector3& MagneticField = Vector3(0.f,0.f,0.f)) :
			ABCForce(pParent),
            m_magneticField(MagneticField)
    {
    }

    //! Set the magnetic field
    inline void setMagneticField(const Vector3 &force) { m_magneticField = force; }
    //! Get the magnetic field
    inline const Vector3& getMagneticField() const { return m_magneticField; }
    //! Get the magnetic field
    inline Vector3& getMagneticField() { return m_magneticField; }

    //! Add force to the magnetic field
    inline Vector3& addForce(const Vector3 &force) { return m_magneticField += force; }
    //! Sub force to the magnetic field
    inline Vector3& subForce(const Vector3 &force) { return m_magneticField -= force; }

	//! process the force on the object
    virtual void processObject(class RigidBody& RigidBody) override;

    //! process the force on the particule
    //virtual void processParticule(class Particule& Particule) override;

	//! serialization
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

    Vector3 m_magneticField;   //!< the magnetic field
};

} // namespace o3d

#endif // _O3D_MAGNETICFORCE_H

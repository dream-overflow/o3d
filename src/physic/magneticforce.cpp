/**
 * @file magneticforce.cpp
 * @brief A magnetic field force
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-05-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/magneticforce.h"

#include "o3d/physic/rigidbody.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(MagneticForce, PHYSIC_FORCE_MAGNETIC, ABCForce)

void MagneticForce::processObject(class RigidBody& rigidBody)
{
    rigidBody.addForce(rigidBody.getSpeed() ^ m_magneticField);
}

//void MagneticForce::processParticule(class Particule& particule)
//{
//	particule.addForce(RigidBody.getSpeed() ^ m_MagneticField);
//}

Bool MagneticForce::writeToFile(OutStream &os)
{
    ABCForce::writeToFile(os);
    os << m_magneticField;
	return True;
}

Bool MagneticForce::readFromFile(InStream &is)
{
    ABCForce::readFromFile(is);
    is >> m_magneticField;
	return True;
}

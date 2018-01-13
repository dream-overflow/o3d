/**
 * @file gravityforce.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/gravityforce.h"

#include "o3d/physic/rigidbody.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(GravityForce, PHYSIC_FORCE_GRAVITY, ABCForce)

void GravityForce::processObject(class RigidBody& rigidBody)
{
    Float mass = rigidBody.getMass();
    rigidBody.addForce(m_gravity * mass);
}

//void GravityForce::processParticule(class Particule& particule)
//{
//	Float invMass = particule.getInvMass();
//	if (invMass > 0.f) {
//      particule.addForce(m_gravity / invMass);
//  }
//}

Bool GravityForce::writeToFile(OutStream &os)
{
    ABCForce::writeToFile(os);
    os << m_gravity;
	return True;
}

Bool GravityForce::readFromFile(InStream &is)
{
    ABCForce::readFromFile(is);
    is >> m_gravity;
	return True;
}

/**
 * @file gravityforce.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/gravityforce.h"

#include "o3d/physic/rigidbody.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(GravityForce, PHYSIC_FORCE_GRAVITY, ABCForce)

/*---------------------------------------------------------------------------------------
  process the force on the object
---------------------------------------------------------------------------------------*/
void GravityForce::processObject(class RigidBody& rigidBody)
{
    Float mass = rigidBody.getMass();
    rigidBody.addForce(m_Gravity * mass);
}

/*---------------------------------------------------------------------------------------
  process the force on the particule
---------------------------------------------------------------------------------------*/
//void GravityForce::processParticule(class Particule& Particule)
//{
//	Float InvMass = Particule.getInvMass();
//	if (InvMass > 0.f) Particule.addForce(m_Gravity / InvMass);
//}

/*---------------------------------------------------------------------------------------
  serialisation
---------------------------------------------------------------------------------------*/
Bool GravityForce::writeToFile(OutStream &os)
{
    ABCForce::writeToFile(os);
    os << m_Gravity;
	return True;
}

Bool GravityForce::readFromFile(InStream &is)
{
    ABCForce::readFromFile(is);
    is >> m_Gravity;
	return True;
}


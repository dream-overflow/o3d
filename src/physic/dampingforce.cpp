/**
 * @file dampingforce.cpp
 * @brief A viscous damping force (add viscous drag)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-06-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/dampingforce.h"

#include "o3d/physic/rigidbody.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(DampingForce, PHYSIC_FORCE_DAMPING, ABCForce)

//---------------------------------------------------------------------------------------
// process the force on the object
//---------------------------------------------------------------------------------------
void DampingForce::processObject(class RigidBody& rigidBody)
{
	rigidBody.addForce(rigidBody.getSpeed() * (-m_Damping));
}

//---------------------------------------------------------------------------------------
// process the force on the particule
//---------------------------------------------------------------------------------------
//void O3DDampingForce::processParticule(class O3DParticule& Particule)
//{
//	Particule.addForce(Particule.getSpeed() * (-m_Damping));
//}

//---------------------------------------------------------------------------------------
// serialisation
//---------------------------------------------------------------------------------------
Bool DampingForce::writeToFile(OutStream &os)
{
    ABCForce::writeToFile(os);
    os << m_Damping;
	return True;
}

Bool DampingForce::readFromFile(InStream &is)
{
    ABCForce::readFromFile(is);
    is >> m_Damping;
	return True;
}


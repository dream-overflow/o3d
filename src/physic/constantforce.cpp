/**
 * @file constantforce.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/constantforce.h"

#include "o3d/physic/rigidbody.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ConstantForce, PHYSIC_FORCE_CONSTANT, ABCForce)

//---------------------------------------------------------------------------------------
// process the force on the object
//---------------------------------------------------------------------------------------
void ConstantForce::processObject(class RigidBody& rigidBody)
{
	rigidBody.addForce(m_Force);
}

//---------------------------------------------------------------------------------------
// process the force on the particule
//---------------------------------------------------------------------------------------
//void O3DConstantForce::processParticule(class O3DParticule& Particule)
//{
//	Particule.addForce(m_Force);
//}

//---------------------------------------------------------------------------------------
// serialisation
//---------------------------------------------------------------------------------------
Bool ConstantForce::writeToFile(OutStream &os)
{
    ABCForce::writeToFile(os);
    os << m_Force;
	return True;
}

Bool ConstantForce::readFromFile(InStream &is)
{
    ABCForce::readFromFile(is);
    is >> m_Force;
	return True;
}


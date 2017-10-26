/**
 * @file springforce.cpp
 * @brief A spring force
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-06-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/springforce.h"

#include "o3d/physic/rigidbody.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(SpringForce, PHYSIC_FORCE_SPRING, ABCForce)

/*---------------------------------------------------------------------------------------
  process the force on the object
---------------------------------------------------------------------------------------*/
void SpringForce::processObject(class RigidBody& rigidBody)
{
	Vector3 delta_P = m_Anchor - rigidBody.getPosition();
	Float Dist = delta_P.length();

	if (Dist > o3d::Limits<Float>::epsilon())
	{
		Vector3 delta_P_normalized = delta_P / Dist;
		Float FactorS = (Dist - m_RestLength) * m_Stiffness;
		Float FactorD = (rigidBody.getSpeed() * delta_P_normalized) * m_Dampling;
		Vector3 Forces = delta_P_normalized * (FactorS - FactorD);

		// add the force to the accumulator
		rigidBody.addForce(Forces);
		// TODO : add a torque and an attach in body frame
	}
}

/*---------------------------------------------------------------------------------------
  process the force on the particule
---------------------------------------------------------------------------------------*/
//void SpringForce::processParticule(class Particule& particule)
//{
//	Vector3 delta_P = m_Anchor - particule.getPosition();
//	Float dist = delta_P.length();
//
//	if (dist > o3d::Limits<Float>::epsilon())
//	{
//		Vector3 delta_P_normalized = delta_P / dist;
//		Float factorS = (dist - m_RestLength) * m_Stiffness;
//		Float factorD = (Particule.getSpeed() * delta_P_normalized) * m_Dampling;
//		Vector3 forces = delta_P_normalized * (factorS - factorD);
//
//		// add the force to the accumulator
//		particule.addForce(forces);
//		// TODO : add a torque and an attach in body frame
//	}
//}

/*---------------------------------------------------------------------------------------
  serialisation
---------------------------------------------------------------------------------------*/
Bool SpringForce::writeToFile(OutStream &os)
{
    ABCForce::writeToFile(os);

    os   << m_Stiffness
		 << m_Dampling
		 << m_RestLength
		 << m_Anchor;

	return True;
}

Bool SpringForce::readFromFile(InStream &is)
{
    ABCForce::readFromFile(is);

    is   >> m_Stiffness
		 >> m_Dampling
		 >> m_RestLength
		 >> m_Anchor;

	return True;
}


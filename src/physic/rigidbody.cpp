/**
 * @file rigidbody.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/rigidbody.h"

#include "o3d/engine/scene/sceneobject.h"
#include "o3d/physic/forcemanager.h"
#include "o3d/physic/physicentitymanager.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(RigidBody, PHYSIC_RIGID_BODY, PhysicEntity)

/*---------------------------------------------------------------------------------------
  constructor
---------------------------------------------------------------------------------------*/
RigidBody::RigidBody(SceneObject *object, Bool assignToObject) :
	PhysicEntity(object),
    m_isPhysic(True),
    m_mass(1),
    m_bounce(0.8f),
    m_friction(0.f),
    m_forceManager(nullptr),
    m_physicModel(nullptr)
{
    m_type = PHYSIC_RIGID_BODY;
    m_object = object;

    if (assignToObject) {
        m_object->setRigidBody(this);
    }

	synchronizeWithObject();
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
RigidBody::~RigidBody()
{
    deletePtr(m_forceManager);
}

/*---------------------------------------------------------------------------------------
  update physic with euler method
---------------------------------------------------------------------------------------*/
void RigidBody::updatePhysicEuler(Double t, Double dt)
{
    if (!m_isPhysic)
		return;

	/*
	 *	d m_Pos(t)
	 *	--------- = m_Speed(t)
	 *	    dt
	 *
	 *	d m_Rot(t)  1
	 *	--------- = - * (Quaternion(m_Omega(t),0)) * m_Rot(t)
	 *      dt      2
	 *
	 *	d m_P(t)
	 *	------- = m_ForceAccumulator(t)
	 *	  dt
	 *
	 *	d m_L(t)
	 *	------- = m_TorqueAccumulator(t)
	 *	  dt
	 */

	// add the impulse
    m_P += m_forceImpulseAccumulator;
    m_L += m_torqueImpulseAccumulator;

	// update the state variables using euler method
    m_P += m_forceAccumulator * dt;
    m_pos += m_speed * dt;
    m_L += m_torqueAccumulator * dt;
    m_rot += (Quaternion(m_omega.x(), m_omega.y(), m_omega.z(), 0.f) * m_rot) * (dt * 0.5f);
    m_rot.normalize();

	computeVariables();

	// save previous position
    m_worldToBodyOld = m_worldToBody;
	computeWorldToBody();

    // finally clear the accumulators
    resetAccumulators();
}

void RigidBody::updatePhysicRK4(Double t, Double dt, UInt32 numStepRK4)
{
    if (!m_isPhysic)
        return;

    // TODO

    // finally clear the accumulators
    resetAccumulators();
}

/*---------------------------------------------------------------------------------------
  update physic with verlet method
---------------------------------------------------------------------------------------*/
void RigidBody::updatePhysicVerlet(Double t, Double dt)
{
    if (!m_isPhysic)
		return;

    // TODO Luhtor eheh

    // finally clear the accumulators
	resetAccumulators();
}

/*---------------------------------------------------------------------------------------
  process a force manager of the object
---------------------------------------------------------------------------------------*/
void RigidBody::processForce(ForceManager& forceManager)
{
    forceManager.processObject(*this);

    if (m_forceManager)
        m_forceManager->processObject(*this);
}

/*---------------------------------------------------------------------------------------
  Compute the world to body matrix transformation whose be used by the SceneObject
---------------------------------------------------------------------------------------*/
void RigidBody::computeWorldToBody()
{
    m_worldToBody.setRotation(m_rotMatrix);
    m_worldToBody.setTranslation(m_pos/* - m_RotMatrix * m_GravityCenter*/);
}

/*---------------------------------------------------------------------------------------
  Compute some variables
---------------------------------------------------------------------------------------*/
void RigidBody::computeVariables()
{
	// compute m_RotMatrix
    m_rotMatrix = m_rot.toMatrix3();

	// compute m_IWorld
    m_IWorld    = m_rotMatrix * m_IBody * m_rotMatrix.transposeTo();
	m_IWorldInv = m_IWorld.invert();

	// compute speed
    m_speed = m_P * getInvMass();

	// compute m_Omega
    m_omega = m_IWorld * m_L;
}

/*---------------------------------------------------------------------------------------
  Reset force and impulse accumulators
---------------------------------------------------------------------------------------*/
void RigidBody::resetAccumulators()
{
    m_forceAccumulator.set(0.f, 0.f, 0.f);
    m_torqueAccumulator.set(0.f, 0.f, 0.f);
    m_forceImpulseAccumulator.set(0.f, 0.f, 0.f);
    m_torqueImpulseAccumulator.set(0.f, 0.f, 0.f);
}

/*---------------------------------------------------------------------------------------
  Set the position and rotation of the rigid body to by in sync with those of the object
---------------------------------------------------------------------------------------*/
void RigidBody::synchronizeWithObject()
{
    if (!m_object)
		return;

	// set position and rotation
    setPosition(m_object->getAbsoluteMatrix().getTranslation());
    m_rot.fromMatrix3(m_object->getAbsoluteMatrix().getRotation());
}

/*---------------------------------------------------------------------------------------
  Set up mass and inertia to represent a sphere
---------------------------------------------------------------------------------------*/
void RigidBody::setUpMassSphere(Float density, Float radius)
{
    m_IBody.identity();
    setMass((4.0f/3.0f) * o3d::PI * radius*radius*radius * density);

    Float inertia = 0.4f * getMass() * radius*radius;

    m_IBody(0,0) = inertia;
    m_IBody(1,1) = inertia;
    m_IBody(2,2) = inertia;

	computeVariables();
}

/*---------------------------------------------------------------------------------------
  Set up mass and inertia to represent a box
---------------------------------------------------------------------------------------*/
void RigidBody::setUpMassBox(Float density, Vector3 dim)
{
    m_IBody.identity();
    Float mass = dim[X] * dim[Y] * dim[Z] * density;
    setMass(mass);

    m_IBody(0,0) = mass/12.f * (dim[Y]*dim[Y] + dim[Z]*dim[Z]);
    m_IBody(1,1) = mass/12.f * (dim[X]*dim[X] + dim[Z]*dim[Z]);
    m_IBody(2,2) = mass/12.f * (dim[X]*dim[X] + dim[Y]*dim[Y]);

	computeVariables();
}

/*---------------------------------------------------------------------------------------
  Set up mass and inertia to represent a box
---------------------------------------------------------------------------------------*/
void RigidBody::setUpMassBox(Float density, Float dimX, Float dimY, Float dimZ)
{
    setUpMassBox(density,Vector3(dimX, dimY, dimZ));
}

/*---------------------------------------------------------------------------------------
  Set up mass and inertia to represent a cylinder aligned on the Z axis
---------------------------------------------------------------------------------------*/
void RigidBody::setUpMassCylinder(
    Float density,
    Float height,
    Float radius,
	UInt32 axis)
{
    m_IBody.identity();

    Float M1 = o3d::PI * radius*radius*height*density;               // cylinder mass
    Float M2 = (4.0f/3.0f) * o3d::PI * radius*radius*radius*density; // total cap mass
	setMass(M1+M2);

    Float Ia = M1 * (0.25f*radius*radius + (1.0f/12.0f)*height*height) + M2*(0.4f*radius*radius + 0.5f*height*height);
    Float Ib = (M1*0.5f + M2*0.4f) * radius*radius;

	switch (axis)
	{
	case X:
		m_IBody(0,0) = Ib;
		m_IBody(1,1) = Ia;
		m_IBody(2,2) = Ia;
		break;
	case Y:
		m_IBody(0,0) = Ia;
		m_IBody(1,1) = Ib;
		m_IBody(2,2) = Ia;
		break;
	case Z:
		m_IBody(0,0) = Ia;
		m_IBody(1,1) = Ia;
		m_IBody(2,2) = Ib;
		break;
	default:
		m_IBody(0,0) = Ia;
		m_IBody(1,1) = Ia;
		m_IBody(2,2) = Ib;
		break;
	}

	computeVariables();
}

/*---------------------------------------------------------------------------------------
  Given mass parameters for some object, adjust them so the total mass is now NewMass.
  This is useful when using the above functions to set the mass parameters for certain
  objects (they take the object density, not the total mass).
---------------------------------------------------------------------------------------*/
void RigidBody::adjustMass(Float newMass)
{
    Float Scale = newMass * getInvMass();
    setMass(newMass);
	m_IBody.scale(Scale);

	computeVariables();
}

/*---------------------------------------------------------------------------------------
  Given mass parameters for some object, adjust them to represent the object displaced
  by Dir relative to the body frame.

  If the body is translated by 'a' relative to its point of reference, the new inertia
  about the point of reference is :

     I + mass*(crossmat(c)^2 - crossmat(c+a)^2)

  Where c is the existing center of mass and I is the old inertia
---------------------------------------------------------------------------------------*/
void RigidBody::translateMass(Vector3 dir)
{
	Matrix3 cCrossP = getGravityCenter().crossProductMatrix();
	cCrossP *= cCrossP;
	cCrossP.scale(getMass());

    Matrix3 caCrossP = (getGravityCenter() + dir).crossProductMatrix();
	caCrossP *= caCrossP;
	caCrossP.scale(getMass());

	setIBody(m_IBody + cCrossP + caCrossP);

	// ensure perfect symmetry
	m_IBody(1,0) = m_IBody(0,1);
	m_IBody(2,0) = m_IBody(0,2);
	m_IBody(2,1) = m_IBody(1,2);

	// adjust center of mass
    m_gravityCenter += dir;

	computeVariables();
}

/*---------------------------------------------------------------------------------------
  Given mass parameters for some object, adjust them to represent the object rotated
  by Rot relative to the body frame.

  If the body is rotated by `R' relative to its point of reference, the new inertia
  about the point of reference is :

  R * I * R'

  where I is the old inertia.
---------------------------------------------------------------------------------------*/
void RigidBody::rotateMass(Matrix3 rot)
{
    setIBody(rot * m_IBody * rot.transposeTo());

	// ensure perfect symmetry
	m_IBody(1,0) = m_IBody(0,1);
	m_IBody(2,0) = m_IBody(0,2);
	m_IBody(2,1) = m_IBody(1,2);

	// adjust center of mass
    m_gravityCenter = rot * getGravityCenter();

	computeVariables();
}

/*---------------------------------------------------------------------------------------
  serialisation
---------------------------------------------------------------------------------------*/
Bool RigidBody::writeToFile(OutStream &os)
{
    os << PHYSIC_RIGID_BODY;

    os   << m_isPhysic
         << m_speed
         << m_omega
		 << m_IBody
         << m_mass
         << m_gravityCenter
         << m_bounce
         << m_friction;

    if (!m_physicModel)
        O3D_ERROR(E_InvalidPrecondition("Physic model must be non null"));

    os << *m_physicModel;

    if (m_forceManager)
	{
        os << True
           << *m_forceManager;
	}
	else
	{
        os << False;
	}

	return True;
}

Bool RigidBody::readFromFile(InStream &is)
{
	UInt32 tmp;
    is >> tmp;

	if (tmp != PHYSIC_RIGID_BODY)
        O3D_ERROR(E_InvalidFormat("Invalid rigid body token"));

    is   >> m_isPhysic
         >> m_speed
         >> m_omega
		 >> m_IBody
         >> m_mass
         >> m_gravityCenter
         >> m_bounce
         >> m_friction;

    deletePtr(m_physicModel);

    m_physicModel = new PhysicModel((BaseObject*)this);
    is >> *m_physicModel;

	Bool bOOL = False;
    is >> bOOL;
	if (bOOL)
	{
        m_forceManager = new ForceManager((BaseObject*)this);
        is >> *m_forceManager;
	}

    if (m_object)
		synchronizeWithObject();

	resetAccumulators();

	// compute some variables
	setIBody(m_IBody);
    setOmega(m_omega);
    setMass(m_mass);
    setSpeed(m_speed);

	return True;
}


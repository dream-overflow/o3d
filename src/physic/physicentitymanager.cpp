/**
 * @file physicentitymanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/physicentitymanager.h"

#include "o3d/core/file.h"
#include "o3d/engine/object/facearray.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(PhysicEntity, PHYSIC_ENTITY, SceneEntity)
O3D_IMPLEMENT_DYNAMIC_CLASS1(PhysicEntityManager, PHYSIC_ENTITY_MANAGER, SceneTemplateManager<PhysicEntity>)

PhysicEntityManager::PhysicEntityManager(BaseObject *parent) :
    SceneTemplateManager<PhysicEntity>(parent),
    m_timeStep(0.03),
    m_maxSubSteps(3),
    m_time(0),
    m_timeStepRest(0),
    m_lastUpdate(-1),
    m_forceManager(nullptr),
    m_solverType(PHYSIC_SOLVER_EULER)
{
    m_forceManager.setParent(this);
}

PhysicEntityManager::~PhysicEntityManager()
{
}

// set the delta time between to physic updates
void PhysicEntityManager::setTimeStep(Double timeStep)
{
    m_timeStep = timeStep;
}

void PhysicEntityManager::setMaxSubSteps(UInt32 maxSubSteps)
{
    m_maxSubSteps = maxSubSteps;
}

void PhysicEntityManager::resetAccumulators()
{
    for (IT_TemplateManager it = begin() ; it != end() ; ++it) {
        (*it).second->resetAccumulators();
    }
}

void PhysicEntityManager::updatePhysicVerlet()
{
    IT_TemplateManager it;

    // force computations
    for (it = begin(); it != end(); ++it) {
        (*it).second->processForce(m_forceManager);
    }

    // @todo constraint manager

    // resolve deflexions and collisions
    m_collisionManager.resolveCollisions();

    // resolve physic
    for (it = begin(); it != end(); ++it) {
        (*it).second->updatePhysicVerlet(m_time, m_timeStep);
    }
}

void PhysicEntityManager::updatePhysicEuler()
{
    IT_TemplateManager it;

    // force computations
    for (it = begin(); it != end(); ++it) {
        (*it).second->processForce(m_forceManager);
    }

    // @todo constraint manager

    // resolve deflexions and collisions
    m_collisionManager.resolveCollisions();

    // resolve physic
    for (it = begin(); it != end(); ++it) {
        (*it).second->updatePhysicEuler(m_time, m_timeStep);
    }
}

void PhysicEntityManager::updatePhysicRK4()
{
    IT_TemplateManager it;

    for (UInt32 nStepRK4 = 1; nStepRK4 < 5; ++nStepRK4) {
        // force computations
        for (it = begin(); it != end(); ++it) {
            (*it).second->processForce(m_forceManager);
        }

        // @todo constraint manager

        // resolve deflexions and collisions only for the last step
        if (nStepRK4 == 4) {
            m_collisionManager.resolveCollisions();
        }

        // resolve physic
        for (it = begin(); it != end(); ++it) {
            (*it).second->updatePhysicRK4(m_time, m_timeStep, nStepRK4);
        }
    }
}

// add a physic entity and specify the collision test
Int32 PhysicEntityManager::addElement(PhysicEntity* pElt, Bool checkCollision)
{
    if (!pElt) {
        return -1;
    }

    if (checkCollision) {
		pElt->setCollision(True);
		// add the object to the collision manager
        m_collisionManager.addPhysicEntity(*pElt);
    } else {
		pElt->setCollision(False);
	}

	return TemplateManager<PhysicEntity>::addElement(pElt);
}

// update of the physic engine
void PhysicEntityManager::update()
{
	// process deferred objects deletion
	processDeferred();

	// time to compute
    Int64 curTime = System::getTime();

    if (m_lastUpdate < 0) {
		// first update
		resetAccumulators();
        m_lastUpdate = curTime;

        // start time
        m_time = (Double)curTime / (Double)System::getTimeFrequency();

		return;
	}

    Double deltaT = ((Double)(curTime - m_lastUpdate) / (Double)System::getTimeFrequency()) + m_timeStepRest;

    UInt32 nbrUpdate = (UInt32)(deltaT / m_timeStep);
    m_timeStepRest = deltaT - nbrUpdate * m_timeStep;

    // avoid spiral of death
    if (nbrUpdate > m_maxSubSteps) {
        nbrUpdate = m_maxSubSteps;
        O3D_WARNING("Physical integration update time step overflow");
    }

    m_lastUpdate = curTime;

	// physics computation
    for (UInt32 numUpdate = 0; numUpdate < nbrUpdate; ++numUpdate) {
        if (m_solverType == PHYSIC_SOLVER_VERLET) {
            updatePhysicVerlet();
        } else if (m_solverType == PHYSIC_SOLVER_EULER) {
			updatePhysicEuler();
        } else if (m_solverType == PHYSIC_SOLVER_RK4) {
            updatePhysicRK4();
        }

        // total time spent during integration
        m_time += m_timeStep;
    }

    // @todo is it interesting to interpolate with the remaining time ?
    // uses of slerp for quaterion and linear interpolation for vectors
    //interpolatePhysicXxx(m_timeStepRest / m_timeStep);

    //System::print("", String::print("%f %f", m_time, getLatency()));
}

Double PhysicEntityManager::getLatency() const
{
    Double lastTime = (Double)m_lastUpdate / (Double)System::getTimeFrequency();
    return o3d::abs(lastTime - m_time) > 0.0001 ? (lastTime - m_time) : 0;
}

//
// Serialization
//

Bool PhysicEntityManager::writeToFile(OutStream &os)
{
    BaseObject::writeToFile(os);

    os   << m_timeStep
         << m_maxSubSteps
         << (UInt32)m_solverType
         << m_forceManager;
		 //<< m_CollisionManager;

	return True;
}

Bool PhysicEntityManager::readFromFile(InStream &is)
{
    BaseObject::readFromFile(is);
	UInt32 tmp;

    is   >> m_timeStep
         >> m_maxSubSteps
		 >> tmp
         >> m_forceManager;
		 //>> m_CollisionManager;

    m_solverType = (PhysicSolverType)tmp;

	return True;
}

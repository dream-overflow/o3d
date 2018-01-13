/**
 * @file collisionmanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/collisionmanager.h"

#include "o3d/core/debug.h"

using namespace o3d;

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

// add a collision beetween two body
void CollisionManager::addCollision(
        RigidBody& rigidBody1, RigidBody& rigidBody2,
        Vector3 position, Vector3 normal,
        Vector3 edgeA, Vector3 edgeB,
        Bool isVertexFace)
{
    Collision* pCollision = nullptr;

    if (m_garbageCollector.empty()) {
		// create a new collision
		pCollision = new Collision;
    } else {
		// get it from the garbage collector
        pCollision = m_garbageCollector.front();
        m_garbageCollector.pop_front();
	}

	// init the collision
    pCollision->setBodyA(rigidBody1);
    pCollision->setBodyB(rigidBody2);
    pCollision->setPosition(position);
    pCollision->setNormal(normal);
    pCollision->setEdgeA(edgeA);
    pCollision->setEdgeB(edgeB);
    pCollision->setVertexFace(isVertexFace);
}

// get a collider beetween two entity
ABCCollider* CollisionManager::getCollider(PhysicEntity& entity0, PhysicEntity& Entity1)
{
    for (IT_ColliderList it = m_colliderList.begin() ; it != m_colliderList.end() ; ++it) {
		ABCCollider* pCollider = *it;

        if ((&pCollider->getCollideE0() == &entity0 && &pCollider->getCollideE1() == &Entity1) ||
            (&pCollider->getCollideE0() == &Entity1 && &pCollider->getCollideE1() == &entity0)) {
			return pCollider;
        }
	}

    return nullptr;
}

void CollisionManager::addPhysicEntity(PhysicEntity& entityA)
{
	IT_PhysicEntityList it;

    switch (entityA.getPhysicType())
    {
    //case PhysicEntity::PHYSIC_PARTICULE_MANAGER:
    //	for (it = m_entityList.begin() ; it != m_entityList.end() ; ++it) {
    //		PhysicEntity* entityB = *it;
    //		if (entityB->getPhysicType() == PhysicEntity::PHYSIC_DEFLECTOR) {
    //			m_ColliderList.push_back(new DeflectorCollider((ParticuleManager&)entityA,
    //															  (ABCDeflector&)*entityB));
    //      }
	//	}
	//	break;

    case PhysicEntity::PHYSIC_RIGID_BODY:
        for (it = m_entityList.begin() ; it != m_entityList.end() ; ++it) {
            PhysicEntity* entityB = *it;
            if (entityB->getPhysicType() == PhysicEntity::PHYSIC_RIGID_BODY) {
                m_colliderList.push_back(new RigidBodyCollider(
                                             (RigidBody&)entityA,
                                             (RigidBody&)*entityB));
            }
		}
        break;

    //case PhysicEntity::PHYSIC_DEFLECTOR:
    //	for (it = m_entityList.begin() ; it != m_entityList.end() ; ++it) {
    //		PhysicEntity* entityB = *it;
    //		if (entityB->getPhysicType() == PhysicEntity::PHYSIC_PARTICULE_MANAGER) {
    //			m_ColliderList.push_back(new DeflectorCollider((ParticuleManager&)*entityA,
    //															  (ABCDeflectorABC&)entityB));
    //      }
	//	}
	//	break;

    default:
		O3D_ERROR(E_InvalidParameter("Unknown physic entity type"));
        break;
    }

    m_entityList.push_back(&entityA);
}

void CollisionManager::resolveCollisions()
{
	// detect collision on each pair of objects involved
    for (IT_ColliderList itCollider = m_colliderList.begin() ; itCollider != m_colliderList.end() ; ++itCollider) {
		(*itCollider)->performCollisionDetection(*this);
    }

	// resolve each collision
    for (IT_CollisionList itCollision = m_collisionList.begin() ; itCollision != m_collisionList.end() ; ++itCollision) {
		Collision* pCollision = *itCollision;
		pCollision->resolve();

		// re-cycle the collision
        m_garbageCollector.push_back(pCollision);
	}

    m_collisionList.clear();
}

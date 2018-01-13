/**
 * @file collisionmanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_COLLISIONMANAGER_H
#define _O3D_COLLISIONMANAGER_H

#include "o3d/core/memorydbg.h"
#include "o3d/physic/collision.h"
#include "o3d/physic/physicentity.h"
#include "o3d/physic/rigidbodycollider.h"
#include "o3d/physic/deflectorcollider.h"

namespace o3d {

/**
 * @brief Physic collision manager for physic entities.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-06-01
 * Manage all collision between the rigid bodies of an PhysicentityManager.
 * The collision are stored in a linked list. It also use a garbage collector
 * to avoid allocating/de-allocating too much memory.
 */
class O3D_API CollisionManager
{
public:

	//! default constructor
	CollisionManager();

	//! destructor
	virtual ~CollisionManager();

	//! resolve all collisions
	void resolveCollisions();

	//! add a collision between two body
    void addCollision(
            RigidBody& rigidBody1, RigidBody& rigidBody2,
            Vector3 position, Vector3 normal,
            Vector3 edgeA, Vector3 edgeB, Bool isVertexFace);

	//! get a collider between two entity
    ABCCollider* getCollider(PhysicEntity& entity0, PhysicEntity& entity1);

	//! add a physic entity to the collision manager
	void addPhysicEntity(PhysicEntity& entity);

protected:

	T_ColliderList m_colliderList;      //!< list of pairs of objects to test
    T_PhysicEntityList m_entityList;    //!< list of all objects involved in collision detection

	T_CollisionList m_collisionList;    //!< the list of collision to resolve
	T_CollisionList m_garbageCollector; //!< a list of collision to re-use
};

} // namespace o3d

#endif // _O3D_COLLISIONMANAGER_H

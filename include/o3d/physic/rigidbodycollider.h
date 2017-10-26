/**
 * @file rigidbodycollider.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_RIGIDBODYCOLLIDER_H
#define _O3D_RIGIDBODYCOLLIDER_H

#include "o3d/core/memorydbg.h"
#include "abccollider.h"
#include "rigidbody.h"

namespace o3d {

class CollisionManager;

/**
 * @brief Track the collision between the pair of rigid body
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-06-01
 */
class O3D_API RigidBodyCollider : public ABCCollider
{
public:

	//! default constructor
    RigidBodyCollider(RigidBody& body0, RigidBody& body1);

	//! destructor
	virtual ~RigidBodyCollider();

    //! perform a collision detection
    virtual void performCollisionDetection(CollisionManager& collisionManager);

	virtual PhysicEntity& getCollideE0();
	virtual PhysicEntity& getCollideE1();

    inline RigidBody& getRigidBody0() { return *m_body0; }
    inline RigidBody& getRigidBody1() { return *m_body1; }

protected:

    RigidBody* m_body0;   //!< first rigid body
    RigidBody* m_body1;   //!< second rigid body

    PhysicEntity *m_collideE0;
    PhysicEntity *m_collideE1;
};

} // namespace o3d

#endif // _O3D_RIGIDBODYCOLLIDER_H


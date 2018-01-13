/**
 * @file rigidbodycollider.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/physic/precompiled.h"
#include "o3d/physic/rigidbodycollider.h"

using namespace o3d;

RigidBodyCollider::RigidBodyCollider(RigidBody& body0, RigidBody& body1) :
    ABCCollider(),
    m_body0(&body0),
    m_body1(&body1)
{
    // @todo
}

RigidBodyCollider::~RigidBodyCollider()
{

}

void RigidBodyCollider::performCollisionDetection(CollisionManager& collisionManager)
{
    // @todo
}

PhysicEntity& RigidBodyCollider::getCollideE0()
{
    return *m_collideE0;
}

PhysicEntity& RigidBodyCollider::getCollideE1()
{
    return *m_collideE1;
}

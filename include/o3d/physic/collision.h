/**
 * @file collision.h
 * @brief Resolve a collision beetween to RigidBody
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-06-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_COLLISION_H
#define _O3D_COLLISION_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/vector3.h"
#include <list>

namespace o3d {

class RigidBody;

/**
 * @brief Resolve a collision beetween two RigidBody objects.
 */
class O3D_API Collision
{
public:

	//! default constructor
	Collision();

	//! destructor
	virtual ~Collision();

	//! Add an impulse on both rigid body to avoid penetration
	void resolve();

    //! set the A body
    inline void setBodyA(RigidBody& bodyA) { m_pBodyA = &bodyA; }
    //! get the A body
	inline RigidBody* getBodyA() { return m_pBodyA; }

    //! set the B body
    inline void setBodyB(RigidBody& bodyB) { m_pBodyB = &bodyB; }
    //! get the B body
	inline RigidBody* getBodyB() { return m_pBodyB; }

    //! set the collision point
    inline void setPosition(Vector3& position) { m_position = position; }
    //! get the collision point
    inline const Vector3& getPosition() const { return m_position; }
    //! get the collision point
    inline  Vector3& getPosition() { return m_position; }

    //! set the normal at collision
    inline void setNormal(Vector3& normal) { m_normal = normal; }
    //! get the normal at collision
    inline const Vector3& getNormal() const { return m_normal; }
    //! get the normal at collision
    inline Vector3& getNormal() { return m_normal; }

    //! set the edge of A colliding
    inline void setEdgeA(Vector3& edgeA) { m_edgeA = edgeA; }
    //! get the edge of A colliding
    inline const Vector3& getEdgeA() const { return m_edgeA; }
    //! get the edge of A colliding
    inline Vector3& getEdgeA() { return m_edgeA; }

    //! set the edge of B colliding
    inline void setEdgeB(Vector3& edgeB) { m_edgeB = edgeB; }
    //! get the edge of B colliding
    inline const Vector3& getEdgeB() const { return m_edgeB; }
    //! get the edge of B colliding
    inline Vector3& getEdgeB() { return m_edgeB; }

    inline void setVertexFace(Bool isvertexFace) { m_isvertexFace = isvertexFace; }
    inline Bool isVertexFace() { return m_isvertexFace; }

protected:

    RigidBody* m_pBodyA;   //!< first rigid body involved
    RigidBody* m_pBodyB;   //!< second rigid body involved

    Vector3 m_position;    //!< position of the collision point, in world frame
    Vector3 m_normal;      //!< normal at collision, pointing from A to B.
                           //!< for edge/edge contact : m_Normal = m_EdgeA ^ m_EdgeB
    Vector3 m_edgeA;       //!< for edge/edge collision : edge of A colliding, in world frame
    Vector3 m_edgeB;       //!< for edge/edge collision : edge of B colliding, in world frame

    Bool m_isvertexFace;   //!< is the collision a vertex/face one ? if not, it is an edge/edge one
};

typedef std::list<Collision*> T_CollisionList;
typedef T_CollisionList::iterator IT_CollisionList;
typedef T_CollisionList::const_iterator CIT_CollisionList;

} // namespace o3d

#endif // _O3D_COLLISION_H

/**
 * @file collision.h
 * @brief Resolve a collision beetween to RigidBody
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
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

//---------------------------------------------------------------------------------------
//! @class Collision
//-------------------------------------------------------------------------------------
//! Resolve a collision beetween to RigidBody
//---------------------------------------------------------------------------------------
class O3D_API Collision
{
public:

	//! default constructor
	Collision();

	//! destructor
	virtual ~Collision();

	//! Add an impulse on both rigid body to avoid penetration
	void resolve();

	//! set/get the A body
	inline void setBodyA(RigidBody& BodyA) { m_pBodyA = &BodyA; }
	inline RigidBody* getBodyA() { return m_pBodyA; }

	//! set/get the B body
	inline void setBodyB(RigidBody& BodyB) { m_pBodyB = &BodyB; }
	inline RigidBody* getBodyB() { return m_pBodyB; }

	//! set/get the collision point
	inline void setPosition(Vector3& Position) { m_Position = Position; }
	inline const Vector3& getPosition()const { return m_Position; }
	inline       Vector3& getPosition()      { return m_Position; }

	//! set/get the normal at collision
	inline void setNormal(Vector3& Normal) { m_Normal = Normal; }
	inline const Vector3& getNormal()const { return m_Normal; }
	inline       Vector3& getNormal()      { return m_Normal; }

	//! set/get the edge of A colliding
	inline void setEdgeA(Vector3& EdgeA) { m_EdgeA = EdgeA; }
	inline const Vector3& getEdgeA()const { return m_EdgeA; }
	inline       Vector3& getEdgeA()      { return m_EdgeA; }

	//! set/get the edge of B colliding
	inline void setEdgeB(Vector3& EdgeB) { m_EdgeB = EdgeB; }
	inline const Vector3& getEdgeB()const { return m_EdgeB; }
	inline       Vector3& getEdgeB()      { return m_EdgeB; }

	inline void setVertexFace(Bool IsvertexFace) { m_IsvertexFace = IsvertexFace; }
	inline Bool isVertexFace() { return m_IsvertexFace; }

protected:

	RigidBody* m_pBodyA;       //!< first rigid body involved
	RigidBody* m_pBodyB;       //!< second rigid body involved

	Vector3 m_Position;        //!< position of the collision point, in world frame
	Vector3 m_Normal;          //!< normal at collision, pointing from A to B.
	                           //!< for edge/edge contact : m_Normal = m_EdgeA ^ m_EdgeB
	Vector3 m_EdgeA;           //!< for edge/edge collision : edge of A colliding, in world frame
	Vector3 m_EdgeB;           //!< for edge/edge collision : edge of B colliding, in world frame

	Bool m_IsvertexFace;   //!< is the collision a vertex/face one ? if not, it is an edge/edge one
};

typedef std::list<Collision*> T_CollisionList;
typedef T_CollisionList::iterator IT_CollisionList;
typedef T_CollisionList::const_iterator CIT_CollisionList;

} // namespace o3d

#endif // _O3D_COLLISION_H


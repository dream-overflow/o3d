/**
 * @file abccollider.h
 * @brief A collider track collision between a pair of object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-06-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ABCCOLLIDER_H
#define _O3D_ABCCOLLIDER_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/templatemanager.h"
#include "o3d/core/vector3.h"
#include "physicentity.h"

#include <list>

namespace o3d {

class CollisionManager;

/**
 * @brief A collider track collision between a pair of object
 */
class O3D_API ABCCollider
{
public:

	//! default constructor
	ABCCollider();

	//! destructor
	virtual ~ABCCollider();

    //! perform a collision detection
	virtual void performCollisionDetection(CollisionManager& CollisionManager) = 0;

	virtual PhysicEntity& getCollideE0() = 0;
	virtual PhysicEntity& getCollideE1() = 0;

	//! Get collision perform activity
    inline Bool isActive() const { return m_isActive; }
	//! Enable collision perform activity
    inline void enable()  { m_isActive = True;  }
	//! Disable collision perform activity
    inline void disable() { m_isActive = False; }

protected:

    Bool m_isActive;  //!< is check collision for this pair
};

typedef std::list<ABCCollider*> T_ColliderList;
typedef T_ColliderList::iterator IT_ColliderList;
typedef T_ColliderList::const_iterator CIT_ColliderList;

} // namespace o3d

#endif // _O3D_ABCCOLLIDER_H

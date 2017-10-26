/**
 * @file physicentity.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PHYSICENTITY_H
#define _O3D_PHYSICENTITY_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/baseobject.h"
#include "o3d/core/string.h"
#include "o3d/engine/scene/sceneentity.h"
#include <list>

namespace o3d {

class ForceManager;

/**
 * @brief The type of physic computation and base of physic element
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-05-30
 */
class O3D_API PhysicEntity : public SceneEntity
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(PhysicEntity)

	//! the physic entity type
	enum PhysicEntityType
	{
        PHYSIC_UNDEFINED,
        PHYSIC_PARTICULE_MANAGER,
        PHYSIC_RIGID_BODY,
        PHYSIC_DEFLECTOR
	};

    //! constructor
	PhysicEntity(BaseObject *pParent) :
		SceneEntity(pParent),
        m_type(PHYSIC_UNDEFINED),
        m_isCollision(True) {}

	//! is collision detection on this entity
    inline void setCollision(Bool collision) { m_isCollision = collision; }
    inline Bool isCollision()const { return m_isCollision; }

	//! get the entity type
	inline PhysicEntityType getPhysicType()const { return m_type; }

	//! update physic with euler method
    virtual void updatePhysicEuler(Double t, Double dt) = 0;
	//! update physic with verlet method
    virtual void updatePhysicVerlet(Double t, Double dt) = 0;
    //! update physic using runge-kutta 4 method
    virtual void updatePhysicRK4(Double t, Double dt, UInt32 numStepRK4) = 0;

	//! process the a force on the entity
    virtual void processForce(ForceManager& forceManager) = 0;

	//! reset the force accumaltors
	virtual void resetAccumulators() = 0;

protected:

	PhysicEntityType m_type;  //!< physic entity type of this object

    Bool m_isCollision;   //!< is this entity take the collision
};

typedef std::list<PhysicEntity*> T_PhysicEntityList;
typedef T_PhysicEntityList::iterator IT_PhysicEntityList;
typedef T_PhysicEntityList::const_iterator CIT_PhysicEntityList;

} // namespace o3d

#endif // _O3D_PHYSICENTITY_H


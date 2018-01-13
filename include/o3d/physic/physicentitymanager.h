/**
 * @file physicentitymanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PHYSICENTITYMANAGER_H
#define _O3D_PHYSICENTITYMANAGER_H

#include "o3d/core/memorydbg.h"
#include "o3d/engine/scene/scenetemplatemanager.h"
#include "o3d/physic/forcemanager.h"
#include "o3d/physic/collisionmanager.h"
#include "o3d/engine/physicmanager.h"

namespace o3d {

/**
 * @brief Physic entity manager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-06-01
 */
class O3D_API PhysicEntityManager :
        public SceneTemplateManager<PhysicEntity>,
        public PhysicManager
{
public:

	O3D_DECLARE_CLASS(PhysicEntityManager)

	enum PhysicSolverType
	{
		PHYSIC_SOLVER_EULER,
        PHYSIC_SOLVER_VERLET,
        PHYSIC_SOLVER_RK4
	};

	//! default constructor
	PhysicEntityManager(BaseObject *pParent);
	//! destructor
	virtual ~PhysicEntityManager();

	//! add a physic entity and specify the collision test
    Int32 addElement(PhysicEntity* pElt, Bool checkCollision = True);

	//! make an update
    virtual void update() override;

    //! get the effective latency of the physic integration and current time
    Double getLatency() const;

    //! get the delta time between to physic updates
    inline Double getTimeStep() { return m_timeStep; }
    //! set the delta time between to physic updates
    void setTimeStep(Double timeStep);

     //! get the max number of physics iteration per update (default is 3, 0 mean unlimited)
    inline UInt32 getMaxSubSteps() { return m_maxSubSteps; }
    //! set the max number of physics iteration per update (default is 3, 0 mean unlimited)
    //! @note Take care using this parameter when unlimited can cause a spiral of death
    //! in some configurations
    void setMaxSubSteps(UInt32 maxSubSteps);

	//! set/get the physic solver type
    inline void setSolverType(PhysicSolverType SolverType) { m_solverType = SolverType; }
    PhysicSolverType getSolverType()const { return m_solverType; }

	//! get the force manager
    inline const ForceManager& getForceManager()const { return m_forceManager; }
    inline ForceManager& getForceManager() { return m_forceManager; }

	//! get the collision manager
    inline const CollisionManager& getCollisionManager()const { return m_collisionManager; }
    inline CollisionManager& getCollisionManager() { return m_collisionManager; }

	//! get a collision beetween two physic entity
    inline ABCCollider* getCollider(PhysicEntity& Entity1, PhysicEntity& Entity2)
	{
        return m_collisionManager.getCollider(Entity1,Entity2);
	}

	//! reset all entity accumulators
    void resetAccumulators();

	//! update all entity with Verlet solver
    void updatePhysicVerlet();

	//! update all entity with Euler solver
    void updatePhysicEuler();

    //! update all entity with runge-kutta 4 solver
    void updatePhysicRK4();

	//! serialisation
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

    Double m_timeStep;           //!< fixed delta time between two physic updates (1/F)
    UInt32 m_maxSubSteps;        //!< maximal number of sub steps per update (default is 3)

    Double m_time;               //!< total of time spent in physics integrations
    Double m_timeStepRest;       //!< the fractional time step that has not been used during last update
    Int64 m_lastUpdate;          //!< time of the last update

    ForceManager m_forceManager;    //!< the force manager common to all entities
    //ConstraintManager m_constraintManager; //!< manage the constraints between rigid body TODO

    CollisionManager m_collisionManager; //!< check collision between all pair of object

    PhysicSolverType m_solverType;   	 //!< the used solver
};

} // namespace o3d

#endif // _O3D_PHYSICENTITYMANAGER_H

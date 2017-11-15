/**
 * @file abcforce.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ABCFORCE_H
#define _O3D_ABCFORCE_H

#include "o3d/core/memorydbg.h"
#include "o3d/engine/scene/sceneobject.h"
#include "o3d/core/smartcounter.h"
#include "o3d/core/file.h"

#include "physictype.h"

namespace o3d {

/**
 * @brief Base class for all the forces
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2016-02-19
 */
class O3D_API ABCForce : public SceneEntity
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(ABCForce)

	//! Default constructor
	ABCForce(BaseObject *pParent) :
        SceneEntity(pParent) {}

    //-----------------------------------------------------------------------------------
    // Force activity
    //-----------------------------------------------------------------------------------

    //! Enable the force.
    inline void enable() { m_activity = True; }

    //! Disable the force.
    inline void disable() { m_activity = False; }

    //! Is the object active.
    inline Bool getActivity() const { return m_activity; }

    //! Set the object activity.
    inline void setActivity(Bool state)
    {
        if (state)
            enable();
        else
            disable();
    }

    //! Toggle the object activity.
    inline Bool toggleActivity()
    {
        if (getActivity())
        {
            disable();
            return False;
        }
        else
        {
            enable();
            return True;
        }
    }

    //-----------------------------------------------------------------------------------
    // Processing
    //-----------------------------------------------------------------------------------

	//! process the force on the object
	virtual void processObject(class RigidBody& rigidBody) = 0;

	//! process the force on the particle
	virtual void processParticule(class Particule& particule) {} // = 0;

protected:

    Bool m_activity;
};

} // namespace o3d

#endif // _O3D_FORCE_H


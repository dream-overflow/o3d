/**
 * @file physictype.h
 * @brief Enumerations for physic.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2012-12-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PHYSICTYPE_H
#define _O3D_PHYSICTYPE_H

namespace o3d {

enum PhysicObjectType
{
    PHYSIC_MANAGER = 0x04000000,   //!< physic manager

    PHYSIC_RIGID_BODY,             //!< a rigid body
    PHYSIC_MODEL,                  //!< a physic model for rigid body

    PHYSIC_ENTITY_MANAGER,         //!< physic entity manager
	PHYSIC_ENTITY,

    PHYSIC_FORCE_MANAGER = 0x04010000, //!<  physic force manager
	PHYSIC_FORCE,                      //!< a base force
	PHYSIC_FORCE_GRAVITY,              //!< a gravity force
	PHYSIC_FORCE_SPRING,               //!< a spring force
	PHYSIC_FORCE_MAGNETIC,             //!< a magnetic force
	PHYSIC_FORCE_CONSTANT,             //!< a simple constant force
	PHYSIC_FORCE_DAMPING               //!< a damping force
};

} // namespace o3d

#endif // _O3D_PHYSICTYPE_H

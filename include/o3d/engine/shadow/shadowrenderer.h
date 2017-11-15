/**
 * @file shadowrenderer.h
 * @brief Shadow renderer.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-01-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SHADOWRENDERER_H
#define _O3D_SHADOWRENDERER_H

#include "shadowable.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class ShadowRenderer
//-------------------------------------------------------------------------------------
//! Shadow renderer interface. Process the light in way to render the visibility
//! manager of the related scene with shadow and lighting.
//---------------------------------------------------------------------------------------
class O3D_API ShadowRenderer
{
public:

	O3D_DECLARE_INTERFACE(ShadowRenderer)

	//! Process rendering of shadow caster accordings to a specified light and the scene.
	virtual void processLight(Light *light) = 0;

	//! Define the light management policy
	//virtual void setLightPolicy(LightPolicy *policy) = 0;

	//! Get the light management policy.
	//virtual LightPolicy* getLightPolicy() = 0;

	//! Set the policy level to use. How are managed lights and object according
	//! the current policy and a specified level.
	virtual void setPolicyLevel(UInt32 level) = 0;

	//! Get the current policy level to use.
	virtual UInt32 getPolicyLevel() const = 0;
};

} // namespace o3d

#endif // _O3D_SHADOWRENDERER_H


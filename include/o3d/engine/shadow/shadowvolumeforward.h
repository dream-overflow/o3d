/**
 * @file shadowvolumeforward.h
 * @brief Shadow volume rendering.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-01-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SHADOWVOLUMEFORWARD_H
#define _O3D_SHADOWVOLUMEFORWARD_H

#include "shadowable.h"
#include "shadowrenderer.h"
#include "../scene/scenedrawer.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Shadow volume rendering using forward light.
 */
class O3D_API ShadowVolumeForward :
        public SceneDrawer,
        public ShadowRenderer
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(ShadowVolumeForward)

	//! Constructor. Take a parent objets.
	ShadowVolumeForward(BaseObject *parent);

	//! Process rendering of shadow caster accordings to a specified light and the scene.
	virtual void processLight(Light *light);

	//! Draw process.
    virtual void draw(ViewPort *viewPort);

	//! Process a simpler draw for the picking pass.
    virtual void drawPicking(ViewPort *viewPort);

	//! Define the light management policy
	//virtual void setLightPolicy(LightPolicy *policy) = 0;

	//! Get the light management policy.
	//virtual LightPolicy* getLightPolicy() = 0;

	//! Set the policy level to use. How are managed lights and object according
	//! the current policy and a specified level.
	virtual void setPolicyLevel(UInt32 level);

	//! Get the current policy level to use.
	virtual UInt32 getPolicyLevel() const;
};

} // namespace o3d

#endif // _O3D_SHADOWVOLUMEFORWARD_H

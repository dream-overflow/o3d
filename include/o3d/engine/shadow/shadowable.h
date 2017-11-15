/**
 * @file shadowable.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SHADOWABLE_H
#define _O3D_SHADOWABLE_H

#include "o3d/core/string.h"
#include "../shader/shadable.h"
#include "../drawinfo.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Give the capacities to cast shadows.
 * @date 2005-10-30
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API Shadowable
{
public:

	O3D_DECLARE_INTERFACE(Shadowable)

	//! Shadow receiver technique.
	enum ShadowTechnique
	{
		SHADOW_NONE = 0,              //!< No shadow are rendered.
		SHADOW_VOLUME = SHADOW_NONE,  //!< Use stencil shadow volume, like no shadow, use stencil.
		SHADOW_MAP_HARD = 1,          //!< Use hard shadow map.
		SHADOW_MAP_SOFT = 2           //!< Use soft shadow map.
	};

	//! Number of shadow receiver techniques.
	static const UInt32 NUM_SHADOW_TECHNIQUE = 3;

	//! Enable the cast of shadow.
	virtual void enableShadowCast() = 0;

	//! Disable the cast of shadow.
	virtual void disableShadowCast() = 0;

	//! Get the shadow cast states.
	virtual Bool getShadowCast() const = 0;

	//! Project the silhouette according to a specific light.
	virtual void projectSilhouette(const DrawInfo &drawInfo) = 0;
};

} // namespace o3d

#endif // _O3D_SHADOWABLE_H


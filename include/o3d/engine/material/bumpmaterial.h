/**
 * @file bumpmaterial.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BUMPMATERIAL_H
#define _O3D_BUMPMATERIAL_H

#include "lambertmaterial.h"

namespace o3d {

/**
 * @brief Material using a bump map and per pixel illumination.
 * @date 2010-02-17
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * Illumination shader, based on per pixel lighting. It supports forward lighting, and
 * deferred shading. The shadable must provides a tangent space
 * (normal, tangent, bitangente) arrays.
 * The picking mode is not supported by this material, uses PickingMaterial for that.
 * @note u_diffuse and u_specular if there is respectively no diffuse map or no specular
 *       map, are premultiplied by the light diffuse and light specular. That mean in the
 *       shader there is no need to performs these products.
 */
class O3D_API BumpMaterial : public LambertMaterial
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(BumpMaterial)

	//! Default constructor.
	BumpMaterial(BaseObject *parent);

	//! Virtual destructor.
	virtual ~BumpMaterial();

	//! Check if the material is supported by the hardware.
    virtual Bool isMaterialSupported() const override;

	//! Initialize material shader according to a specific MaterialPass setting,
	//! and shadable vertex mode.
	//! @param initMode Accept LIGHTING or DEFERRED mode.
	virtual void initialize(
			InitMode initMode,
			MaterialPass &materialPass,
            Shadable &shadable) override;

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Shadable object rendering for ambient pass.
	//! @warning Not supported for this mode.
	virtual void processAmbient(
			Shadable &object,
			const DrawInfo &drawInfo,
            const MaterialPass &materialPass) override;

	//! Shadable object rendering for picking.
	//! @warning Not supported for this mode.
	virtual void processPicking(
			Shadable &object,
			Pickable &pickable,
			const DrawInfo &drawInfo,
            const MaterialPass &materialPass) override;

	//! Shadable object rendering for lighting and shadow pass.
	//! @param object Shadable object to render.
	//! @param processMode Rendering mode.
	//! @param materialPass Material pass object that contain parameters.
	virtual void processLighting(
			Shadable &object,
			Shadowable &shadowable,
			const DrawInfo &drawInfo,
            const MaterialPass &materialPass) override;

	//! Shadable object rendering for deferred diffuse pass.
	//! @param object Shadable object to render.
	//! @param processMode Rendering mode.
	//! @param materialPass Material pass object that contain parameters.
	virtual void processDeferred(
			Shadable &object,
			const DrawInfo &drawInfo,
            const MaterialPass &materialPass) override;

protected:

	UInt32 a_tangent[3];
	UInt32 a_bitangent[3];

	Int32 u_bumpMap[3];

    virtual void getCommonLoc(ShaderInstance &shaderInstance, Int32 dest) override;
};

} // namespace o3d

#endif // _O3D_BUMPMATERIAL_H

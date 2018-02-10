/**
 * @file ambientmaterial.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_AMBIENTMATERIAL_H
#define _O3D_AMBIENTMATERIAL_H

#include "material.h"

namespace o3d {

/**
 * @brief Material using the simple ambient component or ambient.
 * @date 2010-01-19
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 *
 * First it look for an ambient map, if not look for a diffuse map.
 * If there is a diffuse map it use it as ambient map and compute :
 *    - ambient map * ambient color * global ambient.
 * If there is an ambient map it compute :
 *    - ambient map * global ambient.
 * If these is no map it compute :
 *    - ambient color * global ambient.
 *
 * The global ambient is took from scene global ambient.
 * It supports any types of vertex mode (static mesh, rigging, skinning...).
 *
 * @note This material is not supported during a deferred pass.
 */
class O3D_API AmbientMaterial : public Material
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(AmbientMaterial)

	//! Default constructor.
	AmbientMaterial(BaseObject *parent);

	//! Virtual destructor.
	virtual ~AmbientMaterial();

	//! Check if the material is supported by the hardware.
    virtual Bool isMaterialSupported() const override;

	//! Initialize material shader according to a specific MaterialPass setting,
	//! and shadable vertex mode.
	//! @param initMode Only accept AMBIENT.
	virtual void initialize(
			InitMode initMode,
			MaterialPass &materialPass,
            Shadable &shadable) override;

	//! Release initialized data.
    virtual void release() override;

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Shadable object rendering for ambient pass.
	//! @param object Shadable object to render.
	//! @param processMode Rendering mode.
	//! @param materialPass Material pass object that contain parameters.
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
	//! @warning Not supported for this mode.
	virtual void processLighting(
			Shadable &object,
			Shadowable &shadowable,
			const DrawInfo &drawInfo,
            const MaterialPass &materialPass) override;

	//! Shadable object rendering for deferred diffuse pass.
	//! @warning Not supported for this mode.
	virtual void processDeferred(
			Shadable &object,
			const DrawInfo &drawInfo,
            const MaterialPass &materialPass) override;

protected:

	Int32 u_modelViewProjectionMatrix;

    Int32 u_bonesMatrixArray; //!< object space bones matrices

    Int32 m_materialMap;  //!< 0 ambient, 1 diffuse, -1 none
    Int32 u_ambientMap;
    Int32 u_ambient;

	UInt32 a_rigging;
	UInt32 a_skinning;
	UInt32 a_weighting;

	ShaderInstance m_shaderInstance;
};

} // namespace o3d

#endif // _O3D_AMBIENTMATERIAL_H

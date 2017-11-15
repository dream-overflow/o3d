/**
 * @file fontmaterial.h
 * @brief Font material.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-02-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FONTMATERIAL_H
#define _O3D_FONTMATERIAL_H

#include "o3d/engine/material/material.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class FontMaterial
//-------------------------------------------------------------------------------------
//! Font material.
//---------------------------------------------------------------------------------------
class O3D_API FontMaterial : public Material
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(FontMaterial)

	//! Default constructor.
	FontMaterial(BaseObject *parent);

	//! Virtual destructor.
	virtual ~FontMaterial();

	//! Check if the material is supported by the hardware.
	virtual Bool isMaterialSupported() const;

	//! Initialize material shader according to a specific MaterialPass setting,
	//! and shadable vertex mode.
	//! @param initMode Accept LIGHTING or DEFERRED mode.
	virtual void initialize(
			InitMode initMode,
			MaterialPass &materialPass,
			Shadable &shadable);

	//! Release initialized data.
	virtual void release();

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
			const MaterialPass &materialPass);

	//! Shadable object rendering for picking.
	//! @warning Not supported for this mode.
	virtual void processPicking(
			Shadable &object,
			Pickable &pickable,
			const DrawInfo &drawInfo,
			const MaterialPass &materialPass);

	//! Shadable object rendering for lighting and shadow pass.
	//! @warning Not supported for this mode.
	virtual void processLighting(
			Shadable &object,
			Shadowable &shadowable,
			const DrawInfo &drawInfo,
			const MaterialPass &materialPass);

	//! Shadable object rendering for deferred diffuse pass.
	//! @param object Shadable object to render.
	//! @param processMode Rendering mode.
	//! @param materialPass Material pass object that contain parameters.
	virtual void processDeferred(
			Shadable &object,
			const DrawInfo &drawInfo,
			const MaterialPass &materialPass);

protected:

	UInt32 a_texCoords1;

	Int32 u_modelViewProjectionMatrix;

	Int32 u_ambient;
	Int32 u_ambientMap;

	ShaderInstance m_shaderInstance;
};

} // namespace o3d

#endif // _O3DFONTMATERIAL_H


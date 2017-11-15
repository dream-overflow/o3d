/**
 * @file depthpassmaterial.h
 * @brief Simple material that render no output color.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-01-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DEPTHPASSMATERIAL_H
#define _O3D_DEPTHPASSMATERIAL_H

#include "material.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class DepthPassMaterial
//-------------------------------------------------------------------------------------
//! Simple material that render no output color.
//---------------------------------------------------------------------------------------
class O3D_API DepthPassMaterial : public Material
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(DepthPassMaterial)

	//! Default constructor.
	DepthPassMaterial(BaseObject *parent);

	//! Virtual destructor.
	virtual ~DepthPassMaterial();

	//! Check if the material is supported by the hardware.
	virtual Bool isMaterialSupported() const;

	//! Initialize material shader according to a specific MaterialPass setting,
	//! and shadable vertex mode.
	//! @param initMode Only accept AMBIENT.
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
	//! @warning Not supported for this mode.
	virtual void processDeferred(
			Shadable &object,
			const DrawInfo &drawInfo,
			const MaterialPass &materialPass);

protected:

	UInt32 a_texCoords1;

	UInt32 a_rigging;
	UInt32 a_skinning;
	UInt32 a_weighting;

	Bool m_opacityMap;
	Int32 u_opacityMap;

	Int32 u_modelViewProjectionMatrix;
	Int32 u_bonesMatrixArray;

	ShaderInstance m_shaderInstance;
};

} // namespace o3d

#endif // _O3D_DEPTHPASSMATERIAL_H


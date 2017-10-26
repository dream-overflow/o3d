/**
 * @file pickingmaterial.h
 * @brief Material dedicated to the picking pass.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2010-01-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PICKINGMATERIAL_H
#define _O3D_PICKINGMATERIAL_H

#include "material.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class PickingMaterial
//-------------------------------------------------------------------------------------
//! Material dedicated to the picking pass.
//---------------------------------------------------------------------------------------
class O3D_API PickingMaterial : public Material
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(PickingMaterial)

	//! Default constructor.
	PickingMaterial(BaseObject *parent);

	//! Virtual destructor.
	virtual ~PickingMaterial();

	//! Check if the material is supported by the hardware.
	virtual Bool isMaterialSupported() const;

	//! Initialize material shader according to a specific MaterialPass setting,
	//! and shadable vertex mode.
	//! @param initMode Only accept PICKING.
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
	//! @warning Not supported for this mode.
	virtual void processAmbient(
			Shadable &object,
			const DrawInfo &drawInfo,
			const MaterialPass &materialPass);

	//! Shadable object rendering for picking.
	//! @param object Shadable object to render.
	//! @param pickable Pickable object to render.
	//! @param processMode Rendering mode.
	//! @param materialPass Material pass object that contain parameters.
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

	UInt32 a_rigging;
	UInt32 a_skinning;
	UInt32 a_weighting;
	UInt32 a_texCoords1;

	Bool m_opacityMap;
	Int32 u_opacityMap;
	Int32 u_pickingColor;

	Int32 u_modelViewProjectionMatrix;
	Int32 u_bonesMatrixArray;

	ShaderInstance m_shaderInstance;
};

} // namespace o3d

#endif // _O3D_PICKINGMATERIAL_H


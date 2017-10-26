/**
 * @file widgetdrawmaterial.h
 * @brief Widget draw material.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2010-02-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WIDGETDRAWMATERIAL_H
#define _O3D_WIDGETDRAWMATERIAL_H

#include "o3d/engine/material/material.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class WidgetDrawMaterial
//-------------------------------------------------------------------------------------
//! Widget draw material
//---------------------------------------------------------------------------------------
class O3D_API WidgetDrawMaterial : public Material
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(WidgetDrawMaterial)

	//! Default constructor.
	WidgetDrawMaterial(BaseObject *parent);

	//! Virtual destructor.
	virtual ~WidgetDrawMaterial();

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
	//! @warning Not supported for this mode.
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

#endif // _O3DWIDGETDRAWMATERIAL_H


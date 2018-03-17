/**
 * @file lambertmaterial.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_LAMBERTMATERIAL_H
#define _O3D_LAMBERTMATERIAL_H

#include "material.h"

namespace o3d {

/**
 * @brief Material using the Lambert illumination model.
 * @date 2010-01-19
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * Illumination shader, based on Lambert method. It supports forward lighting, and
 * in the case of a deferred shading mode it put no more informations than the model do.
 * That means in deferred mode there is only an interpolated per vertex normal
 * (no tangant space).
 * The picking mode is not supported by this material, uses PickingMaterial for that.
 * @note u_diffuse and u_specular if there is respectively no diffuse map or no specular
 *       map, are premultiplied by the light diffuse and light specular. That mean in the
 *       shader there is no need to performs these products.
 */
class O3D_API LambertMaterial : public Material
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(LambertMaterial)

	//! Default constructor.
	LambertMaterial(BaseObject *parent);

	//! Virtual destructor.
	virtual ~LambertMaterial();

	//! Check if the material is supported by the hardware.
    virtual Bool isMaterialSupported() const override;

	//! Initialize material shader according to a specific MaterialPass setting,
	//! and shadable vertex mode.
	//! @param initMode Accept LIGHTING or DEFERRED mode.
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

    Bool m_ambientMap;
	Bool m_diffuseMap;
	Bool m_specularMap;
    Bool m_shineMap;

	UInt32 a_texCoords1[3];
	UInt32 a_rigging[3];
	UInt32 a_skinning[3];
	UInt32 a_weighting[3];

	Int32 u_modelViewProjectionMatrix[3];
    Int32 u_worldMatrix[3];        //!< world space vertex matrix
    Int32 u_normalMatrix[3];       //!< world space normal matrix
    Int32 u_bonesMatrixArray[3];   //!< object space bones matrices

    Int32 u_ambient;     //!< ambient color, deferred mode only
    Int32 u_ambientMap;  //!< ambient map, deferred mode only

	Int32 u_diffuse[3];
	Int32 u_diffuseMap[3];

	Int32 u_specular[3];
	Int32 u_specularMap[3];

    Int32 u_shine[3];
    Int32 u_shineMap[3];

    Int32 u_eyePos[3];

	Int32 u_lightPos[3];
	Int32 u_lightDir[3];
	Int32 u_lightAtt[3];
	Int32 u_lightDiffuse[3];
	Int32 u_lightSpecular[3];
    Int32 u_lightCosCutOff;    //!< only for lighting and spot light
    Int32 u_lightExponent;     //!< only for lighting and spot light

    ShaderInstance m_shaderInstance[4];   //!< for each light type

	virtual void getCommonLoc(ShaderInstance &shaderInstance, Int32 dest);
	void getRiggingLoc(ShaderInstance &shaderInstance, Int32 dest);
	void getSkinningLoc(ShaderInstance &shaderInstance, Int32 dest);
	void getLightLoc(ShaderInstance &shaderInstance, Int32 dest);
};

} // namespace o3d

#endif // _O3D_LAMBERTMATERIAL_H

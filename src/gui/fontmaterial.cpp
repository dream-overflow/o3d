/**
 * @file fontmaterial.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/fontmaterial.h"
#include "o3d/gui/guitype.h"
#include "o3d/engine/material/materialpass.h"
#include "o3d/engine/shader/shadable.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(FontMaterial, GUI_FONT_MATERIAL, Material)

// Default constructor.
FontMaterial::FontMaterial(BaseObject *parent) :
	Material(parent)
{
	m_initMode = AMBIENT;
	m_shaderName = "gui/font2d";
	m_name = "gui/font2d";
}

// Virtual destructor.
FontMaterial::~FontMaterial()
{

}

// Check if the material is supported by the hardware.
Bool FontMaterial::isMaterialSupported() const
{
	return True;
}

// Initialize material shader according to a specific MaterialPass setting.
void FontMaterial::initialize(
		InitMode initMode,
		MaterialPass &materialPass,
		Shadable &shadable)
{
	m_valid = False;

	if (initMode != AMBIENT)
			O3D_ERROR(E_InvalidParameter("InitMode::AMBIENT only is accepted"));

	m_options = "";

	Shader *shader = getScene()->getShaderManager()->addShader(m_shaderName);

	ShaderInstance &shaderInstance = m_shaderInstance;

	shader->buildInstance(shaderInstance);
    shaderInstance.assign("default", "default", m_options);
	shaderInstance.build(Shader::BUILD_COMPILE_AND_LINK);

	shaderInstance.bindShader();

	u_modelViewProjectionMatrix = shaderInstance.getUniformLocation("u_modelViewProjectionMatrix");

	u_ambientMap = shaderInstance.getUniformLocation("u_ambientMap");
	u_ambient = shaderInstance.getUniformLocation("u_ambient");

	a_texCoords1 = shaderInstance.getAttributeLocation("a_texCoords1");

	shaderInstance.unbindShader();

	m_valid = True;
}

// Release initialized data.
void FontMaterial::release()
{
	m_shaderInstance.detach();
	m_options.destroy();
	m_valid = False;
}

// Shadable object rendering for ambient pass.
void FontMaterial::processAmbient(
		Shadable &object,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	ShaderInstance &shader = m_shaderInstance;

	if ((m_initMode == AMBIENT) && shader.isOperational() && materialPass.getAmbientMap())
	{
		Context *glContext = getScene()->getContext();

		shader.bindShader();

		object.processAllFaces(Shadable::PREPARE_GEOMETRY);

		materialPass.assignMapSetting(MaterialPass::AMBIENT_MAP);
		shader.setConstTexture(u_ambientMap, materialPass.getAmbientMap(), 0);
		shader.setConstColor(u_ambient, materialPass.getAmbient());

		object.attribute(V_UV_MAP_ARRAY, a_texCoords1);

		shader.setConstMatrix4(
				u_modelViewProjectionMatrix,
				False,
				glContext->modelViewProjection());

		object.attribute(V_VERTICES_ARRAY, V_VERTICES_ARRAY);

		object.processAllFaces(Shadable::PROCESS_GEOMETRY);

		glContext->setActiveTextureUnit(0);
		glContext->bindTexture(TEXTURE_2D, 0);

		glContext->disableVertexAttribArray(a_texCoords1);
		glContext->disableVertexAttribArray(V_VERTICES_ARRAY);
		shader.unbindShader();
	}
}

// Shadable object rendering for picking.
void FontMaterial::processPicking(
		Shadable &object,
		Pickable &pickable,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	O3D_ERROR(E_InvalidOperation("Not permit"));
}

// Shadable object rendering for lighting and shadow pass.
void FontMaterial::processLighting(
		Shadable &object,
		Shadowable &shadowable,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	O3D_ERROR(E_InvalidOperation("Not permit"));
}

// Shadable object rendering for deferred diffuse pass.
void FontMaterial::processDeferred(
		Shadable &object,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	O3D_ERROR(E_InvalidOperation("Not permit"));
}

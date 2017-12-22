/**
 * @file pickingmaterial.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/picking.h"
#include "o3d/engine/material/pickingmaterial.h"
#include "o3d/engine/material/materialpass.h"
#include "o3d/engine/shader/shadable.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(PickingMaterial, ENGINE_PICKING_MATERIAL, Material)

// Default constructor.
PickingMaterial::PickingMaterial(BaseObject *parent) :
	Material(parent),
	m_opacityMap(False)
{
	m_shaderName = "picking";
	m_name = "picking";
	m_initMode = PICKING;
}

// Virtual destructor.
PickingMaterial::~PickingMaterial()
{

}

// Check if the material is supported by the hardware.
Bool PickingMaterial::isMaterialSupported() const
{
	return True;
}

// Initialize material shader according to a specific MaterialPass setting.
void PickingMaterial::initialize(
		InitMode initMode,
		MaterialPass &materialPass,
		Shadable &shadable)
{
	m_arrays.clear();
	m_valid = False;

	if (initMode != PICKING)
		O3D_ERROR(E_InvalidParameter("InitMode::PICKING only is accepted"));

	m_options = "";

	// ambient map or ambient color ?
	if (materialPass.getOpacityMap())
	{
		m_options += "OPACITY_MAP;";
		m_opacityMap = True;
	}
	// diffuse color
	else
	{
		m_opacityMap = False;
	}

	m_arrays.push_back(V_VERTICES_ARRAY);

	if (m_opacityMap)
		m_arrays.push_back(V_TEXCOORDS_2D_1_ARRAY);

	// static mesh ?
	if (shadable.getVertexProgramType() == Shadable::VP_MESH)
		m_options += "MESH;";
	else if (shadable.getVertexProgramType() == Shadable::VP_RIGGING)
	{
		m_arrays.push_back(V_RIGGING_ARRAY);
		m_options += String("NUM_BONES=") << Int32(O3D_MAX_SKINNING_MATRIX_ARRAY) << ";RIGGING;";
	}
	else if (shadable.getVertexProgramType() == Shadable::VP_SKINNING)
	{
		m_arrays.push_back(V_WEIGHTING_ARRAY);
		m_arrays.push_back(V_SKINNING_ARRAY);
		m_options += String("NUM_BONES=") << Int32(O3D_MAX_SKINNING_MATRIX_ARRAY) << ";SKINNING;";
	}
	else if (shadable.getVertexProgramType() == Shadable::VP_BILLBOARD)
		m_options += "BILLBOARD;";
	else
		m_options += "MESH;";

	Shader *shader = getScene()->getShaderManager()->addShader(m_shaderName);
	ShaderInstance &shaderInstance = m_shaderInstance;

	shader->buildInstance(shaderInstance);
    shaderInstance.assign("default", "default", m_options);
	shaderInstance.build(Shader::BUILD_COMPILE_AND_LINK);

	shaderInstance.bindShader();

	u_modelViewProjectionMatrix = shaderInstance.getUniformLocation("u_modelViewProjectionMatrix");

	if (m_opacityMap)
	{
		u_opacityMap = shaderInstance.getUniformLocation("u_opacityMap");
		a_texCoords1 = shaderInstance.getAttributeLocation("a_texCoords1");
	}

	u_pickingColor = shaderInstance.getUniformLocation("u_pickingColor");

	// rigging
	if (shadable.getVertexProgramType() == Shadable::VP_RIGGING)
	{
		a_rigging = shaderInstance.getAttributeLocation("a_rigging");
		u_bonesMatrixArray = shaderInstance.getUniformLocation("u_bonesMatrixArray");
	}
	// skinning
	else if (shadable.getVertexProgramType() == Shadable::VP_SKINNING)
	{
		a_skinning = shaderInstance.getAttributeLocation("a_skinning");
		a_weighting = shaderInstance.getAttributeLocation("a_weighting");
		u_bonesMatrixArray = shaderInstance.getUniformLocation("u_bonesMatrixArray");
	}
	else
		a_rigging = a_skinning = a_weighting = 0;

	shaderInstance.unbindShader();

	buildVertexArray(shadable);

	m_valid = True;
}

// Release initialized data.
void PickingMaterial::release()
{
	m_shaderInstance.detach();
	m_options.destroy();
	m_opacityMap = False;
	m_valid = False;
}

// Shadable object rendering for ambient pass.
void PickingMaterial::processAmbient(
		Shadable &object,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	O3D_ERROR(E_InvalidOperation("Not permit"));
}

// Shadable object rendering for picking.
void PickingMaterial::processPicking(
		Shadable &object,
		Pickable &pickable,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	ShaderInstance &shader = m_shaderInstance;
	Context *glContext = getScene()->getContext();

	if (shader.isOperational())
	{
		object.processAllFaces(Shadable::PREPARE_GEOMETRY);

		shader.bindShader();

		if (m_opacityMap)
		{
			materialPass.assignMapSetting(MaterialPass::OPACITY_MAP);
			shader.setConstTexture(u_opacityMap, materialPass.getOpacityMap(), 0);

			object.attribute(V_TEXCOORDS_2D_1_ARRAY, a_texCoords1);
		}

		shader.setConstColor(u_pickingColor, pickable.getPickableColor());

		shader.setConstMatrix4(
				u_modelViewProjectionMatrix,
				False,
				glContext->modelViewProjection());

		object.attribute(V_VERTICES_ARRAY, V_VERTICES_ARRAY);

		// rigging
		if ((a_rigging > 0) && (object.getVertexProgramType() == Shadable::VP_RIGGING))
		{
			shader.setNConstMatrix4(
					u_bonesMatrixArray,
					O3D_MAX_SKINNING_MATRIX_ARRAY,
					False,
					object.getMatrixArray());

			object.attribute(V_RIGGING_ARRAY, a_rigging);
		}
		// skinning
		else if ((a_skinning > 0) && (object.getVertexProgramType() == Shadable::VP_SKINNING))
		{
			shader.setNConstMatrix4(
					u_bonesMatrixArray,
					O3D_MAX_SKINNING_MATRIX_ARRAY,
					False,
					object.getMatrixArray());

			object.attribute(V_SKINNING_ARRAY, a_skinning);
			object.attribute(V_WEIGHTING_ARRAY, a_weighting);
		}

		object.processAllFaces(Shadable::PROCESS_GEOMETRY);

		if (m_opacityMap)
		{
			glContext->setActiveTextureUnit(0);
			glContext->bindTexture(TEXTURE_2D, 0);

			glContext->disableVertexAttribArray(a_texCoords1);
		}

		if ((a_rigging > 0) && (object.getVertexProgramType() == Shadable::VP_RIGGING))
		{
			glContext->disableVertexAttribArray(a_rigging);
		}
		else if ((a_skinning > 0) && (object.getVertexProgramType() == Shadable::VP_SKINNING))
		{
			glContext->disableVertexAttribArray(a_skinning);
			glContext->disableVertexAttribArray(a_weighting);
		}

		glContext->disableVertexAttribArray(V_VERTICES_ARRAY);
		shader.unbindShader();
	}
}

// Shadable object rendering for lighting and shadow pass.
void PickingMaterial::processLighting(
		Shadable &object,
		Shadowable &shadowable,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	O3D_ERROR(E_InvalidOperation("Not permit"));
}

// Shadable object rendering for deferred diffuse pass.
void PickingMaterial::processDeferred(
		Shadable &object,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	O3D_ERROR(E_InvalidOperation("Not permit"));
}


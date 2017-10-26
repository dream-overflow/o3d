/**
 * @file lambertmaterial.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/material/lambertmaterial.h"
#include "o3d/engine/material/materialpass.h"
#include "o3d/engine/shader/shadable.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/context.h"
#include "o3d/engine/object/light.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(LambertMaterial, ENGINE_LAMBERT_MATERIAL, Material)

// Default constructor.
LambertMaterial::LambertMaterial(BaseObject *parent) :
	Material(parent),
	m_diffuseMap(False),
    m_specularMap(False),
    m_shineMap(False)
{
	m_shaderName = "lambert";
	m_name = "lambert";
}

// Virtual destructor.
LambertMaterial::~LambertMaterial()
{

}

// Check if the material is supported by the hardware.
Bool LambertMaterial::isMaterialSupported() const
{
	return True;
}

// Initialize material shader according to a specific MaterialPass setting.
void LambertMaterial::initialize(
		InitMode initMode,
		MaterialPass &materialPass,
		Shadable &shadable)
{
	m_arrays.clear();
	m_valid = False;

    if ((initMode != LIGHTING) && (initMode != DEFERRED)) {
			O3D_ERROR(E_InvalidParameter("InitMode::LIGHTING or DEFERRED only are accepted"));
    }

	m_options = "";

    // ambient map + color ? only on deferred mode
    if (materialPass.getAmbientMap() && (initMode == DEFERRED)) {
        m_options += "AMBIENT_MAP;";
        m_ambientMap = True;
    } else {
        // ambient color
        m_ambientMap = False;
    }

	// diffuse map or diffuse color ?
    if (materialPass.getDiffuseMap()) {
		m_options += "DIFFUSE_MAP;";
		m_diffuseMap = True;
    } else {
        // diffuse color
		m_diffuseMap = False;
    }

	// specular map or specular color ?
    if (materialPass.getSpecularMap()) {
		m_options += "SPECULAR_MAP;";
		m_specularMap = True;
    } else {
        // specular color
		m_specularMap = False;
    }

    // shine map or shine value ?
    if (materialPass.getShineMap()) {
        m_options += "SHINE_MAP;";
        m_shineMap = True;
    } else {
        // shine color
        m_shineMap = False;
    }

	m_arrays.push_back(V_VERTICES_ARRAY);
	m_arrays.push_back(V_NORMALS_ARRAY);

    if (m_ambientMap || m_diffuseMap || m_specularMap) {
        m_arrays.push_back(V_TEXCOORDS_2D_1_ARRAY);
    }

    // static mesh ?
    if (shadable.getVertexProgramType() == Shadable::VP_MESH) {
        m_options += "MESH;";
    } else if (shadable.getVertexProgramType() == Shadable::VP_RIGGING) {
        m_arrays.push_back(V_RIGGING_ARRAY);
        m_options += String("NUM_BONES=") << Int32(O3D_MAX_SKINNING_MATRIX_ARRAY) << ";RIGGING;";
    } else if (shadable.getVertexProgramType() == Shadable::VP_SKINNING) {
        m_arrays.push_back(V_WEIGHTING_ARRAY);
        m_arrays.push_back(V_SKINNING_ARRAY);
        m_options += String("NUM_BONES=") << Int32(O3D_MAX_SKINNING_MATRIX_ARRAY) << ";SKINNING;";
    } else if (shadable.getVertexProgramType() == Shadable::VP_BILLBOARD) {
        m_options += "BILLBOARD;";
    } else {
        m_options += "MESH;";
    }

	// lighting mode
    if (initMode == LIGHTING) {
	/*	switch (materialPass.getFogMode()) TODO
		{
			case MaterialPass::FOG_NONE:
				m_options += "FOG=0;";
				break;
			case MaterialPass::FOG_LINEAR:
				m_options += "FOG=LINEAR;";
				break;
			case MaterialPass::FOG_EXP:
				m_options += "FOG=EXP;";
				break;
			case MaterialPass::FOG_EXP2:
				m_options += "FOG=EXP2;";
				break;
		};*/
/*
		switch (shadable.getShadowMode())
		{
			case Shadowable::SHADOW_NONE:
				m_options += "SHADOW=0;";
				break;
			case Shadowable::SHADOW_MAP_HARD:
				m_options += "SHADODW=HARD;";
				break;
			case Shadowable::SHADOW_MAP_SOFT:
				m_options += "SHADOW=SOFT;";
				break;
		};
*/

		Shader *shader = getScene()->getShaderManager()->addShader(m_shaderName);

		// point light
		{
			ShaderInstance &shaderInstance = m_shaderInstance[Light::POINT_LIGHT];

			shader->buildInstance(shaderInstance);
			shaderInstance.assign("lighting", "lighting", "", m_options + "POINT_LIGHT;");
			shaderInstance.build(Shader::BUILD_COMPILE_AND_LINK);

			shaderInstance.bindShader();

			getCommonLoc(shaderInstance, Light::POINT_LIGHT);
			getLightLoc(shaderInstance, Light::POINT_LIGHT);

            if (shadable.getVertexProgramType() == Shadable::VP_RIGGING)
                getRiggingLoc(shaderInstance, Light::POINT_LIGHT);
            else if (shadable.getVertexProgramType() == Shadable::VP_SKINNING)
                getSkinningLoc(shaderInstance, Light::POINT_LIGHT);

			shaderInstance.unbindShader();
		}

		// spot light
		{
			ShaderInstance &shaderInstance = m_shaderInstance[Light::SPOT_LIGHT];

			shader->buildInstance(shaderInstance);
			shaderInstance.assign("lighting", "lighting", "", m_options + "SPOT_LIGHT;");
			shaderInstance.build(Shader::BUILD_COMPILE_AND_LINK);

			shaderInstance.bindShader();

			getCommonLoc(shaderInstance, Light::SPOT_LIGHT);
            getLightLoc(shaderInstance, Light::SPOT_LIGHT);

            if (shadable.getVertexProgramType() == Shadable::VP_RIGGING)
                getRiggingLoc(shaderInstance, Light::SPOT_LIGHT);
            else if (shadable.getVertexProgramType() == Shadable::VP_SKINNING)
                getSkinningLoc(shaderInstance, Light::SPOT_LIGHT);

			shaderInstance.unbindShader();
		}

		// directional light
		{
			ShaderInstance &shaderInstance = m_shaderInstance[Light::DIRECTIONAL_LIGHT];

			shader->buildInstance(shaderInstance);
			shaderInstance.assign("lighting", "lighting", "", m_options + "DIRECTIONAL_LIGHT;");
			shaderInstance.build(Shader::BUILD_COMPILE_AND_LINK);

			shaderInstance.bindShader();

			getCommonLoc(shaderInstance, Light::DIRECTIONAL_LIGHT);
			getLightLoc(shaderInstance, Light::DIRECTIONAL_LIGHT);

            if (shadable.getVertexProgramType() == Shadable::VP_RIGGING)
                getRiggingLoc(shaderInstance, Light::DIRECTIONAL_LIGHT);
            else if (shadable.getVertexProgramType() == Shadable::VP_SKINNING)
                getSkinningLoc(shaderInstance, Light::DIRECTIONAL_LIGHT);

			shaderInstance.unbindShader();
		}
	}

	// deferred mode
	else if (initMode == DEFERRED)
	{
		Shader *shader = getScene()->getShaderManager()->addShader(m_shaderName);
		ShaderInstance &shaderInstance = m_shaderInstance[0];

		shader->buildInstance(shaderInstance);
        shaderInstance.assign("deferred", "deferred", "", m_options + "AMBIENT;DIFFUSE;SPECULAR;SHINE");
		shaderInstance.build(Shader::BUILD_COMPILE_AND_LINK);

		shaderInstance.bindShader();

        getCommonLoc(shaderInstance, 0);

        if (shadable.getVertexProgramType() == Shadable::VP_RIGGING)
            getRiggingLoc(shaderInstance, 0);
        else if (shadable.getVertexProgramType() == Shadable::VP_SKINNING)
            getSkinningLoc(shaderInstance, 0);

        // ambient(map)
        if (m_ambientMap)
            u_ambientMap = shaderInstance.getUniformLocation("u_ambientMap");

        u_ambient = shaderInstance.getUniformLocation("u_ambient");

		shaderInstance.unbindShader();
	}

	m_initMode = initMode;

	// need normals to process this shader
	if (!shadable.isOperation(Shadable::NORMALS))
	{
		shadable.operation(Shadable::NORMALS);
        shadable.operation(Shadable::CREATE);
	}

	buildVertexArray(shadable);

	m_valid = True;
}

void LambertMaterial::getCommonLoc(ShaderInstance &shaderInstance, Int32 dest)
{
	u_modelViewProjectionMatrix[dest] = shaderInstance.getUniformLocation("u_modelViewProjectionMatrix");
    u_worldMatrix[dest] = shaderInstance.getUniformLocation("u_worldMatrix");
	u_normalMatrix[dest] = shaderInstance.getUniformLocation("u_normalMatrix");

	if (m_diffuseMap)
		u_diffuseMap[dest] = shaderInstance.getUniformLocation("u_diffuseMap");
	else
		u_diffuse[dest] = shaderInstance.getUniformLocation("u_diffuse");

	if (m_specularMap)
		u_specularMap[dest] = shaderInstance.getUniformLocation("u_specularMap");
	else
		u_specular[dest] = shaderInstance.getUniformLocation("u_specular");

    if (m_shineMap)
        u_shineMap[dest] = shaderInstance.getUniformLocation("u_shineMap");
    else
        u_shine[dest] = shaderInstance.getUniformLocation("u_shine");

    if (m_diffuseMap || m_specularMap || m_shineMap)
		a_texCoords1[dest] = shaderInstance.getAttributeLocation("a_texCoords1");

    u_eyePos[dest] = shaderInstance.getUniformLocation("u_eyePos");

	a_rigging[dest] = a_skinning[dest] = a_weighting[dest] = 0;
}

void LambertMaterial::getRiggingLoc(ShaderInstance &shaderInstance, Int32 dest)
{
	a_rigging[dest] = shaderInstance.getAttributeLocation("a_rigging");
	u_bonesMatrixArray[dest] = shaderInstance.getUniformLocation("u_bonesMatrixArray");
}

void LambertMaterial::getSkinningLoc(ShaderInstance &shaderInstance, Int32 dest)
{
	a_skinning[dest] = shaderInstance.getAttributeLocation("a_skinning");
	a_weighting[dest] = shaderInstance.getAttributeLocation("a_weighting");
	u_bonesMatrixArray[dest] = shaderInstance.getUniformLocation("u_bonesMatrixArray");
}

void LambertMaterial::getLightLoc(ShaderInstance &shaderInstance, Int32 dest)
{
    if (dest != Light::DIRECTIONAL_LIGHT)
	{
		u_lightPos[dest] = shaderInstance.getUniformLocation("u_lightPos");
		u_lightAtt[dest] = shaderInstance.getUniformLocation("u_lightAtt");
	}
	else
	{
		u_lightPos[dest] = -1;
		u_lightAtt[dest] = -1;
	}

	if (m_diffuseMap)
		u_lightDiffuse[dest] = shaderInstance.getUniformLocation("u_lightDiffuse");
	else
		u_lightDiffuse[dest] = -1;

	if (m_specularMap)
		u_lightSpecular[dest] = shaderInstance.getUniformLocation("u_lightSpecular");
	else
		u_lightSpecular[dest] = -1;

	if (dest == Light::SPOT_LIGHT)
	{
		u_lightCosCutOff = shaderInstance.getUniformLocation("u_lightCosCutOff");
		u_lightExponent = shaderInstance.getUniformLocation("u_lightExponent");
	}

	if (dest != Light::POINT_LIGHT)
        u_lightDir[dest] = shaderInstance.getUniformLocation("u_lightDir");
	else
        u_lightDir[dest] = -1;
}

// Release initialized data.
void LambertMaterial::release()
{
	m_shaderInstance[0].detach();
	m_shaderInstance[1].detach();
	m_shaderInstance[2].detach();
	m_options.destroy();
	m_diffuseMap = m_specularMap = False;
	m_initMode = AMBIENT;
	m_valid = False;
}

// Shadable object rendering for ambient pass.
void LambertMaterial::processAmbient(
		Shadable &object,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	O3D_ERROR(E_InvalidOperation("Not permit"));
}

// Shadable object rendering for picking.
void LambertMaterial::processPicking(
		Shadable &object,
		Pickable &pickable,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	O3D_ERROR(E_InvalidOperation("Not permit"));
}

// Shadable object rendering for lighting and shadow pass.
void LambertMaterial::processLighting(
		Shadable &object,
		Shadowable &shadowable,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	ShaderInstance &shader = m_shaderInstance[drawInfo.light.type];
	Context *glContext = getScene()->getContext();

	if ((m_initMode == LIGHTING) && shader.isOperational())
	{
        glContext->blending().setFunc(Blending::ONE__ONE);

		shader.bindShader();

		object.processAllFaces(Shadable::PREPARE_GEOMETRY);

        // u_diffuse is pre-multiplied by light diffuse if there is no diffuse map
		if (m_diffuseMap)
		{
			materialPass.assignMapSetting(MaterialPass::DIFFUSE_MAP);
			shader.setConstTexture(u_diffuseMap[drawInfo.light.type], materialPass.getDiffuseMap(), 0);
            shader.setConstColor(u_lightDiffuse[drawInfo.light.type], drawInfo.light.diffuse);
		}
		else
			shader.setConstColor(u_diffuse[drawInfo.light.type], materialPass.getDiffuse()*drawInfo.light.diffuse);

        // u_specular is pre-multiplied by light specular if there is no specular map
		if (m_specularMap)
		{
			materialPass.assignMapSetting(MaterialPass::SPECULAR_MAP);
            shader.setConstTexture(u_specularMap[drawInfo.light.type], materialPass.getSpecularMap(), 1);
            shader.setConstColor(u_lightSpecular[drawInfo.light.type], drawInfo.light.specular);
		}
		else
			shader.setConstColor(u_specular[drawInfo.light.type], materialPass.getSpecular()*drawInfo.light.specular);

        if (m_shineMap)
        {
            materialPass.assignMapSetting(MaterialPass::SHINE_MAP);
            shader.setConstTexture(u_shineMap[drawInfo.light.type], materialPass.getShineMap(), 2);
        }
        else
            shader.setConstFloat(u_shine[drawInfo.light.type], materialPass.getShine());

        if (m_diffuseMap || m_specularMap || m_shineMap)
		{
			object.attribute(V_TEXCOORDS_2D_1_ARRAY, a_texCoords1[drawInfo.light.type]);
		}

		shader.setConstMatrix4(
				u_modelViewProjectionMatrix[drawInfo.light.type],
				False,
				glContext->modelViewProjection());

		shader.setConstMatrix4(
                u_worldMatrix[drawInfo.light.type],
				False,
                object.getObjectWorldMatrix());

		shader.setConstMatrix3(
				u_normalMatrix[drawInfo.light.type],
				False,
                object.getObjectWorldMatrix().getRotation().invert().transposeTo());

		object.attribute(V_VERTICES_ARRAY, V_VERTICES_ARRAY);
        object.attribute(V_NORMALS_ARRAY, V_NORMALS_ARRAY);

		//m_vertexArray.bindArray();

        // eye (cam) pos
        shader.setConstVector3(u_eyePos[drawInfo.light.type], drawInfo.eyePos);

		// point light
		if (drawInfo.light.type == Light::POINT_LIGHT)
		{
            shader.setConstVector4(u_lightPos[drawInfo.light.type], drawInfo.light.worldPos);
			shader.setConstVector3(u_lightAtt[drawInfo.light.type], drawInfo.light.attenuation);
		}
		// spot light
		else if (drawInfo.light.type == Light::SPOT_LIGHT)
		{
            shader.setConstVector4(u_lightPos[drawInfo.light.type], drawInfo.light.worldPos);
            shader.setConstVector3(u_lightDir[drawInfo.light.type], drawInfo.light.worldDir);
			shader.setConstVector3(u_lightAtt[drawInfo.light.type], drawInfo.light.attenuation);
			shader.setConstFloat(u_lightCosCutOff, drawInfo.light.cosCutOff);
			shader.setConstFloat(u_lightExponent, drawInfo.light.exponent);
		}
		// directional light
		else if (drawInfo.light.type == Light::DIRECTIONAL_LIGHT)
		{
            shader.setConstVector3(u_lightDir[drawInfo.light.type], drawInfo.light.worldDir);
        }

        // rigging
        if ((a_rigging[drawInfo.light.type] > 0) && (object.getVertexProgramType() == Shadable::VP_RIGGING))
        {
            shader.setNConstMatrix4(
                    u_bonesMatrixArray[drawInfo.light.type],
                    O3D_MAX_SKINNING_MATRIX_ARRAY,
                    False,
                    object.getMatrixArray());

            object.attribute(V_RIGGING_ARRAY, a_rigging[drawInfo.light.type]);
        }
        // skinning
        else if ((a_skinning[drawInfo.light.type] > 0) && (object.getVertexProgramType() == Shadable::VP_SKINNING))
        {
            shader.setNConstMatrix4(
                    u_bonesMatrixArray[drawInfo.light.type],
                    O3D_MAX_SKINNING_MATRIX_ARRAY,
                    False,
                    object.getMatrixArray());

            object.attribute(V_SKINNING_ARRAY, a_skinning[drawInfo.light.type]);
            object.attribute(V_WEIGHTING_ARRAY, a_weighting[drawInfo.light.type]);
        }

		object.processAllFaces(Shadable::PROCESS_GEOMETRY);

		if (m_diffuseMap)
		{
			glContext->setActiveTextureUnit(0);
			glContext->bindTexture(TEXTURE_2D, 0);
		}

		if (m_specularMap)
		{
			glContext->setActiveTextureUnit(1);
			glContext->bindTexture(TEXTURE_2D, 0);
		}

        if (m_shineMap)
        {
            glContext->setActiveTextureUnit(2);
            glContext->bindTexture(TEXTURE_2D, 0);
        }

		if (m_diffuseMap || m_specularMap)
			glContext->disableVertexAttribArray(a_texCoords1[drawInfo.light.type]);

        if ((a_rigging[drawInfo.light.type] > 0) && (object.getVertexProgramType() == Shadable::VP_RIGGING))
        {
            glContext->disableVertexAttribArray(a_rigging[drawInfo.light.type]);
        }
        else if ((a_skinning[drawInfo.light.type] > 0) && (object.getVertexProgramType() == Shadable::VP_SKINNING))
        {
            glContext->disableVertexAttribArray(a_skinning[drawInfo.light.type]);
            glContext->disableVertexAttribArray(a_weighting[drawInfo.light.type]);
        }

		glContext->disableVertexAttribArray(V_NORMALS_ARRAY);
		glContext->disableVertexAttribArray(V_VERTICES_ARRAY);
		shader.unbindShader();

        //m_vertexArray.unbindArray();
	}
}

// Shadable object rendering for deferred diffuse pass.
void LambertMaterial::processDeferred(
		Shadable &object,
		const DrawInfo &drawInfo,
		const MaterialPass &materialPass)
{
	ShaderInstance &shader = m_shaderInstance[0];
	Context *glContext = getScene()->getContext();

	if ((m_initMode == DEFERRED) && shader.isOperational())
	{
		shader.bindShader();

        object.processAllFaces(Shadable::PREPARE_GEOMETRY);

        if (m_ambientMap)
        {
            materialPass.assignMapSetting(MaterialPass::AMBIENT_MAP);
            shader.setConstTexture(u_ambientMap, materialPass.getAmbientMap(), 0);
        }
        shader.setConstColor(u_ambient, materialPass.getAmbient());

		if (m_diffuseMap)
		{
			materialPass.assignMapSetting(MaterialPass::DIFFUSE_MAP);
            shader.setConstTexture(u_diffuseMap[0], materialPass.getDiffuseMap(), 1);
		}
		else
			shader.setConstColor(u_diffuse[0], materialPass.getDiffuse());

		if (m_specularMap)
		{
			materialPass.assignMapSetting(MaterialPass::SPECULAR_MAP);
            shader.setConstTexture(u_specularMap[0], materialPass.getSpecularMap(), 2);
		}
		else
			shader.setConstColor(u_specular[0], materialPass.getSpecular());

        if (m_shineMap)
        {
            materialPass.assignMapSetting(MaterialPass::SHINE_MAP);
            shader.setConstTexture(u_shineMap[0], materialPass.getShineMap(), 3);
        }
        else
            shader.setConstFloat(u_shine[0], materialPass.getShine());

        if (m_ambientMap || m_diffuseMap || m_specularMap || m_shineMap)
		{
			object.attribute(V_TEXCOORDS_2D_1_ARRAY, a_texCoords1[0]);
		}

		shader.setConstMatrix4(
				u_modelViewProjectionMatrix[0],
				False,
				glContext->modelViewProjection());

        shader.setConstMatrix4(
                u_worldMatrix[0],
                False,
                object.getObjectWorldMatrix());

        shader.setConstMatrix3(
                u_normalMatrix[0],
                False,
                object.getObjectWorldMatrix().getRotation().invert().transposeTo());

		object.attribute(V_VERTICES_ARRAY, V_VERTICES_ARRAY);
		object.attribute(V_NORMALS_ARRAY, V_NORMALS_ARRAY);

		// rigging
        if ((a_rigging[0] > 0) && (object.getVertexProgramType() == Shadable::VP_RIGGING))
        {
            shader.setNConstMatrix4(
                    u_bonesMatrixArray[0],
                    O3D_MAX_SKINNING_MATRIX_ARRAY,
                    False,
                    object.getMatrixArray());

            object.attribute(V_RIGGING_ARRAY, a_rigging[0]);
        }
        // skinning
        else if ((a_skinning[0] > 0) && (object.getVertexProgramType() == Shadable::VP_SKINNING))
        {
            shader.setNConstMatrix4(
                    u_bonesMatrixArray[0],
                    O3D_MAX_SKINNING_MATRIX_ARRAY,
                    False,
                    object.getMatrixArray());

            object.attribute(V_SKINNING_ARRAY, a_skinning[0]);
            object.attribute(V_WEIGHTING_ARRAY, a_weighting[0]);
        }

		object.processAllFaces(Shadable::PROCESS_GEOMETRY);

        if (m_ambientMap)
        {
            glContext->setActiveTextureUnit(0);
            glContext->bindTexture(TEXTURE_2D, 0);
        }

		if (m_diffuseMap)
		{
            glContext->setActiveTextureUnit(1);
			glContext->bindTexture(TEXTURE_2D, 0);
		}

		if (m_specularMap)
		{
            glContext->setActiveTextureUnit(2);
			glContext->bindTexture(TEXTURE_2D, 0);
		}

        if (m_shineMap)
        {
            glContext->setActiveTextureUnit(3);
            glContext->bindTexture(TEXTURE_2D, 0);
        }


        if (m_ambientMap || m_diffuseMap || m_specularMap || m_shineMap)
			glContext->disableVertexAttribArray(a_texCoords1[0]);

        if ((a_rigging[0] > 0) && (object.getVertexProgramType() == Shadable::VP_RIGGING))
        {
            glContext->disableVertexAttribArray(a_rigging[0]);
        }
        else if ((a_skinning[0] > 0) && (object.getVertexProgramType() == Shadable::VP_SKINNING))
        {
            glContext->disableVertexAttribArray(a_skinning[0]);
            glContext->disableVertexAttribArray(a_weighting[0]);
        }

		glContext->disableVertexAttribArray(V_NORMALS_ARRAY);
		glContext->disableVertexAttribArray(V_VERTICES_ARRAY);

		shader.unbindShader();
	}
}


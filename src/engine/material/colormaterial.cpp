/**
 * @file colormaterial.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/material/colormaterial.h"
#include "o3d/engine/material/materialpass.h"
#include "o3d/engine/shader/shadable.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ColorMaterial, ENGINE_COLOR_MATERIAL, Material)

// Default constructor.
ColorMaterial::ColorMaterial(BaseObject *parent) :
    Material(parent),
    m_diffuseMap(False),
    a_rigging(0),
    a_skinning(0)
{
    m_shaderName = "color";
    m_name = "color";
    m_initMode = AMBIENT;
}

// Virtual destructor.
ColorMaterial::~ColorMaterial()
{

}

// Check if the material is supported by the hardware.
Bool ColorMaterial::isMaterialSupported() const
{
    return True;
}

// Initialize material shader according to a specific MaterialPass setting.
void ColorMaterial::initialize(
        InitMode initMode,
        MaterialPass &materialPass,
        Shadable &shadable)
{
    m_arrays.clear();
    m_valid = False;

    if ((initMode != AMBIENT) && (initMode != DEFERRED))
        O3D_ERROR(E_InvalidParameter("InitMode::AMBIENT or DEFERRED only are accepted"));

    m_options = "";

    a_rigging = a_skinning = 0;

    // diffuse map ?
    if (materialPass.getDiffuseMap())
    {
        m_options += "DIFFUSE_MAP;";
        m_diffuseMap = True;
    }
    else
        m_diffuseMap = False;

    if (materialPass.getAlphaTest())
    {
        if (materialPass.getAlphaTestFunc() == COMP_GREATER)
            m_options += String("ALPHA_FUNC_GREATER;ALPHA_TEST_REF=") << materialPass.getAlphaTestFuncRef() << ";";
        else if (materialPass.getAlphaTestFunc() == COMP_LESS)
            m_options += String("ALPHA_FUNC_LESS;ALPHA_TEST_REF=") << materialPass.getAlphaTestFuncRef() << ";";
    }
    //GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, and GL_ALWAYS
    m_arrays.push_back(V_VERTICES_ARRAY);

    if (m_diffuseMap)
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

    // ambient mode
    if (initMode == AMBIENT)
    {
        Shader *shader = getScene()->getShaderManager()->addShader(m_shaderName);
        ShaderInstance &shaderInstance = m_shaderInstance;

        shader->buildInstance(shaderInstance);
        shaderInstance.assign("default", "default", "", m_options);
        shaderInstance.build(Shader::BUILD_COMPILE_AND_LINK);

        shaderInstance.bindShader();

        u_modelViewProjectionMatrix = shaderInstance.getUniformLocation("u_modelViewProjectionMatrix");

        if (m_diffuseMap)
            u_diffuseMap = shaderInstance.getUniformLocation("u_diffuseMap");

        u_diffuse = shaderInstance.getUniformLocation("u_diffuse");

        if (shadable.getVertexProgramType() == Shadable::VP_RIGGING)
        {
            a_rigging = shaderInstance.getAttributeLocation("a_rigging");
            u_bonesMatrixArray = shaderInstance.getUniformLocation("u_bonesMatrixArray");
        }
        else if (shadable.getVertexProgramType() == Shadable::VP_SKINNING)
        {
            a_skinning = shaderInstance.getAttributeLocation("a_skinning");
            a_weighting = shaderInstance.getAttributeLocation("a_weighting");
            u_bonesMatrixArray = shaderInstance.getUniformLocation("u_bonesMatrixArray");
        }

        shaderInstance.unbindShader();
    }
    // deferred mode
    else if (initMode == DEFERRED)
    {
        Shader *shader = getScene()->getShaderManager()->addShader(m_shaderName);
        ShaderInstance &shaderInstance = m_shaderInstance;

        shader->buildInstance(shaderInstance);
        shaderInstance.assign("default", "deferred", "", m_options + "AMBIENT");
        shaderInstance.build(Shader::BUILD_COMPILE_AND_LINK);

        shaderInstance.bindShader();

        u_modelViewProjectionMatrix = shaderInstance.getUniformLocation("u_modelViewProjectionMatrix");
        u_worldMatrix = shaderInstance.getUniformLocation("u_worldMatrix");
        u_normalMatrix = shaderInstance.getUniformLocation("u_normalMatrix");

        if (m_diffuseMap)
            u_diffuseMap = shaderInstance.getUniformLocation("u_diffuseMap");

        u_diffuse = shaderInstance.getUniformLocation("u_diffuse");

        if (shadable.getVertexProgramType() == Shadable::VP_RIGGING)
        {
            a_rigging = shaderInstance.getAttributeLocation("a_rigging");
            u_bonesMatrixArray = shaderInstance.getUniformLocation("u_bonesMatrixArray");
        }
        else if (shadable.getVertexProgramType() == Shadable::VP_SKINNING)
        {
            a_skinning = shaderInstance.getAttributeLocation("a_skinning");
            a_weighting = shaderInstance.getAttributeLocation("a_weighting");
            u_bonesMatrixArray = shaderInstance.getUniformLocation("u_bonesMatrixArray");
        }

        shaderInstance.unbindShader();
    }

    m_initMode = initMode;

    buildVertexArray(shadable);

    m_valid = True;
}

// Release initialized data.
void ColorMaterial::release()
{
    m_shaderInstance.detach();
    m_options.destroy();
    m_diffuseMap = False;
    m_valid = False;
}

// Shadable object rendering for ambient pass.
void ColorMaterial::processAmbient(
        Shadable &object,
        const DrawInfo &drawInfo,
        const MaterialPass &materialPass)
{
    ShaderInstance &shader = m_shaderInstance;
    Context *glContext = getScene()->getContext();

    if (shader.isOperational())
    {
        shader.bindShader();

        object.processAllFaces(Shadable::PREPARE_GEOMETRY);

        if (m_diffuseMap)
        {
            materialPass.assignMapSetting(MaterialPass::DIFFUSE_MAP);
            shader.setConstTexture(u_diffuseMap, materialPass.getDiffuseMap(), 0);

            object.attribute(V_TEXCOORDS_2D_1_ARRAY, V_TEXCOORDS_2D_1_ARRAY);
        }

        shader.setConstColor(u_diffuse, materialPass.getDiffuse());

        shader.setConstMatrix4(u_modelViewProjectionMatrix, False, getScene()->getContext()->modelViewProjection());

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

        if (m_diffuseMap)
        {
            glContext->setActiveTextureUnit(0);
            glContext->bindTexture(TEXTURE_2D, 0);

            glContext->disableVertexAttribArray(V_TEXCOORDS_2D_1_ARRAY);
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

// Shadable object rendering for picking.
void ColorMaterial::processPicking(
        Shadable &object,
        Pickable &pickable,
        const DrawInfo &drawInfo,
        const MaterialPass &materialPass)
{
    O3D_ERROR(E_InvalidOperation("Not permit"));
}

// Shadable object rendering for lighting and shadow pass.
void ColorMaterial::processLighting(
        Shadable &object,
        Shadowable &shadowable,
        const DrawInfo &drawInfo,
        const MaterialPass &materialPass)
{
    O3D_ERROR(E_InvalidOperation("Not permit"));
}

// Shadable object rendering for deferred diffuse pass.
void ColorMaterial::processDeferred(
        Shadable &object,
        const DrawInfo &drawInfo,
        const MaterialPass &materialPass)
{
    ShaderInstance &shader = m_shaderInstance;
    Context *glContext = getScene()->getContext();

    if ((m_initMode == DEFERRED) && shader.isOperational())
    {
        shader.bindShader();

        object.processAllFaces(Shadable::PREPARE_GEOMETRY);

        if (m_diffuseMap)
        {
            materialPass.assignMapSetting(MaterialPass::DIFFUSE_MAP);
            shader.setConstTexture(u_diffuseMap, materialPass.getDiffuseMap(), 0);

            object.attribute(V_TEXCOORDS_2D_1_ARRAY, V_TEXCOORDS_2D_1_ARRAY);
        }

        shader.setConstColor(u_diffuse, materialPass.getDiffuse());

        shader.setConstMatrix4(
                u_modelViewProjectionMatrix,
                False,
                glContext->modelViewProjection());

        shader.setConstMatrix4(
                u_worldMatrix,
                False,
                object.getObjectWorldMatrix());

        shader.setConstMatrix3(
                u_normalMatrix,
                False,
                object.getObjectWorldMatrix().getRotation().invert().transposeTo());

        object.attribute(V_VERTICES_ARRAY, V_VERTICES_ARRAY);
        object.attribute(V_NORMALS_ARRAY, V_NORMALS_ARRAY);

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

        if (m_diffuseMap)
        {
            glContext->setActiveTextureUnit(0);
            glContext->bindTexture(TEXTURE_2D, 0);

            glContext->disableVertexAttribArray(V_TEXCOORDS_2D_1_ARRAY);
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

        glContext->disableVertexAttribArray(V_NORMALS_ARRAY);
        glContext->disableVertexAttribArray(V_VERTICES_ARRAY);
        shader.unbindShader();
    }
}


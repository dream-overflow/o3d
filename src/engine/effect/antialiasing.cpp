/**
 * @file antialiasing.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/effect/antialiasing.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/context.h"

using namespace o3d;

AntiAliasing::AntiAliasing(BaseObject *parent) :
    SceneEntity(parent),
    m_pixelSize(1.0, 1.0),
    m_weight(1.0),
    m_vertices(getScene()->getContext())
{
    const Float vertices[] = {	0.0f, 0.0f, 0.0f, 1.0f,
                                1.0f, 0.0f, 0.0f, 1.0f,
                                0.0f, 1.0f, 0.0f, 1.0f,
                                1.0f, 1.0f, 0.0f, 1.0f };

    m_vertices.create(16, VertexBuffer::STATIC, vertices);

    // ortho projection
    m_modelviewProj.buildOrtho(
                0.0f, 1.0f,  // left right
                0.0f, 1.0f,  // bottom top
               -1.0f, 1.0f);

    Shader *shader = getScene()->getShaderManager()->addShader("effect/antialiasing");
    O3D_ASSERT(shader);

    shader->buildInstance(m_shader.shader);

    m_shader.shader.assign("default", "default");

    m_shader.u_modelViewProjectionMatrix = m_shader.shader.getUniformLocation("u_modelViewProjectionMatrix");
    m_shader.u_colorMap = m_shader.shader.getUniformLocation("u_colorMap");
    m_shader.u_normalMap = m_shader.shader.getUniformLocation("u_normalMap");
    //m_shader.u_depthMap = m_shader.shader.getUniformLocation("u_depthMap");

    m_shader.u_screenSize = m_shader.shader.getUniformLocation("u_screenSize");

    m_shader.u_pixelSize = m_shader.shader.getUniformLocation("u_pixelSize");
    m_shader.u_weight = m_shader.shader.getUniformLocation("u_weight");
}

AntiAliasing::~AntiAliasing()
{

}

void AntiAliasing::process(const Vector2i &size,
                           Texture2D *depthMap,
                           Texture2D *normalMap,
                           Texture2D *colorMap)
{
    Context &context = *getScene()->getContext();
    m_pixelSize.set(1.0 / size.x(), 1.0 / size.y());

    context.blending().setDefaultFunc();
    context.enableDepthTest();
    context.disableDepthWrite();
    context.setDepthFunc(COMP_ALWAYS);

    m_shader.shader.bindShader();
        m_vertices.attribute(V_VERTICES_ARRAY, 4, 0, 0);

        m_shader.shader.setConstVector2(m_shader.u_screenSize, Vector2f(size));

        m_shader.shader.setConstMatrix4(m_shader.u_modelViewProjectionMatrix, False, m_modelviewProj);

        m_shader.shader.setConstVector2(m_shader.u_pixelSize, m_pixelSize);
        m_shader.shader.setConstFloat(m_shader.u_weight, m_weight);

//        m_shader.shader.setConstTexture(m_shader.u_depthMap, depthMap, 0);
        m_shader.shader.setConstTexture(m_shader.u_normalMap, normalMap, 1);
        m_shader.shader.setConstTexture(m_shader.u_colorMap, colorMap, 2);

        getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);
    m_shader.shader.unbindShader();
}

void AntiAliasing::setPixelSize(const Vector2f &ps)
{
    O3D_ASSERT(ps.x() > 0.0 && ps.y() > 0.0);
    m_pixelSize = ps;
}

void AntiAliasing::setWeight(Float w)
{
    O3D_ASSERT(w >= 0.0);
    m_weight = w;
}


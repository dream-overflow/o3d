/**
 * @file deferreddrawer.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/engine/glextdefines.h"
#include "o3d/engine/deferred/deferreddrawer.h"
#include "o3d/engine/object/primitive.h"
#include "o3d/engine/object/geometrydata.h"

#include "o3d/core/gl.h"
#include "o3d/engine/context.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/scene/sceneobjectmanager.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/object/light.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/renderer.h"
#include "o3d/engine/viewport.h"
#include "o3d/geom/frustum.h"
#include "o3d/engine/effect/specialeffectsmanager.h"
#include "o3d/engine/visibility/visibilitymanager.h"
#include "o3d/engine/landscape/landscape.h"
#include "o3d/engine/alphapipeline.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/vertexbuffer.h"
#include "o3d/engine/shader/shadermanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS2(DeferredDrawer, ENGINE_DEFERRED_DRAWER, SceneDrawer, ShadowRenderer)

DeferredDrawer::PointLight::PointLight(BaseObject *parent) :
    geometry(parent)
{
    Sphere sphere(1.f, 16, 16, Sphere::FILLED_MODE);
    geometry = new GeometryData(parent, sphere);

    geometry->create();
    geometry->bindFaceArray(0);
}

DeferredDrawer::DirectLight::DirectLight(BaseObject *parent)
{

}

DeferredDrawer::SpotLight::SpotLight(BaseObject *parent) :
    geometry(parent)
{
    Cylinder cone(1.f, 0.f, 1.f, 16, 16, Cylinder::FILLED_MODE);
    geometry = new GeometryData(parent, cone);

    geometry->create();
    geometry->bindFaceArray(0);
    // need to rotate by -PI on X and translate by its length on Y
}

DeferredDrawer::DeferredDrawer(BaseObject *parent) :
    SceneDrawer(parent),
    m_gbuffer(this),
    m_vertices(getScene()->getContext()),
    m_texCoords(getScene()->getContext()),
    m_blitting(True),
    m_lightGeometry(True),
    m_pointLight(this),
    m_directLight(this),
    m_spotLight(this),
    m_AA(this)
{
    const Float texCoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    const Float vertices[] = {	0.0f, 0.0f, 0.0f, 1.0f,
                                1.0f, 0.0f, 0.0f, 1.0f,
                                0.0f, 1.0f, 0.0f, 1.0f,
                                1.0f, 1.0f, 0.0f, 1.0f };

    m_vertices.create(16, VertexBuffer::STATIC, vertices);
    m_texCoords.create(8, VertexBuffer::STATIC, texCoords);

    // ortho projection
    m_modelviewProj.buildOrtho(
                0.0f, 1.0f,  // left right
                0.0f, 1.0f,  // bottom top
               -1.0f, 1.0f);

    //
    // shader used to render the GBuffer
    //
    Shader *shader = getScene()->getShaderManager()->addShader("deferred");
    O3D_ASSERT(shader);

    shader->buildInstance(m_backShader.shader);

    m_backShader.shader.assign("gbuffer", "gbuffer");

    m_backShader.u_modelViewProjectionMatrix = m_backShader.shader.getUniformLocation("u_modelViewProjectionMatrix");
    m_backShader.u_colorMap = m_backShader.shader.getUniformLocation("u_colorMap");
    m_backShader.u_depthMap = m_backShader.shader.getUniformLocation("u_depthMap");

    //
    // shader used to render the stencil to the GBuffer
    //
    shader = getScene()->getShaderManager()->addShader("deferred");
    O3D_ASSERT(shader);

    shader->buildInstance(m_stencilShader.shader);

    m_stencilShader.shader.assign("stencil", "stencil");

    m_stencilShader.u_modelViewProjectionMatrix = m_stencilShader.shader.getUniformLocation("u_modelViewProjectionMatrix");

    //
    // shader used to ambient illumination
    //
    shader = getScene()->getShaderManager()->addShader("deferred");
    O3D_ASSERT(shader);

    shader->buildInstance(m_ambientShader.shader);

    m_ambientShader.shader.assign("ambient", "ambient");

    m_ambientShader.u_screenSize = m_ambientShader.shader.getUniformLocation("u_screenSize");

    m_ambientShader.u_modelViewProjectionMatrix = m_ambientShader.shader.getUniformLocation("u_modelViewProjectionMatrix");
    m_ambientShader.u_ambientMap = m_ambientShader.shader.getUniformLocation("u_ambientMap");
    //m_ambientShader.u_diffuseMap = m_ambientShader.shader.getUniformLocation("u_diffuseMap");
    m_ambientShader.u_ambient = m_ambientShader.shader.getUniformLocation("u_ambient");

    //
    // shader used to illuminate with a point light
    //
    shader = getScene()->getShaderManager()->addShader("deferred");
    O3D_ASSERT(shader);

    shader->buildInstance(m_pointLight.shader);

    m_pointLight.shader.assign("pointlight", "pointlight");

    m_pointLight.u_screenSize = m_pointLight.shader.getUniformLocation("u_screenSize");

    m_pointLight.u_lightDiffuse = m_pointLight.shader.getUniformLocation("u_lightDiffuse");
    m_pointLight.u_lightSpecular = m_pointLight.shader.getUniformLocation("u_lightSpecular");
    m_pointLight.u_lightPos = m_pointLight.shader.getUniformLocation("u_lightPos");
    m_pointLight.u_lightAtt = m_pointLight.shader.getUniformLocation("u_lightAtt");

    m_pointLight.u_eyeVec = m_pointLight.shader.getUniformLocation("u_eyeVec");

    m_pointLight.u_modelViewProjectionMatrix = m_pointLight.shader.getUniformLocation("u_modelViewProjectionMatrix");
    m_pointLight.u_diffuseMap = m_pointLight.shader.getUniformLocation("u_diffuseMap");
    //m_pointLightShader.u_depthMap = m_pointLightShader.shader.getUniformLocation("u_depthMap");
    m_pointLight.u_specularMap = m_pointLight.shader.getUniformLocation("u_specularMap");
    m_pointLight.u_normalMap = m_pointLight.shader.getUniformLocation("u_normalMap");
    m_pointLight.u_positionMap = m_pointLight.shader.getUniformLocation("u_positionMap");

    //
    // shader used to illuminate with a directionnal light
    //
    shader = getScene()->getShaderManager()->addShader("deferred");
    O3D_ASSERT(shader);

    shader->buildInstance(m_directLight.shader);

    m_directLight.shader.assign("directLight", "directLight");

    m_directLight.u_screenSize = m_directLight.shader.getUniformLocation("u_screenSize");

    m_directLight.u_lightDiffuse = m_directLight.shader.getUniformLocation("u_lightDiffuse");
    m_directLight.u_lightSpecular = m_directLight.shader.getUniformLocation("u_lightSpecular");
    m_directLight.u_lightDir = m_directLight.shader.getUniformLocation("u_lightDir");
    m_directLight.u_eyeVec = m_directLight.shader.getUniformLocation("u_eyeVec");

    m_directLight.u_modelViewProjectionMatrix = m_directLight.shader.getUniformLocation("u_modelViewProjectionMatrix");
    m_directLight.u_diffuseMap = m_directLight.shader.getUniformLocation("u_diffuseMap");
    //m_directLight.u_depthMap = m_directLight.shader.getUniformLocation("u_depthMap");
    m_directLight.u_specularMap = m_directLight.shader.getUniformLocation("u_specularMap");
    m_directLight.u_normalMap = m_directLight.shader.getUniformLocation("u_normalMap");
    m_directLight.u_positionMap = m_directLight.shader.getUniformLocation("u_positionMap");

    //
    // shader used to illuminate with a spot light
    //
    shader = getScene()->getShaderManager()->addShader("deferred");
    O3D_ASSERT(shader);

    shader->buildInstance(m_spotLight.shader);

    m_spotLight.shader.assign("spotlight", "spotlight");

    m_spotLight.u_screenSize = m_spotLight.shader.getUniformLocation("u_screenSize");

    m_spotLight.u_lightDiffuse = m_spotLight.shader.getUniformLocation("u_lightDiffuse");
    m_spotLight.u_lightSpecular = m_spotLight.shader.getUniformLocation("u_lightSpecular");
    m_spotLight.u_lightPos = m_spotLight.shader.getUniformLocation("u_lightPos");
    m_spotLight.u_lightDir = m_spotLight.shader.getUniformLocation("u_lightDir");
    m_spotLight.u_lightAtt = m_spotLight.shader.getUniformLocation("u_lightAtt");
    m_spotLight.u_lightCosCutOff = m_spotLight.shader.getUniformLocation("u_lightCosCutOff");
    m_spotLight.u_lightExponent = m_spotLight.shader.getUniformLocation("u_lightExponent");

    m_spotLight.u_eyeVec = m_spotLight.shader.getUniformLocation("u_eyeVec");

    m_spotLight.u_modelViewProjectionMatrix = m_spotLight.shader.getUniformLocation("u_modelViewProjectionMatrix");
    m_spotLight.u_diffuseMap = m_spotLight.shader.getUniformLocation("u_diffuseMap");
    //m_spotLight.u_depthMap = m_spotLight.shader.getUniformLocation("u_depthMap");
    m_spotLight.u_specularMap = m_spotLight.shader.getUniformLocation("u_specularMap");
    m_spotLight.u_normalMap = m_spotLight.shader.getUniformLocation("u_normalMap");
    m_spotLight.u_positionMap = m_spotLight.shader.getUniformLocation("u_positionMap");
}

void DeferredDrawer::setGBuffer(GBuffer *gbuffer)
{
    if (!gbuffer->isValid()) {
        O3D_ERROR(E_InvalidParameter("GBuffer must be valid. Create it before."));
    }

    m_gbuffer = gbuffer;
}

void DeferredDrawer::processLight(Light *light)
{
    if ((light != nullptr) && light->getActivity()) {
        Context &context = *getScene()->getContext();
        Camera &camera = *getScene()->getActiveCamera();

        if (light->getShadowCast()) {
            // TODO
        }

        // With light geometry we use the stencil technique.
        // it is possible to improve the case where the light volume intersect the far clip
        // plane and in this case to draw front faces

        // TODO: offer a variant of the light volume without stencil pass, only testing
        // backface, depth, and check for camera/light volume intersections

        // TODO: offer a possibility to pass many lights in a single time.
        // this case cannot works with light volume but it could be interesting for
        // rendering a lot of little lights.
        // We can use light volumes if we are sure they does not overlap between them.
        // so depending of a scene manager that should defines which lights and in what
        // order to apply them.
        // The shader could apply mani lights too (of the same type)

        // TODO: see if it is interesting to defines a material ID into the GBuffer,
        // to have more materials parameters in somes cases.

        // TODO: Shadow volume and shadow maps with deferred.
        // TODO: Applying precomputed light map with deferred.

        if (light->getLightType() == Light::DIRECTIONAL_LIGHT) {
            m_directLight.shader.bindShader();
                m_vertices.attribute(V_VERTICES_ARRAY, 4, 0, 0);

                m_directLight.shader.setConstMatrix4(m_directLight.u_modelViewProjectionMatrix, False, m_modelviewProj);
                m_directLight.shader.setConstVector2(
                            m_directLight.u_screenSize,
                            Vector2f(m_gbuffer->getDimension().x(), m_gbuffer->getDimension().y()));

                m_directLight.shader.setConstVector3(m_directLight.u_eyeVec, camera.getObjectWorldMatrix().getTranslation());
                m_directLight.shader.setConstVector3(m_directLight.u_lightDir, light->getWorldDirection());

                m_directLight.shader.setConstColor(m_directLight.u_lightDiffuse, light->getDiffuse());
                m_directLight.shader.setConstColor(m_directLight.u_lightSpecular, light->getSpecular());

                m_directLight.shader.setConstTexture(m_directLight.u_diffuseMap, m_gbuffer->getDiffuseMap(), 0);
                m_directLight.shader.setConstTexture(m_directLight.u_specularMap, m_gbuffer->getSpecularMap(), 1);
                m_directLight.shader.setConstTexture(m_directLight.u_normalMap, m_gbuffer->getNormalMap(), 2);
                m_directLight.shader.setConstTexture(m_directLight.u_positionMap, m_gbuffer->getPositionMap(), 3);
                //m_directLightShader.shader.setConstTexture(m_directLightShader.u_depthMap, m_gbuffer->getDepthMap(), 4);

                getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);
            m_directLight.shader.unbindShader();
        } else if (light->getLightType() == Light::POINT_LIGHT) {
            // use two pass, one with stencil, second with lighting
            if (m_lightGeometry) {
                // disable back face culling
                context.setCullingMode(CULLING_NONE);
                context.enableDepthTest();
                context.enableStencilTest();
                context.disableColorWrite();
                context.blending().setFunc(Blending::DISABLED);
                context.setStencilTestFunc(COMP_ALWAYS, 0, 0);

                glClear(GL_STENCIL_BUFFER_BIT);

                glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

                Float radius = light->getRadius();
                // printf("point=%f\n", radius);

                Matrix4 mdl = camera.getModelviewMatrix() * light->getObjectWorldMatrix();

                Matrix4 m;
                m.scale(Vector3(radius, radius, radius));
                mdl *= m;

                Matrix4 mvp = camera.getProjectionMatrix() * mdl;

                m_stencilShader.shader.bindShader();
                m_pointLight.geometry->attribute(V_VERTICES_ARRAY, V_VERTICES_ARRAY);

                m_stencilShader.shader.setConstMatrix4(m_stencilShader.u_modelViewProjectionMatrix, False, mvp);

                m_pointLight.geometry->draw();
                m_stencilShader.shader.unbindShader();

                context.disableDepthTest();
                context.disableDoubleSide();
                context.enableColorWrite();
                context.blending().setFunc(Blending::ONE__ONE);
                context.setStencilTestFunc(COMP_NOTEQUAL, 0, 0xff);

                context.setCullingMode(CULLING_FRONT_FACE);

                m_pointLight.shader.bindShader();
                m_pointLight.geometry->attribute(V_VERTICES_ARRAY, V_VERTICES_ARRAY);

                m_pointLight.shader.setConstMatrix4(m_pointLight.u_modelViewProjectionMatrix, False, mvp);

                m_pointLight.shader.setConstVector2(
                            m_pointLight.u_screenSize,
                            Vector2f(m_gbuffer->getDimension().x(), m_gbuffer->getDimension().y()));

                m_pointLight.shader.setConstVector3(m_pointLight.u_eyeVec, camera.getObjectWorldMatrix().getTranslation());
                m_pointLight.shader.setConstVector4(m_pointLight.u_lightPos, light->getWorldPosition());
                m_pointLight.shader.setConstVector3(m_pointLight.u_lightAtt, light->getAttenuation());

                m_pointLight.shader.setConstColor(m_pointLight.u_lightDiffuse, light->getDiffuse());
                m_pointLight.shader.setConstColor(m_pointLight.u_lightSpecular, light->getSpecular());

                m_pointLight.shader.setConstTexture(m_pointLight.u_diffuseMap, m_gbuffer->getDiffuseMap(), 0);
                m_pointLight.shader.setConstTexture(m_pointLight.u_specularMap, m_gbuffer->getSpecularMap(), 1);
                m_pointLight.shader.setConstTexture(m_pointLight.u_normalMap, m_gbuffer->getNormalMap(), 2);
                m_pointLight.shader.setConstTexture(m_pointLight.u_positionMap, m_gbuffer->getPositionMap(), 3);
                //m_pointLightShader.shader.setConstTexture(m_pointLightShader.u_depthMap, m_gbuffer->getDepthMap(), 2);

                m_pointLight.geometry->draw();
                m_pointLight.shader.unbindShader();

                context.setCullingMode(CULLING_BACK_FACE);

                context.disableDepthTest();
                context.disableDoubleSide();
                context.disableStencilTest();
            } else {
                // single pass on the entiere screen
                m_pointLight.shader.bindShader();
                m_vertices.attribute(V_VERTICES_ARRAY, 4, 0, 0);

                m_pointLight.shader.setConstMatrix4(m_pointLight.u_modelViewProjectionMatrix, False, m_modelviewProj);

                m_pointLight.shader.setConstVector2(
                            m_pointLight.u_screenSize,
                            Vector2f(m_gbuffer->getDimension().x(), m_gbuffer->getDimension().y()));

                m_pointLight.shader.setConstVector3(m_pointLight.u_eyeVec, camera.getObjectWorldMatrix().getTranslation());
                m_pointLight.shader.setConstVector4(m_pointLight.u_lightPos, light->getWorldPosition());
                m_pointLight.shader.setConstVector3(m_pointLight.u_lightAtt, light->getAttenuation());

                m_pointLight.shader.setConstColor(m_pointLight.u_lightDiffuse, light->getDiffuse());
                m_pointLight.shader.setConstColor(m_pointLight.u_lightSpecular, light->getSpecular());

                m_pointLight.shader.setConstTexture(m_pointLight.u_diffuseMap, m_gbuffer->getDiffuseMap(), 0);
                m_pointLight.shader.setConstTexture(m_pointLight.u_specularMap, m_gbuffer->getSpecularMap(), 1);
                m_pointLight.shader.setConstTexture(m_pointLight.u_normalMap, m_gbuffer->getNormalMap(), 2);
                m_pointLight.shader.setConstTexture(m_pointLight.u_positionMap, m_gbuffer->getPositionMap(), 3);
                //m_pointLightShader.shader.setConstTexture(m_pointLightShader.u_depthMap, m_gbuffer->getDepthMap(), 2);

                getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);
                m_pointLight.shader.unbindShader();
            }
        } else if (light->getLightType() == Light::SPOT_LIGHT) {
            // use two pass, one with stencil, second with lighting
            if (m_lightGeometry) {
                // disable back face culling
                context.setCullingMode(CULLING_NONE);
                context.enableDepthTest();
                context.enableStencilTest();
                context.disableColorWrite();
                context.blending().setFunc(Blending::DISABLED);
                context.setStencilTestFunc(COMP_ALWAYS, 0, 0);

                glClear(GL_STENCIL_BUFFER_BIT);

                glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

                Matrix4 mdl = camera.getModelviewMatrix() * light->getObjectWorldMatrix();

                Float length = light->getLength();
                Float radius = light->getRadius();

                // System::print(String::print("r=%f l=%f\n", radius, length));

                // Directional wire cone (Z aligned)
                Matrix4 m;
                m.translate(Vector3(0, 0, length));
                mdl *= m;

                m.identity();
                m.rotateX(o3d::toRadian(-90.f));
                mdl *= m;

                m.identity();
                m.scale(Vector3(radius, length, radius));
                mdl *= m;

                Matrix4 mvp = camera.getProjectionMatrix() * mdl;

                m_stencilShader.shader.bindShader();
                m_spotLight.geometry->attribute(V_VERTICES_ARRAY, V_VERTICES_ARRAY);

                m_stencilShader.shader.setConstMatrix4(m_stencilShader.u_modelViewProjectionMatrix, False, mvp);

                m_spotLight.geometry->draw();
                m_stencilShader.shader.unbindShader();

                context.disableDepthTest();
                context.disableDoubleSide();
                context.enableColorWrite();
                context.blending().setFunc(Blending::ONE__ONE);
                context.setStencilTestFunc(COMP_NOTEQUAL, 0, 0xff);

                context.setCullingMode(CULLING_FRONT_FACE);

                m_spotLight.shader.bindShader();
                m_spotLight.geometry->attribute(V_VERTICES_ARRAY, V_VERTICES_ARRAY);

                m_spotLight.shader.setConstMatrix4(m_spotLight.u_modelViewProjectionMatrix, False, mvp);

                m_spotLight.shader.setConstVector2(
                            m_spotLight.u_screenSize,
                            Vector2f(m_gbuffer->getDimension().x(), m_gbuffer->getDimension().y()));

                m_spotLight.shader.setConstVector3(m_spotLight.u_eyeVec, camera.getObjectWorldMatrix().getTranslation());
                m_spotLight.shader.setConstVector4(m_spotLight.u_lightPos, light->getWorldPosition());
                m_spotLight.shader.setConstVector3(m_spotLight.u_lightDir, light->getWorldDirection());
                m_spotLight.shader.setConstVector3(m_spotLight.u_lightAtt, light->getAttenuation());
                m_spotLight.shader.setConstFloat(m_spotLight.u_lightCosCutOff, cosf(o3d::toRadian(light->getCutOff())));
                m_spotLight.shader.setConstFloat(m_spotLight.u_lightExponent, light->getExponent());

                m_spotLight.shader.setConstColor(m_spotLight.u_lightDiffuse, light->getDiffuse());
                m_spotLight.shader.setConstColor(m_spotLight.u_lightSpecular, light->getSpecular());

                m_spotLight.shader.setConstTexture(m_spotLight.u_diffuseMap, m_gbuffer->getDiffuseMap(), 0);
                m_spotLight.shader.setConstTexture(m_spotLight.u_specularMap, m_gbuffer->getSpecularMap(), 1);
                m_spotLight.shader.setConstTexture(m_spotLight.u_normalMap, m_gbuffer->getNormalMap(), 2);
                m_spotLight.shader.setConstTexture(m_spotLight.u_positionMap, m_gbuffer->getPositionMap(), 3);
                //m_spotLight.shader.setConstTexture(m_spotLight.u_depthMap, m_gbuffer->getDepthMap(), 2);

                m_spotLight.geometry->draw();
                m_spotLight.shader.unbindShader();

                context.setCullingMode(CULLING_BACK_FACE);

                context.disableDepthTest();
                context.disableDoubleSide();
                context.disableStencilTest();
            } else {
                // single pass on the whole screen
                m_spotLight.shader.bindShader();
                m_vertices.attribute(V_VERTICES_ARRAY, 4, 0, 0);

                m_spotLight.shader.setConstMatrix4(m_spotLight.u_modelViewProjectionMatrix, False, m_modelviewProj);

                m_spotLight.shader.setConstVector2(
                            m_spotLight.u_screenSize,
                            Vector2f(m_gbuffer->getDimension().x(), m_gbuffer->getDimension().y()));

                m_spotLight.shader.setConstVector3(m_spotLight.u_eyeVec, camera.getObjectWorldMatrix().getTranslation());
                m_spotLight.shader.setConstVector4(m_spotLight.u_lightPos, light->getWorldPosition());
                m_spotLight.shader.setConstVector3(m_spotLight.u_lightDir, light->getWorldDirection());
                m_spotLight.shader.setConstVector3(m_spotLight.u_lightAtt, light->getAttenuation());
                m_spotLight.shader.setConstFloat(m_spotLight.u_lightCosCutOff, cosf(o3d::toRadian(light->getCutOff())));
                m_spotLight.shader.setConstFloat(m_spotLight.u_lightExponent, light->getExponent());

                m_spotLight.shader.setConstColor(m_spotLight.u_lightDiffuse, light->getDiffuse());
                m_spotLight.shader.setConstColor(m_spotLight.u_lightSpecular, light->getSpecular());

                m_spotLight.shader.setConstTexture(m_spotLight.u_diffuseMap, m_gbuffer->getDiffuseMap(), 0);
                m_spotLight.shader.setConstTexture(m_spotLight.u_specularMap, m_gbuffer->getSpecularMap(), 1);
                m_spotLight.shader.setConstTexture(m_spotLight.u_normalMap, m_gbuffer->getNormalMap(), 2);
                m_spotLight.shader.setConstTexture(m_spotLight.u_positionMap, m_gbuffer->getPositionMap(), 3);
                //m_spotLight.shader.setConstTexture(m_spotLight.u_depthMap, m_gbuffer->getDepthMap(), 2);

                getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);
                m_spotLight.shader.unbindShader();
            }
        } else if (light->getLightType() == Light::LIGHT_MAP) {
            // @todo
        }
    }
}

void DeferredDrawer::draw(ViewPort */*viewPort*/)
{
    // The camera modelview should be set before draw()
    if (getScene()->getActiveCamera() == nullptr) {
        return;
    }

    // GBuffer is necessary
    if (!m_gbuffer || !m_gbuffer->isValid()) {
        return;
    }

    Context &context = *getScene()->getContext();
    Camera &camera = *getScene()->getActiveCamera();

    // Computes frustum just after camera put
    getScene()->getFrustum()->computeFrustum(context.projection().get(), camera.getModelviewMatrix());

    // Determines visible objects
    getScene()->getVisibilityManager()->processVisibility();

    // use MSAA if MS renderer
    if (m_gbuffer->getNumSamples() > 1) {
        context.setAntiAliasing(Context::AA_MULTI_SAMPLE);
    }

    // resize the GBuffer if necessary
    if (m_gbuffer->getDimension() != context.getViewPort().size()) {
        m_gbuffer->reshape(context.getViewPort().size());
    }

    m_gbuffer->bind();

    // clear all buffer
    m_gbuffer->clear();

    // draw buffer to attributes mode
    m_gbuffer->drawAttributes();

    //
    // Draw first pass with materials attributes, in camera space
    //

    // draw the first pass with depth and material attributes
    DrawInfo drawInfo(DrawInfo::DEFERRED_PASS);
    drawInfo.setFromCamera(&camera);

    context.disableStencilTest();
    context.enableDepthWrite();
    context.setDefaultDepthTest();
    context.setDefaultDepthFunc();

    // landscape @todo drawInfo
    getScene()->getLandscape()->draw();

    // world objects
    getScene()->getVisibilityManager()->draw(drawInfo);

    //
    // Process an ambient pass
    //

    context.disableDepthWrite();
    context.disableDepthTest();

    m_gbuffer->drawLigthing();

    m_ambientShader.shader.bindShader();
        m_vertices.attribute(V_VERTICES_ARRAY, 4, 0, 0);

        m_ambientShader.shader.setConstVector2(
                    m_ambientShader.u_screenSize,
                    Vector2f(m_gbuffer->getDimension().x(), m_gbuffer->getDimension().y()));

        m_ambientShader.shader.setConstMatrix4(m_ambientShader.u_modelViewProjectionMatrix, False, m_modelviewProj);
        m_ambientShader.shader.setConstColor(m_ambientShader.u_ambient, getScene()->getGlobalAmbient());

        // take the diffuse map for generate an ambient
        //m_ambientShader.shader.setConstTexture(m_ambientShader.u_ambientMap, m_gbuffer->getDiffuseMap(), 0);
        m_ambientShader.shader.setConstTexture(m_ambientShader.u_ambientMap, m_gbuffer->getAmbientMap(), 0);

        getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);
    m_ambientShader.shader.unbindShader();

    //
    // Process effectives lights
    //

    context.disableDepthTest();

    context.blending().setFunc(Blending::ONE__ONE);

    const TemplateArray<Light*> lights = getScene()->getVisibilityManager()->getEffectiveLights();
    for (Int32 i = 0; i < lights.getSize(); ++i) {
        processLight(lights[i]);
    }

    m_gbuffer->resetDrawBuffers();

    //
    // Post effects
    //

    // @todo maybe the post effect can replace the last step of blitting/drawing
    if (m_gbuffer->getAuxColorMap() &&
        m_gbuffer->getAuxColorMap()->getTextureType() == TEXTURE_2D &&
        m_gbuffer->getNumSamples() <= 1) {
        // draw to auxiliary buffer
        const GLenum color6[] = {GL_COLOR_ATTACHMENT6};
        glDrawBuffers(1, color6);

        m_AA.process(m_gbuffer->getDimension(),
                     (Texture2D*)m_gbuffer->getDepthMap(),
                     (Texture2D*)m_gbuffer->getNormalMap(),
                     (Texture2D*)m_gbuffer->getColorMap());

        // swap current color buffer with auxiliary
        m_gbuffer->swapColorMap();

        const GLenum color0[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, color0);
    }

    m_gbuffer->unbind();

    //
    // Render the GBuffer to back buffer and depth buffer
    //

//    if (m_gbuffer->getNumSamples() > 1) {
//        context.setAntiAliasing(Context::AA_NONE);
//    }

    // @todo if MSAA need to have a shaders supporting them
    // and same for the non blitting method shader

    // @todo how to have blit working with GLES
    if (GL::getType() != GL::API_GLES_3 && m_blitting) { // && m_gbuffer->getNumSamples() <= 1) {
        m_gbuffer->draw();
    } else {
        // use the same projection as lighting
        context.blending().setDefaultFunc();
        context.enableDepthTest();
        context.enableDepthWrite();
        context.setDepthFunc(COMP_ALWAYS);

        m_backShader.shader.bindShader();
        m_vertices.attribute(V_VERTICES_ARRAY, 4, 0, 0);
        m_texCoords.attribute(V_TEXCOORDS_2D_1_ARRAY, 2, 0, 0);

        m_backShader.shader.setConstMatrix4(m_backShader.u_modelViewProjectionMatrix, False, m_modelviewProj);

        m_backShader.shader.setConstTexture(m_backShader.u_colorMap, m_gbuffer->getColorMap(), 1);
        m_backShader.shader.setConstTexture(m_backShader.u_depthMap, m_gbuffer->getDepthMap(), 2);

        getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);
        m_backShader.shader.unbindShader();
    }

    context.disableStencilTest();
    context.setDefaultStencilTestFunc();
    context.setDefaultDepthTest();
    context.setDefaultDepthWrite();
    context.setDefaultDepthFunc();
    context.setDefaultDepthRange();

    // restore default AA settings
    if (m_gbuffer->getNumSamples() > 1) {
        context.setDefaultAntiAliasing();
    }

    //
    // Special effects
    //

    drawInfo.pass = DrawInfo::AMBIENT_PASS;

    getScene()->getSpecialEffectsManager()->draw(drawInfo);

    context.disableDepthWrite();
        getScene()->getAlphaPipeline()->sort();
        getScene()->getAlphaPipeline()->draw(drawInfo);
    context.setDefaultDepthWrite();

    // camera clear
    camera.clearCameraChanged();
}

void DeferredDrawer::drawPicking(ViewPort *)
{
    // The camera modelview should be set before draw()
    if (getScene()->getActiveCamera() == nullptr) {
        return;
    }

    Camera &camera = *getScene()->getActiveCamera();

    DrawInfo drawInfo(DrawInfo::PICKING_PASS);
    drawInfo.setFromCamera(&camera);

    // world objects
    getScene()->getVisibilityManager()->draw(drawInfo);

    // TODO
    //getScene()->getContext()->disableDepthWrite();
    //getScene()->getAlphaPipeline()->sort();
    //getScene()->getAlphaPipeline()->draw(drawInfo);
    //getScene()->getContext()->setDefaultDepthWrite();

    // camera clear
    camera.clearCameraChanged();
}

void DeferredDrawer::setPolicyLevel(UInt32 level)
{
    // TODO
}

UInt32 DeferredDrawer::getPolicyLevel() const
{
    return 0;
}

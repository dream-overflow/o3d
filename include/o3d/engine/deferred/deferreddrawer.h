/**
 * @file deferreddrawer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DEFERREDDRAWER_H
#define _O3D_DEFERREDDRAWER_H

#include "gbuffer.h"
#include "../shadow/shadowrenderer.h"
#include "../scene/scenedrawer.h"
#include "../shader/shader.h"
#include "../vertexbuffer.h"
#include "../effect/antialiasing.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Scene drawer specialized for the deferred shading and shadow map.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-01-31
 */
class O3D_API DeferredDrawer :
        public SceneDrawer,
        public ShadowRenderer
{
public:

    O3D_DECLARE_DYNAMIC_CLASS(DeferredDrawer)

    //! Constructor. Take a parent objets.
    DeferredDrawer(BaseObject *parent);

    //! Defines the GBuffer (can be shared, ref counting).
    void setGBuffer(GBuffer *gbuffer);

    //! Get the related GBuffer.
    inline GBuffer* getGBuffer() { return m_gbuffer.get(); }
    //! Get the related GBuffer (const version).
    inline GBuffer* getGBuffer() const { return m_gbuffer.get(); }

    /**
     * @brief setBlitting Set the usage of blitting to render the GBuffer to
     *        fixed (main) buffer.
     * @param use True use blitting, textures formats are the same as main buffers, False
     *        use a textured quad rendering.
     */
    inline void setBlitting(Bool use) { m_blitting = use; }

    //! Is use blitting. Default is True.
    inline Bool isBlitting() const { return m_blitting; }

    /**
     * @brief setLightGeometry Set the usage of the light geometry to permform the
     *        ligthing pass (sphere for point light, cone for spot).
     * @param use True use geometry. Default is True.
     */
    inline void setLightGeometry(Bool use) { m_lightGeometry = use; }

    //! Is use light geometry. Default is True.
    inline Bool isLightGeometry() const { return m_lightGeometry; }

    //! Process rendering of shadow caster accordings to a specified light and the scene.
    virtual void processLight(Light *light) override;

    //! Draw process.
    virtual void draw(ViewPort *viewPort) override;

    //! Process a simpler draw for the picking pass.
    virtual void drawPicking(ViewPort *viewPort) override;

    //! Define the light management policy
    //virtual void setLightPolicy(LightPolicy *policy) = 0;

    //! Get the light management policy.
    //virtual LightPolicy* getLightPolicy() = 0;

    //! Set the policy level to use. How are managed lights and object according
    //! the current policy and a specified level.
    virtual void setPolicyLevel(UInt32 level) override;

    //! Get the current policy level to use.
    virtual UInt32 getPolicyLevel() const override;

protected:

    SmartObject<GBuffer> m_gbuffer;

    ArrayBufferf m_vertices;
    ArrayBufferf m_texCoords;

    Bool m_blitting;
    Bool m_lightGeometry;

    struct BackShader
    {
        ShaderInstance shader;

        Int32 u_modelViewProjectionMatrix;

        Int32 u_colorMap;
        Int32 u_depthMap;
    };

    BackShader m_backShader;

    struct AmbientShader
    {
        ShaderInstance shader;

        Int32 u_screenSize;

        Int32 u_ambient;

        Int32 u_modelViewProjectionMatrix;
        Int32 u_ambientMap;
        //Int32 u_diffuseMap;
    };

    AmbientShader m_ambientShader;

    struct StencilShader
    {
        ShaderInstance shader;

        Int32 u_modelViewProjectionMatrix;
    };

    StencilShader m_stencilShader;

    struct PointLight
    {
        PointLight(BaseObject *parent);

        ShaderInstance shader;

        Int32 u_screenSize;

        Int32 u_lightDiffuse;
        Int32 u_lightSpecular;
        Int32 u_lightPos;
        Int32 u_lightAtt;

        Int32 u_eyeVec;

        Int32 u_modelViewProjectionMatrix;
        Int32 u_ambientMap;
        Int32 u_diffuseMap;
        Int32 u_specularMap;
        Int32 u_normalMap;  // + shine as A
        Int32 u_depthMap;
        Int32 u_positionMap;

        SmartObject<GeometryData> geometry;    //!< A sphere to render the point light geometry
    };

    PointLight m_pointLight;

    struct DirectLight
    {
        DirectLight(BaseObject *parent);

        ShaderInstance shader;

        Int32 u_screenSize;

        Int32 u_lightDiffuse;
        Int32 u_lightSpecular;
        Int32 u_lightDir;

        Int32 u_eyeVec;

        Int32 u_modelViewProjectionMatrix;
        Int32 u_ambientMap;
        Int32 u_diffuseMap;
        Int32 u_specularMap;
        Int32 u_normalMap;  // + shine as A
        Int32 u_depthMap;
        Int32 u_positionMap;
    };

    DirectLight m_directLight;

    struct SpotLight
    {
        SpotLight(BaseObject *parent);

        ShaderInstance shader;

        Int32 u_screenSize;

        Int32 u_lightDiffuse;
        Int32 u_lightSpecular;
        Int32 u_lightPos;
        Int32 u_lightDir;
        Int32 u_lightAtt;
        Int32 u_lightCosCutOff;
        Int32 u_lightExponent;

        Int32 u_eyeVec;

        Int32 u_modelViewProjectionMatrix;
        Int32 u_ambientMap;
        Int32 u_diffuseMap;
        Int32 u_specularMap;
        Int32 u_normalMap;  // + shine as A
        Int32 u_depthMap;
        Int32 u_positionMap;

        SmartObject<GeometryData> geometry;    //!< A cone to render the spot light geometry
    };

    SpotLight m_spotLight;

    Matrix4 m_modelviewProj;

    // TODO a post effect list
    AntiAliasing m_AA;
};

} // namespace o3d

#endif // _O3D_DEFERREDDRAWER_H


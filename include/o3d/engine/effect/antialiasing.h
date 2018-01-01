/**
 * @file antialiasing.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ANTIALIASING_H
#define _O3D_ANTIALIASING_H

#include "../scene/sceneentity.h"
#include "../shader/shader.h"
#include "../drawinfo.h"
#include "../vertexbuffer.h"
#include "../framebuffer.h"

namespace o3d {

/**
 * @brief Anti aliasing post effect.
 * Uses it as a post effect, to be applied on a FBO with the input as read buffer 0.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-04-06
 */
class O3D_API AntiAliasing : public SceneEntity
{
public:

    AntiAliasing(BaseObject *parent);

    virtual ~AntiAliasing();

    /**
     * @brief process Apply the anti-aliasing effect on the current context.
     * The result is draw into the current bound buffer at draw buffer 0.
     */
    virtual void process(const Vector2i &size,
                         Texture2D* depthMap,
                         Texture2D* normalMap,
                         Texture2D* colorMap);

    /**
     * @brief setPixelSize Define the pixel sizes in x and y directions.
     * @param ps Pixel size ]0,+oo[ (default is (1,1)).
     */
    void setPixelSize(const Vector2f &ps);
    //! Get the pixel size in x and y directions (default is (1,1)).
    inline const Vector2f& getPixelSize() const { return m_pixelSize; }

    /**
     * @brief setWeight Define the anti-aliasing effect weight on each pixel.
     * @param w Weight factor ]0..oo[ (default is 1.0).
     */
    void setWeight(Float w);
    //! Get the anti-aliasing effect weight (default is 1.0).
    inline Float getWeight() const { return m_weight; }

private:

    Vector2f m_pixelSize;    //!< Pixel size (default is (1,1)).
    Float m_weight;          //!< Effect weight factor (default is 1).

    ArrayBufferf m_vertices;
    Matrix4 m_modelviewProj;

    struct AAShader
    {
        ShaderInstance shader;

        Int32 u_modelViewProjectionMatrix;
        Int32 u_screenSize;

        Int32 u_pixelSize;
        Int32 u_weight;

        Int32 u_depthMap;
        Int32 u_normalMap;
        Int32 u_colorMap;
    };

    AAShader m_shader;
};

} // namespace o3d

#endif // _O3D_ANTIALIASING_H


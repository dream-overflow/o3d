/**
 * @file gbuffer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GBUFFER_H
#define _O3D_GBUFFER_H

#include "o3d/core/smartobject.h"
#include "../framebuffer.h"
#include "../drawbuffers.h"
#include "../scene/sceneentity.h"

namespace o3d {

class Texture2DMS;

/**
 * @brief The GBuffer class. Deferred rendering GBuffer.
 * It is a FBO with multiple attachements :
 *  - final color buffer, where the result of a deferred shader must write
 *  - ambient buffer, defines the ambient RGBA
 *  - diffuse buffer, defines the diffuse RGBA
 *  - depth buffer, contains only the 32 bits float depth
 *  - normal buffer, contains RGB as XYZ components of the normal
 *  - transparency buffer, later contains...
 *  - specular buffer, contains specular RGBA
 *  - shine buffer, contains the specular power is a 8 bits integer or 32 bits float in R
 * @date 2013-01-14
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 */
class O3D_API GBuffer : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(GBuffer)

    //! Buffer types.
    enum BufferType
    {
        COLOR_BUFFER = 0,         //!< RGBA. Default is 16 bits/channel.
        AMBIENT_BUFFER = 1,       //!< RGBA. Default is 8 bits/channel.
        DEPTH_BUFFER = 2,         //!< DEPTH/STENCIL. Default is 24/8.
        NORMAL_BUFFER = 3,        //!< RGBA. Default is 16 bits/channel. A is for shine.
        DIFFUSE_BUFFER = 4,       //!< RGBA. Default is 8 bits/channel.
        SPECULAR_BUFFER = 5,      //!< RGBA. Default is 8 bits/channel.
        POSITION_BUFFER = 6,      //!< RGB. Default is 32 bits/channel.
        AUX_COLOR_BUFFER = 7,     //!< Same format as COLOR_BUFFER.
        SHININESS_BUFFER = SPECULAR_BUFFER,
        SPECULAR_POWER_BUFFER = SPECULAR_BUFFER,
        SPECULAR_EXPONENT_BUFFER = SPECULAR_BUFFER,
        TMP1_COLOR_BUFFER = AMBIENT_BUFFER,
        TMP2_COLOR_BUFFER = DIFFUSE_BUFFER
    };

    //! Max number of buffer type to manage.
    static const UInt32 NUM_BUFFERS_TYPE = 8;

    /**
     * Buffer formats. Depending of the required quality.
     * Memory usage is a direct consequence. Performances and compatibility will depends
     * of the hardware and drivers.
     */
    enum BufferFormat
    {
        FORMAT_8U = 0,         //!< 8 bits unsigned integer per channel.
        FORMAT_16F = 1,        //!< 16 bits float per channel.
        FORMAT_32F = 2,        //!< 32 bits float per channel.
        FORMAT_24_8 = 3       //!< 24/8 bits for depth/stencil buffer.
    };

    //! Constructor.
    GBuffer(BaseObject *parent);

    //! Destructor, call release.
    virtual ~GBuffer();

    /**
     * @brief setFormat Set the texture format of a specific buffer.
     * @param buffer One of the valids buffers types.
     * @param format One of the compatibles buffers formats.
     * @note Some formats are not allowed on somes buffer. An exception is thrown in these
     *       cases.
     * @warning This operation is not allowed and will throws an exception if the buffer
     *          is already created.
     */
    void setFormat(BufferType buffer, BufferFormat format);

    //! Get the format of a specific buffer.
    BufferFormat getFormat(BufferType buffer) const;

    /**
     * @brief setBufferUsage Enable or disable a buffer, to use it for deferred shading.
     * Deferred shaders must take care of the state of each buffer before doing anything.
     * @note Some buffers can not be disabled. An exception is thrown in these
     *       cases : Color, Ambiant, Depth and Normal.
     * @warning This operation is not allowed and will throws an exception if the buffer
     *          is already created.
     */
    void setBufferUsage(BufferType buffer, Bool state);

    //! Get the state of usage of a specific buffer.
    Bool getBufferUsage(BufferType buffer) const;

    //! Create the textures and the FBO.
    /**
     * @brief create Create the textures and the FBO.
     * @param width Initial width.
     * @param height Initial height.
     * @param samples If multi-sampled (@see MSAA) use 2, 4 or 8, else 0 or 1 mean no MS.
     */
    void create(UInt32 width, UInt32 height, UInt32 samples = 1);

    //! Returns True if the FBO is created.
    inline Bool isValid() const { return m_fbo.isValid(); }

    //! Realease the FBO and the textures.
    void release();

    //! Get the number of samples for multi-sampled GBuffer (valid after create).
    inline UInt32 getNumSamples() const { return m_numSamples; }

    //! Reshape the GBuffer.
    void reshape(const Vector2i& size);

    //! Get the current size of the buffers.
	inline const Vector2i& getDimension() const { return m_fbo.getDimension(); }

    //! Get the final color texture.
    Texture* getColorMap() const { return m_buffers[COLOR_BUFFER].texture.get(); }

    //! Get the auxiliary color texture. Useful for post effects.
    Texture* getAuxColorMap() const { return m_buffers[AUX_COLOR_BUFFER].texture.get(); }

    //! Get the ambient texture.
    Texture* getAmbientMap() const { return m_buffers[AMBIENT_BUFFER].texture.get(); }

    //! Get the diffuse texture.
    Texture* getDiffuseMap() const { return m_buffers[DIFFUSE_BUFFER].texture.get(); }

    //! Get the specular+shine texture.
    Texture* getSpecularMap() const { return m_buffers[SPECULAR_BUFFER].texture.get(); }

    //! Get the depth texture.
    Texture* getDepthMap() const { return m_buffers[DEPTH_BUFFER].texture.get(); }

    //! Get the normal texture.
    Texture* getNormalMap() const { return m_buffers[NORMAL_BUFFER].texture.get(); }

    //! Get the position buffer texture.
    Texture* getPositionMap() const { return m_buffers[POSITION_BUFFER].texture.get(); }

    //! Clear all buffers. GBuffer must be bound.
    void clear();

    //! Clear depth buffer. GBuffer must be bound.
    void clearDepth();

    //! Clear colors buffers. GBuffer must be bound.
    void clearColors();

    //! Bind the GBuffer for processing.
    void bind();

    //! Unbind (bind current to 0).
    void unbind();

    //! Swap primary and auxiliary color maps. The buffer must be bound.
    void swapColorMap();

    /**
     * @brief drawAttributes Prepare before drawing materials attributes into the buffers.
     * Draw buffers are :
     *  - 0 o_ambient
     *  - 1 o_normal
     *  - 2 o_diffuse
     *  - 3 o_specular
     *  - and depth is attached to the FBO.
     */
    void drawAttributes();

    /**
     * @brief drawLigthing Prepare before lights processing (attachments).
     * Draw buffers are :
     *  - 0 o_finalColor
     *  - and depth is attached to the FBO.
     */
    void drawLigthing();

    /**
     * @brief drawStencil Prepare before lights with stencil processing (attachments).
     * Draw buffers are :
     *  - NONE
     *  - and depth is attached to the FBO.
     */
    void drawStencil();

    //! Go back to defaults draw buffers.
    void resetDrawBuffers();

    /**
     * @brief Perform a blitting to the fixeds (mains) color and depth buffers.
     * @note The format of the color buffer, and the depth buffer must be the same as
     *       the fixeds (mains) buffers. Else buffer must be rendered using a textured
     *       quad.
     */
    void draw();

private:

    struct Buffer
    {
        Bool actif;
        BufferType type;
        BufferFormat format;
        SmartObject<Texture> texture;
    };

    Buffer m_buffers[NUM_BUFFERS_TYPE];
    FrameBuffer m_fbo;

    UInt32 m_numSamples;

    //! Where to write attributes.
    DrawBuffers m_drawBuffers;

    PixelFormat bufferFormatToPixelFormat(BufferType buffer, BufferFormat format) const;
};

} // namespace o3d

#endif // _O3D_GBUFFER_H


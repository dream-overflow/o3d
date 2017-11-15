/**
 * @file texture2dms.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTURE2DMS_H
#define _O3D_TEXTURE2DMS_H

#include "texture.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Texture 2D multi-sample.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-04-18
 */
class O3D_API Texture2DMS : public Texture
{
public:

    Texture2DMS();

    O3D_DECLARE_CLASS(Texture2DMS)

    //! Construct a texture object.
    Texture2DMS(BaseObject *parent, const TextureOps &operations = TextureOps());

    //! Destructor.
    virtual ~Texture2DMS();


    //-----------------------------------------------------------------------------------
    // Texture creation
    //-----------------------------------------------------------------------------------

    /**
     * @brief create Create an empty texture given a size and a format.
     * @param samples Number of samples [0..MAX_SAMPLES[
     * @param width
     * @param height
     * @param pixelFormat can be one of the PixelFormat enum.
     * @param dontUnbind if true the texture is not unbound, in others words it is kept as current
     * @return If the texture already exists it return false immediately.
     */
    Bool create(
        UInt32 samples,
        UInt32 width,
        UInt32 height,
        PixelFormat pixelFormat,
        Bool dontUnbind = False);

    //! Resize the texture (min mipmap level). The texture content is not kept.
    void resize(UInt32 width, UInt32 height, Bool dontUnbind = False);

    //! Destroy the OpenGL texture, and the picture if it exists.
    virtual void destroy();

    //! Forbidden. Always return False.
    virtual Bool addMipsLevels(UInt32 minLevel, UInt32 maxLevel);

    //! Get the number of samples.
    inline UInt32 getNumSamples() const { return m_numSamples; }


    //-----------------------------------------------------------------------------------
    // Processing
    //-----------------------------------------------------------------------------------

    //! Set the texture parameter, such as enable extra coord for cubemap...
    virtual void set();
    //! Bind an opengl texture
    virtual Bool bind();
    //! unbind the texture
    virtual void unbind();
    //! Unset the texture parameter
    virtual void unSet();


    //-----------------------------------------------------------------------------------
    // Serialization
    //-----------------------------------------------------------------------------------

    virtual Bool writeToFile(OutStream &os);
    virtual Bool readFromFile(InStream &is);

protected:

    UInt32 m_numSamples;

    //! Set the filtering mode to theL texture object.
    virtual void setFilteringMode();
    //! Set the wrap mode to the texture object.
    virtual void setWrapMode();
    //! Set the anisotropy level to the texture object.
    virtual void setAnisotropyLevel();
};

} // namespace o3d

#endif // _O3D_TEXTURE2DMS_H


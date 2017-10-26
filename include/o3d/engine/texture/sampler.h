/**
 * @file sampler.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SAMPLER_H
#define _O3D_SAMPLER_H

#include "o3d/engine/scene/sceneentity.h"

namespace o3d {

class Context;
class Scene;

/**
 * @brief Sampler object for texture.
 * @author Frederic Scherma
 * @date 2014-03-16
 */
class O3D_API Sampler
{
public:

    //! Filtering mode.
    enum Filtering
    {
        NEAREST= 0x2600,                 //!< Nearest, no filtering.
        LINEAR = 0x2601,                 //!< Linear filtering.
        NEAREST_MIPMAP_NEAREST = 0x2700, //!< Nearest on sampler, nearest on mipmaps.
        LINEAR_MIPMAP_NEAREST = 0x2701,  //!< Linear on sampler, nearest on mipmaps.
        NEAREST_MIPMAP_LINEAR = 0x2702,  //!< Nearest on sampler, linear on mipmaps.
        LINEAR_MIPMAP_LINEAR = 0x2703    //!< Linear on sampler, linear on mipmaps.
    };

    //! Warp mode.
    enum Wrap
    {
        REPEAT = 0x2901,          //!< Texture repeat. that means -0.2 becomes 0.8.
        CLAMP_TO_EDGE = 0x812F,   //!< Texture clamp to edge. range [0..1].
        MIRRORED_REPEAT = 0x8370, //!< Texture coordinate wraps around like a mirror. -0.2 becomes 0.2, -1.2 becomes 0.8.
        MIRROR_CLAMP = 0x8743     //!< Texture mirrored clamp to edge. range [-1..1].
    };

    //! Comparison mode for depth and stencil samplers only.
    enum CompMode
    {
        COMP_NONE = 0,                //!< No comparisons are performed.
        COMP_REF_TO_TEXTURE = 0x884E  //!< The interpolated and clamped R texture coordinate are compared to the value in the currently bound texture.
    };

    //! Create a new sampler object.
    Sampler(Context *context);

    //! Unbind and delete.
    ~Sampler();

    //! Release the sampler object. It is no longer usable after release.
    void release();

    //! Get the gl context (read only).
    inline const Context* getContext() const { return m_context; }
    //! Get the gl context.
    inline Context* getContext() { return m_context; }

    /**
     * @brief bindSampler Bind this sampler to a valid texture unit.
     * @param unit Valid texture unit.
     */
    void bindSampler(UInt32 unit);

    /**
     * @brief unbindSampler Unbind this sampler (bind to 0) from a valid texture unit.
     * @param unit Valid texture unit.
     */
    void unbindSampler(UInt32 unit);

    //! Return the OpenGL sampler identifier.
    inline UInt32 getSamplerId() const { return m_samplerId; }

    //
    // Filtering and anisotropy
    //

    //! Set the mignifier filtering. Default is NEAREST_MIPMAP_LINEAR.
    void setMignifier(Filtering filtering);
    //! Get the mignifier filtering. Default is NEAREST_MIPMAP_LINEAR.
    inline Filtering getMignifier() const { return (Filtering)m_filtering[0]; }

    /**
     * Set the magnifier filtering. Default is LINEAR.
     * @param filtering Only NEAREST and LINEAR are allowed.
     */
    void setMagnifier(Filtering filtering);
    //! Get the magnifier filtering. Default is LINEAR.
    inline Filtering getMagnifier() const { return (Filtering)m_filtering[1]; }

    //! Set the anisotropy level. Default is 1.0 meaning isotropic filtering.
    void setMaxAnisotropy(Float degree);
    //! Get the anisotropy level. Default is 1.0 meaning isotropic filtering.
    inline Float getMaxAnisotropy() const { return m_maxAnisotropy; }

    //! Set the min/mag filtering to NEAREST isotropic.
    void setNearest();

    //! Set the min/mag filtering to LINEAR isotropic.
    void setLinear();

    /**
     * Set the min/mag filtering to bilinear and the (an)isotropy level.
     * Minifier is set to LINEAR_MIPMAP_NEAREST, magnifier is set do LINEAR.
     * @param anisotropy By default 1.0 mean isotropy. Greater value mean anisotropy.
     */
    void setBilinear(Float anisotropy = 1.0f);

    /**
     * Set the min/mag filtering to trilinear and the (an)isotropy level.
     * Minifier is set to LINEAR_MIPMAP_LINEAR, magnifier is set do LINEAR.
     * @param anisotropy By default 1.0 mean isotropy. Greater value mean anisotropy.
     */
    void setTrilinear(Float anisotropy = 1.0f);

    //
    // Wrap (repeat, clamp, mirror)
    //

    //! Set the S wrap channel (U). Default is REPEAT.
    void setWrapS(Wrap wrap);
    //! Get the S wrap channel (U). Default is REPEAT.
    inline Wrap getWrapS() const { return m_wrap[0]; }

    //! Set the T wrap channel (V). Default is REPEAT.
    void setWrapT(Wrap wrap);
    //! Get the T wrap channel (V). Default is REPEAT.
    inline Wrap getWrapT() const { return m_wrap[1]; }

    //! Set the R wrap channel (W). Default is REPEAT.
    void setWrapR(Wrap wrap);
    //! Get the R wrap channel (W). Default is REPEAT.
    inline Wrap getWrapR() const { return m_wrap[2]; }

    //! Set the STR channel (UVW) to the same mode.
    void setWrap(Wrap wrap);

    //
    // Level of detail range and bias
    //

    /**
     * @brief setLodRange Set the level of detail range.
     * @param min Min level. Default is -1000.
     * @param max Max level. Default is 1000.
     */
    void setLodRange(Float min, Float max);

    /**
     * @brief setLodRange Set the level of detail range.
     * @param range Min and max levels. Default is [-1000,1000].
     */
    void setLodRange(Float range[2]);

    //! Set the min level. Default is -1000.
    void setLodMin(Float min);
    //! Get the min level. Default is -1000.
    inline Float getLodMin() const { return m_lodRange[0]; }

    //! Set the max level. Default is 1000.
    void setLodMax(Float max);
    //! Get the max level. Default is 1000.
    inline Float getLodMax() const { return m_lodRange[1]; }

    //! Set the level of detail bias. Default is 0.0.
    void setLodBias(Float bias);
    //! Get the level of detail bias. Default is 0.0.
    inline Float getLodBias() const { return m_lodBias; }

    //
    // Comparison mode and function. Usefull for depth and stencil samplers.
    //

    /**
     * @brief setCompMode
     * @param mode
     */
    void setCompMode(CompMode mode);
    //! Get the sampler comparison mode. Default is NONE.
    CompMode getCompMode() const { return m_compMode; }

    //! Set the comparison function. Default is COMP_LEQUAL.
    void setCompFunc(Comparison comp);
    //! Get the comparison function. Default is COMP_LEQUAL.
    inline Comparison getCompFunc() const { return m_compFunc; }

private:

    Context *m_context;

    UInt32 m_samplerId;

    Int32 m_filtering[2];   //!< Sampler Min/Mag filtering. Default is LINEAR/LINEAR.
    Wrap m_wrap[3];         //!< Sampler S/T/R wrap mode. Default is REPEAT/REPEAT/REPEAT.

    Float m_maxAnisotropy;  //!< Max anisotropy level. Default is 1.0.

    Float m_lodRange[2];    //!< LOD range. Default [-1000, 1000].
    Float m_lodBias;        //!< LOD bias. Default 0.0.

    CompMode m_compMode;    //!< Sampler comparison mode. Default is NONE (0).
    Comparison m_compFunc;  //!< Sampler comparison function. Default is COMP_LEQUAL.
};

} // namespace o3d

#endif // _O3D_SAMPLER_H


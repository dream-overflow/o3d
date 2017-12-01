/**
 * @file colormaterial.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_COLORMATERIAL_H
#define _O3D_COLORMATERIAL_H

#include "material.h"

namespace o3d {

/**
 * @brief Material using the simple diffuse color and diffuse map (if one).
 * @date 2014-03-01
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * Use diffuse color or diffuse map * diffuse color to compute the fragment color.
 * No lighting is performed.
 * It supports any types of vertex mode (static mesh, rigging, skinning...).
 * @note A deferred shading pass is supported an fill only the ambient buffer.
 */
class O3D_API ColorMaterial : public Material
{
public:

    O3D_DECLARE_DYNAMIC_CLASS(ColorMaterial)

    //! Default constructor.
    ColorMaterial(BaseObject *parent);

    //! Virtual destructor.
    virtual ~ColorMaterial();

    //! Check if the material is supported by the hardware.
    virtual Bool isMaterialSupported() const;

    //! Initialize material shader according to a specific MaterialPass setting,
    //! and shadable vertex mode.
    //! @param initMode Only accept AMBIENT.
    virtual void initialize(
            InitMode initMode,
            MaterialPass &materialPass,
            Shadable &shadable);

    //! Release initialized data.
    virtual void release();

    //-----------------------------------------------------------------------------------
    // Processing
    //-----------------------------------------------------------------------------------

    //! Shadable object rendering for ambient pass.
    //! @param object Shadable object to render.
    //! @param processMode Rendering mode.
    //! @param materialPass Material pass object that contain parameters.
    virtual void processAmbient(
            Shadable &object,
            const DrawInfo &drawInfo,
            const MaterialPass &materialPass);

    //! Shadable object rendering for picking.
    //! @warning Not supported for this mode.
    virtual void processPicking(
            Shadable &object,
            Pickable &pickable,
            const DrawInfo &drawInfo,
            const MaterialPass &materialPass);

    //! Shadable object rendering for lighting and shadow pass.
    //! @warning Not supported for this mode.
    virtual void processLighting(
            Shadable &object,
            Shadowable &shadowable,
            const DrawInfo &drawInfo,
            const MaterialPass &materialPass);

    //! Shadable object rendering for deferred diffuse pass.
    //! @warning Not supported for this mode.
    virtual void processDeferred(
            Shadable &object,
            const DrawInfo &drawInfo,
            const MaterialPass &materialPass);

protected:

    Int32 u_modelViewProjectionMatrix;
    Int32 u_worldMatrix;      //!< only for deferred, world space vertex matrix
    Int32 u_normalMatrix;     //!< only for deferred, world space normal matrix

    Int32 u_bonesMatrixArray; //!< object space bones matrices

    Bool m_diffuseMap;
    Int32 u_diffuseMap;
    Int32 u_diffuse;

    UInt32 a_rigging;
    UInt32 a_skinning;
    UInt32 a_weighting;

    ShaderInstance m_shaderInstance;
};

} // namespace o3d

#endif // _O3D_COLORMATERIAL_H

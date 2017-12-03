/**
 * @file blending.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BLENDING_H
#define _O3D_BLENDING_H

#include "o3d/core/base.h"

namespace o3d {

/**
 * @brief Blending mode controller.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2016-02-06
 * This class help the usage of current and custom blending functions and equations.
 * It support a global and a per draw buffer settings.
 * About the blending function, it does only the necessary changes on OpenGL while
 * you uses the function profiles.
 * For the blending equations, it can optimizes changes for merged (non separates)
 * equations.
 * Also, applying a change for a specific draw buffer that is already defined globally
 * will does not perform OpenGL call.
 * In others word, using customs values for functions or seperated equations
 * will not be optimized.
 * @note If you uses frequently a non listed configuration into the profiles, thanks to
 * report it.
 */
class O3D_API Blending
{
public:

    //! Source and destination blending functions.
    //! @see https://www.opengl.org/wiki/GLAPI/glBlendFuncSeparate for more details.
    enum Func
    {
        ZERO                      = 0,       //!< Default for DST
        ONE                       = 1,       //!< Default for SRC
        SRC_COLOR                 = 0x0300,
        ONE_MINUS_SRC_COLOR       = 0x0301,
        SRC_ALPHA                 = 0x0302,
        ONE_MINUS_SRC_ALPHA       = 0x0303,
        DST_ALPHA                 = 0x0304,
        ONE_MINUS_DST_ALPHA       = 0x0305,
        DST_COLOR                 = 0x0306,
        ONE_MINUS_DST_COLOR       = 0x0307,
        SRC_ALPHA_SATURATE        = 0x0308,  //!< @note not compatible with DST
        CONSTANT_COLOR            = 0x8001,
        ONE_MINUS_CONSTANT_COLOR  = 0x8002,
        CONSTANT_ALPHA            = 0x8003,
        ONE_MINUS_CONSTANT_ALPHA  = 0x8004,
        BLEND_COLOR               = 0x8005,
        SRC1_COLOR                = 0x88F9,
        SRC1_ALPHA                = 0x8589,
        ONE_MINUS_SRC1_COLOR      = 0x88FA,
        ONE_MINUS_SRC1_ALPHA      = 0x88FB,
    };

    enum Equation
    {
        //! The source and destination colors are added to each other. O = sS + dD.
        //! The The s and d are blending parameters that are multiplied into each of S and D before the addition.
        //! @note This is the default equation.
        FUNC_ADD               = 0x8006,

        //! The output color is the component-wise minimum value of the source and dest colors.
        //! So performing GL_MIN in the RGB equation means that Or = min(Sr, Dr), Og = min(Sg, Dg), and so forth.
        //! The parameters s and d are ignored for this equation.
        MIN                    = 0x8007,

        //! The output color is the component-wise maximum value of the source and dest colors.
        //! The parameters s and d are ignored for this equation.
        MAX                    = 0x8008,

        //! Subtracts the destination from the source. O = sS - dD.
        //! The source and dest are multiplied by blending parameters.
        FUNC_SUBTRACT          = 0x800A,

        //! Subtracts the source from the destination. O = dD - sS.
        //! The source and dest are multiplied by blending parameters.
        FUNC_REVERSE_SUBTRACT  = 0x800B,
    };

    //! More comme blending configuration profiles.
    enum FuncProfile
    {
        //! No blending (disabled). This is the default profile.
        DISABLED = 0,
        //! Uses a custom set of blend functions.
        CUSTOM = 1,
        //! Unified source alpha / one minus source alpha
        SRC_A__ONE_MINUS_SRC_A = 2,
        //! Unified source alpha / one.
        SRC_A__ONE,
        //! Unified source alpha / zero
        SRC_A__ZERO,
        //! Unified One / one
        ONE__ONE,
        //! Unified destination color / source color
        DST_COL__SRC_COL,
        //! Separated Rgb(one / one minus source alpha), Alpha(one / zero)
        ONE__ONE_MINUS_SRC_A___ONE__ZERO,
        //! Separated Rgb(one / one minus source alpha), Alpha(source alpha / zero)
        ONE__ONE_MINUS_SRC_A___SRC_A__ZERO,
        //! Separated Rgb(one / one minus source alpha), Alpha(one / one minus source alpha)
        ONE__ONE_MINUS_SRC_A___ONE__ONE_MINUS_SRC_A,
    };

    Blending();

    ~Blending();

    //! Force to the initial blending functions for any draw buffers (disabled).
    //! Independing of the current value, the value is reset with forcing to default.
    void forceDefaultFunc();
    //! Force to the initial blending equations for any draw buffers (FUNC_ADD).
    //! Independing of the current value, the value is reset with forcing to default.
    void forceDefaultEquation();

    //! Set default blending functions for any draw buffers (disabled).
    void setDefaultFunc();
    //! Set default blending functions for a specific draw buffer index (disabled).
    //! @param drawBuffer Draw buffer index.
    void setDefaultFunc(UInt32 drawBuffer);

    //! Set default blending equations for any draw buffers (FUNC_ADD).
    void setDefaultEquation();
    //! Set default blending equations for a specific draw buffer index (FUNC_ADD).
    //! @param drawBuffer Draw buffer index.
    void setDefaultEquation(UInt32 drawBuffer);

    //! Get the global blending profile for any draw buffers.
    //! @warning Throws a warning if global functions are overrided by per buffer index
    //! configurations.
    FuncProfile getProfile() const;
    //! Get the blending profile for a specific draw buffer index.
    //! @param drawBuffer Draw buffer index.
    FuncProfile getProfile(UInt32 drawBuffer) const;

    //! Is blending globaly enabled.
    //! @warning Throws a warning if global functions are overrided by per buffer index
    //! configurations.
    Bool getState() const;
    //! Is blending enabled for a particular draw buffer index.
    //! @param drawBuffer Draw buffer index.
    Bool getState(UInt32 drawBuffer) const;


    //! Defines the blending configuration for any draw buffers, using a profile.
    //! The advantages to uses a profile ares :
    //! - quickly set with a simple enum
    //! - optimized OpenGL calls to avoid reapplying states
    //! @note This will overrides any per draw buffer blending functions.
    void setFunc(FuncProfile profile);
    //! Defines the blending configuration for a particular draw buffer, using a profile.
    void setFunc(UInt32 drawBuffer, FuncProfile profile);

    //! Defines the blending configuration for any draw buffers, unified for Rbg and Alpha.
    //! @note This will overrides any per draw buffer blending functions.
    void setFunc(Func src, Func dst);
    //! Defines the blending configuration for any draw buffers, separated for Rbg and Alpha.
    void setFunc(Func srcRgb, Func dstRgb, Func srcAlpha, Func dstAlpha);

    //! Defines the blending configuration for a particular draw buffers, unified for Rbg and Alpha.
    void setFunc(UInt32 drawBuffer, Func src, Func dst);
    //! Defines the blending configuration for a particular draw buffers, separated for Rbg and Alpha.
    void setFunc(UInt32 drawBuffer, Func srcRgb, Func dstRgb, Func srcAlpha, Func dstAlpha);

    //! Set the blend equation for any draw buffers.
    void setEquation(Equation eq);
    //! Set the separated blend equation for any draw buffers.
    void setEquation(Equation rgb, Equation a);

    //! Set the blend equation for a particalar draw buffer.
    void setEquation(UInt32 drawBuffer, Equation eq);
    //! Set the separate blend equation for a particalar draw buffer.
    void setEquation(UInt32 drawBuffer, Equation rgb, Equation a);

private:

    static const UInt32 MAX_DRAW_BUFFERS = 8;

    struct BufferIndex
    {
        Bool globalFunc;        //! global apply on this local function
        Bool globalEquation;    //! global apply on this local equation
        FuncProfile functions;  //! current blend func + eq (from profile or custom)
        Equation equations;     //! merged equations

        BufferIndex();
    };

    BufferIndex m_global;
    BufferIndex m_locals[MAX_DRAW_BUFFERS];
};


} // namespace o3d

#endif // _O3D_BLENDING_H

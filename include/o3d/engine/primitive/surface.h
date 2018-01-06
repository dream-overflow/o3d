/**
 * @file surface.h
 * @brief Surface primitive
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_SURFACE_H
#define _O3D_SURFACE_H

#include "primitive.h"

namespace o3d {

/**
 * @brief Definition of a plane surface primitive
 * @details Many options :
 *  - Filled or wired mode
 *  - One or double sided generation of indexes for filled mode only
 *  - Simple grid (only horizontals and verticals lines).
 *  - Texture UV coordinates generation (not available for simple grid mode).
 *  - Alternate the generation of the triangle for filled and wired modes only.
 */
class O3D_API Surface : public Primitive
{
    O3D_DECLARE_ABSTRACT_CLASS(Surface)

public:

    enum SurfaceFlags
    {
        GRID = 32  //!< Only simple lines per division and no triangulation
    };

    enum SurfaceMode
    {
        ONE_SIDED = FILLED_MODE,
        DOUBLE_SIDED = FILLED_MODE | BACK_SIDE,
        SIMPLE_GRID = WIRED_MODE | GRID,
        META_WIRED = WIRED_MODE | META
    };

    //! Constructor. Build a plane surface primitive on X+Z+ (normal at Y+).
    //! @param width Width of the plane
    //! @param height Height of the plane
    //! @param widthDiv Number of divisions on width (X)
    //! @param heightDiv Number of divisions on height (Z)
    //! @param isWired Wired if true, filled if false
    //! @param asGrid (Only if isWired) Make a grid and not a real wireframe model
    Surface(
        Float width,
        Float height,
        UInt32 widthDiv,
        UInt32 heightDiv,
        UInt32 flags = ONE_SIDED);

    //! Get the width
    inline Float getWidth() const { return m_width; }

    //! Get the height
    inline Float getHeight() const { return m_height; }

    //! Get the number of divisions on width (X)
    inline UInt32 getNumWidthDiv() const { return m_widthDiv; }

    //! Get the number of divisions on height (Z)
    inline UInt32 getNumHeightDiv() const { return m_heightDiv; }

    //! Is simple grid (useless in filled surface)
    inline Bool isSimpleGrid() const { return (m_capacities & GRID) == GRID; }

protected:

    void constructFilled(UInt32 widthDiv, UInt32 heightDiv, Bool doubleSided);
    void constructWired(UInt32 widthDiv, UInt32 heightDiv);

    //! Restricted copy constructor
    Surface(Surface &dup) : Primitive(dup) {}

    Float m_width;
    Float m_height;

    UInt32 m_widthDiv;
    UInt32 m_heightDiv;
};

} // namespace o3d

#endif // _O3D_SURFACE_H

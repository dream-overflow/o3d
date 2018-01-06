/**
 * @file cylinder.h
 * @brief Cylinder primitive
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Julien IBARZ
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_CYLINDER_H
#define _O3D_CYLINDER_H

#include "primitive.h"

namespace o3d {

/**
 * @brief Definition of a cylinder primitive
 */
class O3D_API Cylinder : public Primitive
{
    O3D_DECLARE_ABSTRACT_CLASS(Cylinder)

public:

    //! Default constructor. Build an Y+ up aligned cylinder.
    //! @param base1 the radius of the beginning base
    //! @param base2 the radius of the ending base (can be equal to zero and in this case a cone is build)
    //! @param height the height of the cylinder (distance between the two bases)
    //! @param slices the number of division on the bases
    //! @param stacks the number of division between the bases
    //! @note In wired mode the bases are not filled with lines.
    Cylinder(
        Float base1,
        Float base2,
        Float height,
        UInt32 slices,
        UInt32 stacks,
        UInt32 flags = 0);

    //! Get the radius of the beginning base
    inline Float getBase1Radius() const { return m_base1; }

    //! Get the radius of the first base
    inline Float getBase2Radius() const { return m_base2; }

    //! Get base2 the radius of the ending base (can be equal to zero and in this case a cone is build)
    inline Float getHeight() const { return m_height; }

    //! Get the number of division on the bases
    inline UInt32 getNumSlices() const { return m_slices; }

    //! Get the number of division between the bases
    inline UInt32 getNumStacks() const { return m_stacks; }

protected:

    void constructFilled(
        Float base1,
        Float base2,
        Float height,
        UInt32 slices,
        UInt32 stacks);

    void constructWired(
        Float base1,
        Float base2,
        Float height,
        UInt32 slices,
        UInt32 stacks);

    //! Restricted copy constructor
    Cylinder(Cylinder &dup) : Primitive(dup) {}

    Float m_base1;
    Float m_base2;
    Float m_height;
    UInt32 m_slices;
    UInt32 m_stacks;
};

} // namespace o3d

#endif // _O3D_CYLINDER_H

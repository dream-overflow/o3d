/**
 * @file sphere.h
 * @brief Sphere primitive
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Julien IBARZ
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_SPHERE_H
#define _O3D_SPHERE_H

#include "primitive.h"

namespace o3d {

/**
 * @brief Definition of a sphere primitive
 */
class O3D_API Sphere : public Primitive
{
    O3D_DECLARE_ABSTRACT_CLASS(Sphere)

public:

    enum TexCoordCorrectionPolicy
    {
        NO_CORRECTION = 32,
        FAST_CORRECTION = 64
    };

    //! Default constructor. Build a sphere Y axis aligned.
    //! @param radius the radius of the sphere
    //! @param stacks the number of division on latitude
    //! @param slices the number of division on longitude
    Sphere(Float radius, UInt32 slices, UInt32 stacks, UInt32 flags = 0);

    //! Get the radius
    inline Float getRadius() const { return m_radius; }

    //! Get the number of division on latitude
    inline UInt32 getNumSlices() const { return m_slices; }

    //! Get the number of division on longitude
    inline UInt32 getNumStacks() const { return m_slices; }

    //! Returns the texture generation correction flag
    inline TexCoordCorrectionPolicy getTexCoordCorrectionPolicy() const;

protected:

    void constructVertices(Float radius, UInt32 slices, UInt32 stacks);
    void constructFilled(Float radius, UInt32 slices, UInt32 stacks);
    void constructWired(Float radius, UInt32 slices, UInt32 stacks);

    //! Restricted copy constructor
    Sphere(Sphere &dup) : Primitive(dup) {}

    Float m_radius;
    UInt32 m_slices;
    UInt32 m_stacks;
};

} // namespace o3d

#endif // _O3D_SPHERE_H

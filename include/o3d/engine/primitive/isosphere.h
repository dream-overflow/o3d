/**
 * @file isosphere.h
 * @brief Definition of an iso-sphere primitive.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-11-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ISOSPHERE_H
#define _O3D_ISOSPHERE_H

#include "primitive.h"

namespace o3d {

/**
 * @brief Iso-sphere geometry generator.
 */
class O3D_API IsoSphere : public Primitive
{
	O3D_DECLARE_ABSTRACT_CLASS(IsoSphere)

public:

	enum IsoSphereMode
	{
		HALF_SPHERE = 4,
	};

	//! Default constructor. Build an iso-sphere.
	//! @param radius Radius of the iso-sphere
	//! @param subDiv Specify how many times you want to subdivide the icosahedron
	//! @param halfSphere True if you want only a half sphere
    IsoSphere(Float radius, UInt32 subDiv, UInt32 flags = 0);

	//! Is half sphere
    Bool isHalfSphere() const { return (m_capacities & HALF_SPHERE) == HALF_SPHERE; }

protected:

	//! Build a isosphere from a icosahedron subdivision
	void buildIsoSphere();

    /**
     * @brief IN. the radius of the sphere
     */
    Float m_radius;

    /**
     * @brief IN. specify how many times you want to subdivide the icosahedron.
     * The vertex number is given by:  V(subDiv) = V(n) = 2+10*2^(2*n) for a whole sphere
     * V(n) = 1+5.2^(n-1)*(2^(n+1)+1) for a half sphere
     */
    UInt32 m_subDiv;

    /**
     * @brief IN. true if you want only a half sphere
     */
    Bool m_halfSphere;

    /**
     * @brief Not supported yet
     */
    Bool m_optimize;
};

} // namespace o3d

#endif // _O3D_ISOSPHERE_H

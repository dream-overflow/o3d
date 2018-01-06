/**
 * @file primitive.h
 * @brief Geometry primitive base class.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Julien IBARZ
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PRIMITIVES_H
#define _O3D_PRIMITIVES_H

#include "o3d/image/color.h"
#include "o3d/core/baseobject.h"
#include "../enginetype.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Abstract definition of a 3d primitive
 * @details Most of the primitives can be created with filled mode (triangles primitives)
 * or in wired mode (lines primitives).
 * Texture UV mapping can be generated in filled mode.
 * Some primitives have specifics options.
 */
class O3D_API Primitive : NonAssignable<>, public BaseObject
{
	O3D_DECLARE_ABSTRACT_CLASS(Primitive)

public:

	enum PrimitiveFlags
	{
        FILLED_MODE = 0,     //!< Triangle mode.
        WIRED_MODE = 1,      //!< Lines with triangulations.
        GEN_TEX_COORDS = 2,  //!< UV mapping generation as possible.
        BACK_SIDE = 4,       //!< Generate back side for filled mode (when implemented).
        META = 8,            //!< If meta, no triangulation are done in wired (depend of implementation).
        ALTERNATE_TRIANGLE = 16  //! Alternate the generation of the triangulation (when triangulation on wired)
    };

	//! Default constructor
	Primitive(UInt32 flags = 0);

	//! Internal constructor
    Primitive(UInt32 verticesCount, UInt32 indicesCount, UInt32 flags = 0);

	//! Destructor
	virtual ~Primitive();

	//! Return the vertices array
	//! @note The size of the array is equal to GetNbrVertices() * 3
	inline const Float* getVertices() const { return m_pVertices; }

	//! Return the number of vertices
	inline UInt32 getNumVertices() const { return m_verticesCount; }

	//! Return the faces indices array
	//! @note The size of the array is equal to GetNbrFaces() * 3
	//! if i'ts filled and getNumVertices() * 2 if it's wired.
	inline const UInt32* getFacesIndices() const { return m_pIndices; }

	//! Return the number of faces indices
	inline UInt32 getNumIndices() const { return m_indicesCount; }

	//! Return the number of faces
	inline UInt32 getNumFaces() const
	{
		return isWired() ? m_indicesCount/2 : m_indicesCount/3;
	}

	//! Return the face format (lines or triangles)
	inline PrimitiveFormat getFaceType() const
	{
		return isWired() ? P_LINES : P_TRIANGLES;
	}

	//! Get a pointer to the texture coordinates buffer
    //! @return A pointer to the first texture coordinate or null if none
	const Float* getTexCoords() const { return m_pTexCoords; }

	//! Is texture coordinates generation set
    inline Bool isTexCoords() const { return (m_capacities & GEN_TEX_COORDS) == GEN_TEX_COORDS; }

	//! Is wired mode
    inline Bool isWired() const { return (m_capacities & WIRED_MODE) == WIRED_MODE; }

	//! Is filled mode
    inline Bool isFilled() const { return (m_capacities & WIRED_MODE) == 0; }

    //! Is one sided plane (useless in wired surface)
    inline Bool isOneSided() const { return (m_capacities & BACK_SIDE) == 0; }

    //! Is double sided plane (useless in wired surface)
    inline Bool isDoubleSided() const { return (m_capacities & BACK_SIDE) == BACK_SIDE; }

    //! Is meta (useless in filled surface)
    inline Bool isMeta() const { return (m_capacities & META) == META; }

    //! Is alternated triangulation.
    inline Bool isAlternateTriangle() const { return (m_capacities & ALTERNATE_TRIANGLE) == ALTERNATE_TRIANGLE; }

protected:

	//! Restricted copy constructor
	Primitive(const Primitive &dup);

    UInt32 m_capacities;

	UInt32 m_verticesCount;
	Float *m_pVertices;

	UInt32 m_indicesCount;
	UInt32 *m_pIndices;

	Float *m_pTexCoords;
};

} // namespace o3d

#endif // _O3D_PRIMITIVES_H

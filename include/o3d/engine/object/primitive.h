/**
 * @file primitive.h
 * @brief Draw of some 3d instances.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author  Julien IBARZ
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

/**
 * @brief Definition of a cube primitive
 */
class O3D_API Cube : public Primitive
{
	O3D_DECLARE_ABSTRACT_CLASS(Cube)

public:

    enum CubeFlags
    {
        GRID = 32  //!< Only simple lines per division and no triangulation
    };

    enum CubeMode
    {
        ONE_SIDED = FILLED_MODE,
        GRID_CUBE = WIRED_MODE | GRID,
        META_CUBE = WIRED_MODE | META,
    };

	//! Default constructor
	//! @param the size of the edges of the cube
    //! @param division the number of division between the vertices of the cube
    //! @note In wired mode the cells are not triangulated (@todo could be done later)
    Cube(Float size, UInt32 division, UInt32 flags = 0);

	//! Get the number of divisions
	inline UInt32 getNumDivisions() const { return m_division ; }

	//! Get the size
	inline Float getSize() const { return m_size; }

    //! Is simple grid (useless in filled cube)
    inline Bool isSimpleGrid() const { return (m_capacities & GRID) == GRID; }

private:

	//! The edges
	static const UInt32 EDGES[12][2];
	//! Faces
	static const UInt32 FACES[6][4];
	//! True means as the same direction than the definition edges
	static const Bool FACE_EDGE_DIR[6][4];

	inline UInt32 getBeginVertice(const UInt32 edge, const Bool sens)
	{
        if (sens) {
			return EDGES[edge][0];
        } else {
			return EDGES[edge][1];
        }
	}

	inline UInt32 getEndVertice(const UInt32 edge, const Bool sens)
	{
        if (sens) {
			return EDGES[edge][1];
        } else {
			return EDGES[edge][0];
        }
	}

	inline UInt32 getVertice(const UInt32 edge, const UInt32 vertice, const Bool sens)
	{
        if(sens) {
			return vertice + m_pEdgeOffSet[edge];
        } else {
			return m_pEdgeOffSet[edge] + (m_division - 1) - vertice;
        }
	}

	inline UInt32 getVertice(const UInt32 face, const UInt32 i, const UInt32 j)
	{
        if (i == 0) {
            if (j == 0) {
				return getBeginVertice(FACES[face][3], FACE_EDGE_DIR[face][3]);
            }

            if (j == m_division + 1) {
				return getEndVertice(FACES[face][3], FACE_EDGE_DIR[face][3]);
            }

			return getVertice(FACES[face][3], j - 1, FACE_EDGE_DIR[face][3]);
		}

        if (i == m_division + 1) {
            if (j == 0) {
				return getBeginVertice(FACES[face][1], FACE_EDGE_DIR[face][1]);
            }

            if (j == m_division + 1) {
				return getEndVertice(FACES[face][1], FACE_EDGE_DIR[face][1]);
            }

			return getVertice(FACES[face][1], j - 1, FACE_EDGE_DIR[face][1]);
		}

        if (j == 0) {
            if (i == 0) {
				return getBeginVertice(FACES[face][0], FACE_EDGE_DIR[face][0]);
            }

            if (i == m_division + 1) {
				return getEndVertice(FACES[face][0], FACE_EDGE_DIR[face][0]);
            }

			return getVertice(FACES[face][0], i - 1, FACE_EDGE_DIR[face][0]);
		}

        if (j == m_division + 1) {
            if (i == 0) {
				return getBeginVertice(FACES[face][2], FACE_EDGE_DIR[face][2]);
            }

            if (i == m_division + 1) {
				return getEndVertice(FACES[face][2], FACE_EDGE_DIR[face][2]);
            }

			return getVertice(FACES[face][2], i - 1, FACE_EDGE_DIR[face][2]);
		}

		return m_pFaceOffSet[face] + (i-1) * m_division + (j - 1);
	}

	//! Do not use
    Cube() :
        m_size(0.f),
        m_division(0)
    {
    }

protected:

	void constructFilled(const Float size, UInt32 offSet);
	void constructWired(const Float size);

	//! Restricted copy constructor
	Cube(Cube &dup) : Primitive(dup), m_size(0.f), m_division(0) {}

	// Keep the offSet index of the first vertex of the edge (this values are deleted after the construction)
	UInt32 *m_pEdgeOffSet;
	UInt32 *m_pFaceOffSet;
	
	const Float m_size;
	const UInt32 m_division;
};

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

#endif // _O3D_PRIMITIVES_H

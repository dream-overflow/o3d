/**
 * @file cube.h
 * @brief Cube primitive
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Julien IBARZ
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2018 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_CUBE_H
#define _O3D_CUBE_H

#include "primitive.h"

namespace o3d {

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
    //! @param flags
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

    void buildVertices(UInt32 offSet);
    void constructFilled(const Float size, UInt32 offSet);
    void constructWired(const Float size, UInt32 offSet);

    //! Restricted copy constructor
    Cube(Cube &dup) : Primitive(dup), m_size(0.f), m_division(0) {}

    // Keep the offSet index of the first vertex of the edge (this values are deleted after the construction)
    UInt32 *m_pEdgeOffSet;
    UInt32 *m_pFaceOffSet;

    const Float m_size;
    const UInt32 m_division;
};

} // namespace o3d

#endif // _O3D_CUBE_H

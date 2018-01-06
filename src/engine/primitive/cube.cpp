/**
 * @file cube.cpp
 * @brief Implementation of cube.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Julien IBARZ
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/primitive/cube.h"

#include "o3d/core/vector3.h"
#include "o3d/core/debug.h"

#include <math.h>

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Cube, ENGINE_CUBE, Primitive)

// The edges
const UInt32 Cube::EDGES[12][2] = {
    { 0, 1 },
    { 1, 2 },
    { 2, 3 },
    { 3, 0 },
    { 0, 4 },
    { 1, 5 },
    { 2, 6 },
    { 3, 7 },
    { 4, 5 },
    { 5, 6 },
    { 6, 7 },
    { 7, 4 }
};

//Faces
const UInt32 Cube::FACES[6][4] = {
    { 0, 5, 8 , 4  },
    { 1, 6, 9 , 5  },
    { 2, 7, 10, 6  },
    { 3, 4, 11, 7  },
    { 0, 3, 2 , 1  },
    { 8, 9, 10, 11 }
};

// true means as the same direction than the definition edges
const Bool Cube::FACE_EDGE_DIR[6][4] = {
    { True , True , True, True },
    { True , True , True, True },
    { True , True , True, True },
    { True , True , True, True },
    { False, False, True, True },
    { True , True , False , False  }
};


Cube::Cube(
    Float size,
    UInt32 division,
    UInt32 flags) :
        Primitive(flags),
        m_size(size),
        m_division(division)
{
    O3D_ASSERT(size > 0.);

    m_pEdgeOffSet = new UInt32[12];
    m_pFaceOffSet = new UInt32[6];

    if (isSimpleGrid()) {
        // simple grid cube
        m_verticesCount = (8 + 12 * division);
        m_indicesCount = (12 + division * 2 * 6) * 2;
    } else if (isMeta()) {
        // wired without triangulation
        m_verticesCount = (8 + 12 * division + division * division * 6);
        m_indicesCount = (2 * (division+1) * (division+1) + division+1) * 2 * 6;
    } else if (isWired()) {
        // wired with triangulation
        m_verticesCount = (8 + 12 * division + division * division * 6);
        m_indicesCount = (3 * (division+1) * (division+1) + division+1) * 2 * 6;
    } else {
        // filled with triangles
        m_verticesCount = (8 + 12 * division + division * division * 6);
        m_indicesCount = (division + 1) * (division + 1) * 6 * 2 * 3;
    }

    m_pVertices = new Float[m_verticesCount*3];
    m_pIndices = new UInt32[m_indicesCount];

    Float halfSize = size * 0.5f;

    // Les sommets du cube
    Vector3 sommets[8];
    sommets[0] = Vector3(-halfSize , -halfSize , -halfSize );
    sommets[1] = Vector3(halfSize, -halfSize , -halfSize );
    sommets[2] = Vector3(halfSize, halfSize, -halfSize );
    sommets[3] = Vector3(-halfSize , halfSize, -halfSize );
    sommets[4] = Vector3(-halfSize , -halfSize , halfSize);
    sommets[5] = Vector3(halfSize, -halfSize , halfSize);
    sommets[6] = Vector3(halfSize, halfSize, halfSize);
    sommets[7] = Vector3(-halfSize , halfSize, halfSize);

    // Copy sommets to the vertices buffer
    for(UInt32 i = 0 ; i < 8 ; ++i) {
        sommets[i].copyTo(&m_pVertices[i * 3]);
    }

    // Generate the edges
    UInt32 offSet = 8 * 3;

    if (division != 0) {
        for(UInt32 edge = 0 ; edge < 12 ; ++edge) {
            m_pEdgeOffSet[edge] = offSet / 3;

            Vector3 step;
            for(int i = 0 ; i < 3 ; ++i) {
                step[i] = (sommets[ EDGES[edge][1] ][i] - sommets[ EDGES[edge][0] ][i]) / Float(m_division + 1);
            }

            Vector3 position(sommets[ EDGES[edge][0] ]);

            position += step;
            for(UInt32 i = 0 ; i < division ; ++i) {
                position.copyTo(&m_pVertices[offSet]);
                position += step;
                offSet += 3;
            }
        }
    }

    if (isWired()) {
        constructWired(size, offSet);
    } else {
        constructFilled(size, offSet);
    }

    deleteArray(m_pEdgeOffSet);
    deleteArray(m_pFaceOffSet);
}

void Cube::buildVertices(UInt32 offSet)
{
    // Construct the vertices per face
    if (m_division != 0) {
        for (UInt32 face = 0 ; face < 6 ; ++face) {
            m_pFaceOffSet[face] = offSet / 3;
            const Vector3 begin(&(m_pVertices[ getBeginVertice(FACES[face][0], FACE_EDGE_DIR[face][0])*3 ]));

            Vector3 iStep;
            for (UInt32 c = 0 ; c < 3 ; ++c) {
                iStep[c] = (
                        m_pVertices[getEndVertice(FACES[face][0],FACE_EDGE_DIR[face][0])*3 + c] -
                        m_pVertices[ getBeginVertice(FACES[face][0], FACE_EDGE_DIR[face][0])*3 + c]) /
                    Float(m_division + 1);
            }

            Vector3 jStep;
            for (UInt32 c = 0 ; c < 3 ; ++c) {
                jStep[c] = (
                        m_pVertices[getEndVertice(FACES[face][3],FACE_EDGE_DIR[face][3])*3 + c] -
                        m_pVertices[ getBeginVertice(FACES[face][3],FACE_EDGE_DIR[face][3])*3 + c]) /
                    Float(m_division + 1);
            }

            for (UInt32 i = 0 ; i < m_division ; ++i) {
                for (UInt32 j = 0 ; j < m_division ; ++j) {
                    const Vector3 pos(begin + iStep * Float(i + 1) + jStep * Float(j + 1));

                    pos.copyTo(&(m_pVertices[offSet]));
                    offSet += 3;
                }
            }
        }
    }
}

void Cube::constructFilled(Float /*size*/, UInt32 offSet)
{
    buildVertices(offSet);

    // Construct the faces
    UInt32 *indices = m_pIndices;

    if (isAlternateTriangle()) {
        for(UInt32 face = 0 ; face < 6 ; ++face) {
            for(UInt32 y = 0 ; y < m_division + 1 ; ++y) {
                for(UInt32 x = 0 ; x < m_division + 1 ; ++x) {
                    if ((x+y) % 2 == 0) {
                        // First triangle
                        *indices++ = getVertice(face,x,y);
                        *indices++ = getVertice(face,x+1,y);
                        *indices++ = getVertice(face,x,y+1);

                        // Second triangle
                        *indices++ = getVertice(face,x+1,y);
                        *indices++ = getVertice(face,x+1,y+1);
                        *indices++ = getVertice(face,x,y+1);
                    } else {
                        // First triangle
                        *indices++ = getVertice(face,x,y);
                        *indices++ = getVertice(face,x+1,y+1);
                        *indices++ = getVertice(face,x,y+1);

                        // Second triangle
                        *indices++ = getVertice(face,x,y);
                        *indices++ = getVertice(face,x+1,y);
                        *indices++ = getVertice(face,x+1,y+1);
                    }
                }
            }
        }
    } else {
        for(UInt32 face = 0 ; face < 6 ; ++face) {
            for(UInt32 y = 0 ; y < m_division + 1 ; ++y) {
                for(UInt32 x = 0 ; x < m_division + 1 ; ++x) {
                    // First triangle
                    *indices++ = getVertice(face,x,y);
                    *indices++ = getVertice(face,x+1,y);
                    *indices++ = getVertice(face,x,y+1);

                    // Second triangle
                    *indices++ = getVertice(face,x+1,y);
                    *indices++ = getVertice(face,x+1,y+1);
                    *indices++ = getVertice(face,x,y+1);
                }
            }
        }
    }
}

void Cube::constructWired(Float /*size*/, UInt32 offSet)
{
    // Generate lines
    UInt32 *indices = m_pIndices;
    UInt32 p1,p2,p3,p4;

    if (isSimpleGrid()) {
        // Generate edges lines
        for(UInt32 i = 0 ; i < 12 ; ++i) {
            *indices++ = EDGES[i][0];
            *indices++ =  EDGES[i][1];
        }

        for (UInt32 face = 0 ; face < 6 ; ++face) {
            // Generate intermediary lines
            for (UInt32 i = 0 ; i < m_division ; ++i) {
                // First line
                *indices++ = this->getVertice(FACES[face][0], i, FACE_EDGE_DIR[face][0]);
                *indices++ = this->getVertice(FACES[face][2], i, FACE_EDGE_DIR[face][2]);

                // Second line
                *indices++ = this->getVertice(FACES[face][1], i, FACE_EDGE_DIR[face][1]);
                *indices++ = this->getVertice(FACES[face][3], i, FACE_EDGE_DIR[face][3]);
            }
        }
    } else if (isMeta()) {
        buildVertices(offSet);

        // 1...2
        // .   .
        // 3...x

        for(UInt32 face = 0 ; face < 6 ; ++face) {
            for(UInt32 y = 0 ; y < m_division + 1 ; ++y) {
                for(UInt32 x = 0 ; x < m_division + 1 ; ++x) {
                    p1 = getVertice(face, x, y);
                    p2 = getVertice(face, x+1, y);
                    p3 = getVertice(face, x, y+1);

                    // hor. line
                    *indices++ = p1;
                    *indices++ = p2;

                    // vert. line 2
                    *indices++ = p1;
                    *indices++ = p3;
                }
            }

            // last hor. lines
            for (UInt32 x = 0 ; x < m_division+1 ; ++x) {
                p3 = getVertice(face, x, m_division+1);
                p4 = getVertice(face, x+1, m_division+1);

                *indices++ = p3;
                *indices++ = p4;
            }
        }
    } else {
        buildVertices(offSet);

        if (isAlternateTriangle()) {
            // Generate 6 surfaces but avoid redefinitions of edges
            for(UInt32 face = 0 ; face < 6 ; ++face) {
                for(UInt32 y = 0 ; y < m_division + 1 ; ++y) {
                    for(UInt32 x = 0 ; x < m_division + 1 ; ++x) {
                        p1 = getVertice(face, x, y);
                        p2 = getVertice(face, x+1, y);
                        p3 = getVertice(face, x, y+1);
                        p4 = getVertice(face, x+1, y+1);

                        // hor. line
                        *indices++ = p1;
                        *indices++ = p2;

                        // vert. line 2
                        *indices++ = p1;
                        *indices++ = p3;

                        // triangulation line
                        if (((x+y) % 2) == 0) {
                            *indices++ = p1;
                            *indices++ = p4;
                        } else {
                            *indices++ = p2;
                            *indices++ = p3;
                        }
                    }
                }

                // last hor. lines
                for (UInt32 x = 0 ; x < m_division+1 ; ++x) {
                    p3 = getVertice(face, x, m_division+1);
                    p4 = getVertice(face, x+1, m_division+1);

                    *indices++ = p3;
                    *indices++ = p4;
                }
            }
        } else {
            // Generate 6 surfaces but avoid redefinitions of edges
            for(UInt32 face = 0 ; face < 6 ; ++face) {
                for(UInt32 y = 0 ; y < m_division + 1 ; ++y) {
                    for(UInt32 x = 0 ; x < m_division + 1 ; ++x) {
                        p1 = getVertice(face, x, y);
                        p2 = getVertice(face, x+1, y);
                        p3 = getVertice(face, x, y+1);
                        p4 = getVertice(face, x+1, y+1);

                        // hor. line
                        *indices++ = p1;
                        *indices++ = p2;

                        // vert. line 2
                        *indices++ = p1;
                        *indices++ = p3;

                        // triangulation line
                        *indices++ = p1;
                        *indices++ = p4;
                    }
                }

                // last hor. lines
                for (UInt32 x = 0 ; x < m_division+1 ; ++x) {
                    p3 = getVertice(face, x, m_division+1);
                    p4 = getVertice(face, x+1, m_division+1);

                    *indices++ = p3;
                    *indices++ = p4;
                }
            }
        }
    }
}

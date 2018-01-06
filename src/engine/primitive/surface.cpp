/**
 * @file surface.cpp
 * @brief Implementation of surface.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/primitive/surface.h"

#include "o3d/core/vector3.h"
#include "o3d/core/debug.h"

#include <math.h>

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Surface, ENGINE_SURFACE, Primitive)

Surface::Surface(
    Float width,
    Float height,
    UInt32 widthDiv,
    UInt32 heightDiv,
    UInt32 flags) :
        Primitive(flags),
            m_width(width),
            m_height(height),
            m_widthDiv(widthDiv),
            m_heightDiv(heightDiv)
{
    O3D_ASSERT(width > 0.f);
    O3D_ASSERT(height > 0.f);

    if (isSimpleGrid()) {
        // vertices
        m_verticesCount = ((widthDiv+2) * (heightDiv+2)) - (widthDiv * heightDiv);
        m_pVertices = new Float[m_verticesCount*3];

        // texcoords
        if (isTexCoords()) {
            O3D_ERROR(E_InvalidParameter("Simple grid surface cannot have texture coordinates"));
        }

        // indices
        m_indicesCount = ((widthDiv+2) + (heightDiv+2)) * 2;
        m_pIndices = new UInt32[m_indicesCount];

        // step
        Float xDelta = width / (Float)(widthDiv+1);
        Float yDelta = height / (Float)(heightDiv+1);

        UInt32 offSet = 0;
        UInt32 verticeIndex;

        // start at half height
        Float yScale = height * 0.5f;

        // horizontal lines
        for (UInt32 y = 0; y < heightDiv+2; ++y) {
            verticeIndex = offSet*3;

            m_pVertices[verticeIndex  ] = -width * 0.5f;
            m_pVertices[verticeIndex+1] = 0.f;
            m_pVertices[verticeIndex+2] = yScale;

            m_pIndices[offSet] = offSet;
            ++offSet;

            verticeIndex = offSet*3;

            m_pVertices[verticeIndex  ] = width * 0.5f;
            m_pVertices[verticeIndex+1] = 0.f;
            m_pVertices[verticeIndex+2] = yScale;

            m_pIndices[offSet] = offSet;
            ++offSet;

            yScale -= yDelta;
        }

        // left and right vert lines
        m_pIndices[offSet] = 0;
        ++offSet;
        m_pIndices[offSet] = (heightDiv+1)*2;
        ++offSet;

        m_pIndices[offSet] = 1;
        ++offSet;
        m_pIndices[offSet] = (heightDiv+1)*2+1;
        ++offSet;

        // start at -half width + 1*xDelta
        Float xScale = -width * 0.5f + xDelta;

        // vertical lines
        for (UInt32 x = 1; x < widthDiv+1; ++x) {
            verticeIndex = (offSet-4)*3;

            m_pVertices[verticeIndex  ] = xScale;
            m_pVertices[verticeIndex+1] = 0.f;
            m_pVertices[verticeIndex+2] = -height * 0.5f;

            m_pIndices[offSet] = offSet-4;
            ++offSet;

            verticeIndex = (offSet-4)*3;

            m_pVertices[verticeIndex  ] = xScale;
            m_pVertices[verticeIndex+1] = 0.f;
            m_pVertices[verticeIndex+2] = height * 0.5f;

            m_pIndices[offSet] = offSet-4;
            ++offSet;

            xScale += xDelta;
        }
    } else {
        if (isWired() && isDoubleSided()) {
            O3D_ERROR(E_InvalidParameter("Wired mode is incompatible with double sided mode"));
        }

        // vertices
        m_verticesCount = ((widthDiv+2) * (heightDiv+2)) * (isDoubleSided() ? 2 : 1);

        m_pVertices = new Float[m_verticesCount*3];

        UInt32 n = 0;
        Float x,y;

        // step
        Float deltaX = width / (Float)(widthDiv+1);
        Float deltaY = height / (Float)(heightDiv+1);

        // start at half height
        y = height * 0.5f;

        if (isTexCoords()) {
            // generating vertices with textures coordinates

            m_pTexCoords = new Float[m_verticesCount*2];

            UInt32 o = 0;
            Float u,v;

            // step
            Float deltaU = 1.0f / (Float)(widthDiv+1);
            Float deltaV = 1.0f / (Float)(heightDiv+1);

            // start at 1.0
            v = 1.0f;

            for (UInt32 i = 0 ; i < heightDiv+2 ; ++i) {
                x = -width * 0.5f;
                u = 0.0f;

                for (UInt32 j = 0 ; j < widthDiv+2 ; ++j) {
                    m_pVertices[n] = x;
                    m_pVertices[n+1] = 0.f; // y+ oriented
                    m_pVertices[n+2] = y;

                    m_pTexCoords[o] = u;
                    m_pTexCoords[o+1] = v;

                    n += 3;
                    o += 2;

                    x += deltaX;
                    u += deltaU;
                }

                y -= deltaY;
                v -= deltaV;
            }

            // duplicate texcoords for the other side
            if (isDoubleSided()) {
                memcpy(&m_pTexCoords[o],m_pTexCoords,o*sizeof(Float));
            }
        } else {
            // generating vertices with textures coordinates
            for (UInt32 i = 0 ; i < heightDiv+2 ; ++i) {
                x = -width * 0.5f;

                for (UInt32 j = 0 ; j < widthDiv+2 ; ++j) {
                    m_pVertices[n] = x;
                    m_pVertices[n+1] = 0.f; // y+ oriented
                    m_pVertices[n+2] = y;

                    n += 3;
                    x += deltaX;
                }

                y -= deltaY;
            }
        }

        // duplicate vertices for the other side
        if (isDoubleSided()) {
            memcpy(&m_pVertices[n],m_pVertices,n*sizeof(Float));
        }

        // faces
        if (isMeta()) {
            // wired without triangulation
            m_indicesCount = (((2 * (widthDiv+1)) + 1) * (heightDiv+1) + widthDiv+1) * 2;
            m_pIndices = new UInt32[m_indicesCount];
            constructWired(widthDiv, heightDiv);
        } else if (isWired()) {
            // wired with triangulation
            m_indicesCount = (((3 * (widthDiv+1)) + 1) * (heightDiv+1) + widthDiv+1) * 2;
            m_pIndices = new UInt32[m_indicesCount];
            constructWired(widthDiv, heightDiv);
        } else {
            // filled triangles
            m_indicesCount = (2 * (widthDiv+1) * (heightDiv+1)) * 3 * (isDoubleSided() ? 2 : 1);
            m_pIndices = new UInt32[m_indicesCount];
            constructFilled(widthDiv, heightDiv, isDoubleSided());
        }
    }
}

void Surface::constructFilled(UInt32 widthDiv, UInt32 heightDiv, Bool doubleSided)
{
    UInt32 p1,p2,p3,p4;

    UInt32 verticeOffset = widthDiv+2;
    UInt32 *indices = m_pIndices;

    // 1...3
    // .   .
    // 2...4

    if (isAlternateTriangle()) {
        for (UInt32 y = 0 ; y < heightDiv+1 ; ++y) {
            for (UInt32 x = 0 ; x < widthDiv+1 ; ++x) {
                p1 = x + y * verticeOffset;
                p2 = p1 + verticeOffset;
                p3 = p2 + 1;
                p4 = p1 + 1;

                if ((x+y) % 2) {
                    // face 1
                    *indices++ = p1;
                    *indices++ = p3;
                    *indices++ = p2;

                    // face 2
                    *indices++ = p1;
                    *indices++ = p4;
                    *indices++ = p3;
                } else {
                    // face 1
                    *indices++ = p2;
                    *indices++ = p4;
                    *indices++ = p3;

                    // face 2
                    *indices++ = p1;
                    *indices++ = p4;
                    *indices++ = p2;
                }
            }
        }

        if (!doubleSided) {
            return;
        }

        // second part of the vertices
        UInt32 nextSideOffset = ((widthDiv+2) * (heightDiv+2));

        for (UInt32 y = 0 ; y < heightDiv+1 ; ++y) {
            for (UInt32 x = 0 ; x < widthDiv+1 ; ++x) {
                p1 = x + y * verticeOffset + nextSideOffset;
                p2 = p1 + verticeOffset;
                p3 = p2 + 1;
                p4 = p1 + 1;

                if ((x+y) % 2) {
                    // face 1
                    *indices++ = p1;
                    *indices++ = p2;
                    *indices++ = p3;

                    // face 2
                    *indices++ = p1;
                    *indices++ = p3;
                    *indices++ = p4;
                } else {
                    // face 1
                    *indices++ = p2;
                    *indices++ = p3;
                    *indices++ = p4;

                    // face 2
                    *indices++ = p1;
                    *indices++ = p2;
                    *indices++ = p4;
                }
            }
        }
    } else {
        for (UInt32 y = 0 ; y < heightDiv+1 ; ++y) {
            for (UInt32 x = 0 ; x < widthDiv+1 ; ++x) {
                p1 = x + y * verticeOffset;
                p2 = p1 + verticeOffset;
                p3 = p2 + 1;
                p4 = p1 + 1;

                // face 1
                *indices++ = p1;
                *indices++ = p3;
                *indices++ = p2;

                // face 2
                *indices++ = p1;
                *indices++ = p4;
                *indices++ = p3;
            }
        }

        if (!doubleSided) {
            return;
        }

        // second part of the vertices
        UInt32 nextSideOffset = ((widthDiv+2) * (heightDiv+2));

        for (UInt32 y = 0 ; y < heightDiv+1 ; ++y) {
            for (UInt32 x = 0 ; x < widthDiv+1 ; ++x) {
                p1 = x + y * verticeOffset + nextSideOffset;
                p2 = p1 + verticeOffset;
                p3 = p2 + 1;
                p4 = p1 + 1;

                // face 1
                *indices++ = p1;
                *indices++ = p2;
                *indices++ = p3;

                // face 2
                *indices++ = p1;
                *indices++ = p3;
                *indices++ = p4;
            }
        }
    }
}

void Surface::constructWired(UInt32 widthDiv, UInt32 heightDiv)
{
    UInt32 p1,p2,p3,p4;

    UInt32 verticeOffset = widthDiv+2;
    UInt32 *indices = m_pIndices;

    // no triangulation
    if (isMeta()) {
        // 1...2
        // .   .
        // 3...x

        for (UInt32 y = 0 ; y < heightDiv+1 ; ++y) {
            for (UInt32 x = 0 ; x < widthDiv+1 ; ++x) {
                p1 = x + y * verticeOffset;
                p2 = p1 + 1;
                p3 = p1 + verticeOffset;

                // hor. line
                *indices++ = p1;
                *indices++ = p2;

                // vert. line 2
                *indices++ = p1;
                *indices++ = p3;
            }

            // last vert. line
            *indices++ = p2; // *(indices-3);
            *indices++ = p2 /**(indices-1)*/ + verticeOffset;
        }

        // last hor. lines
        verticeOffset = (widthDiv+2)*(heightDiv+1);
        for (UInt32 x = 0 ; x < widthDiv+1 ; ++x) {
            *indices++ = verticeOffset++;
            *indices++ = verticeOffset;
        }
    } else {
        // 1...4
        // .   .
        // 2...3

        if (isAlternateTriangle()) {
            for (UInt32 y = 0 ; y < heightDiv+1 ; ++y) {
                for (UInt32 x = 0 ; x < widthDiv+1 ; ++x) {
                    p1 = x + y * verticeOffset;
                    p2 = p1 + verticeOffset;
                    p3 = p2 + 1;
                    p4 = p1 + 1;

                    // hor. line
                    *indices++ = p1;
                    *indices++ = p4;

                    // vert. line 2
                    *indices++ = p1;
                    *indices++ = p2;

                    if ((x+y) % 2) {
                        // triangulation line
                        *indices++ = p1;
                        *indices++ = p3;
                    } else {
                        // triangulation line
                        *indices++ = p2;
                        *indices++ = p4;
                    }
                }

                // last vert. line
                *indices++ = p4; // *(indices-5);
                *indices++ = p4 /**(indices-1)*/ + verticeOffset;
            }

            // last hor. lines
            verticeOffset = (widthDiv+2)*(heightDiv+1);
            for (UInt32 x = 0 ; x < widthDiv+1 ; ++x) {
                *indices++ = verticeOffset++;
                *indices++ = verticeOffset;
            }
        } else {
            for (UInt32 y = 0 ; y < heightDiv+1 ; ++y) {
                for (UInt32 x = 0 ; x < widthDiv+1 ; ++x) {
                    p1 = x + y * verticeOffset;
                    p2 = p1 + verticeOffset;
                    p3 = p2 + 1;
                    p4 = p1 + 1;

                    // hor. line
                    *indices++ = p1;
                    *indices++ = p4;

                    // vert. line 2
                    *indices++ = p1;
                    *indices++ = p2;

                    // triangulation line
                    *indices++ = p1;
                    *indices++ = p3;
                }

                // last vert. line
                *indices++ = p4; //*(indices-5);
                *indices++ = p4 /**(indices-1)*/ + verticeOffset;
            }

            // last hor. lines
            verticeOffset = (widthDiv+2)*(heightDiv+1);
            for (UInt32 x = 0 ; x < widthDiv+1 ; ++x) {
                *indices++ = verticeOffset++;
                *indices++ = verticeOffset;
            }
        }
    }
}

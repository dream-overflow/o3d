/**
 * @file cylinder.cpp
 * @brief Implementation of cylinder.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Julien IBARZ
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/primitive/cylinder.h"

#include "o3d/core/vector3.h"
#include "o3d/core/debug.h"

#include <math.h>

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Cylinder, ENGINE_CYLINDER, Primitive)

Cylinder::Cylinder(
    Float base1,
    Float base2,
    Float height,
    UInt32 slices,
    UInt32 stacks,
    UInt32 flags) :
        Primitive(flags),
            m_base1(base1),
            m_base2(base2),
            m_height(height),
            m_slices(slices),
            m_stacks(stacks)
{
    O3D_ASSERT(base1 > 0.f);
    O3D_ASSERT(base2 >= 0.f);
    // We can't build a circle with less than three slices (a triangle...)
    O3D_ASSERT(slices > 3);
    // We do not support height < 0.
    O3D_ASSERT(height > 0.f);

    if (isWired()) {
        m_verticesCount = slices * (stacks + 2);
        m_indicesCount = slices * 2 + slices * 2 * 2 * (stacks + 1);
    } else {
        m_verticesCount = (slices + 1) * (stacks + 2);
        m_indicesCount = slices * 3 * 2 + slices * 3 * 2 * (stacks + 1);
    }

    if (base2 == 0.f) {
        if (isWired()) {
            m_indicesCount -= slices * 2;
            m_verticesCount -= slices - 1;
        } else {
            m_indicesCount -= slices * 3 * 2;
            m_verticesCount -= slices;
        }
    }

    m_pVertices = new Float[m_verticesCount*3];
    m_pIndices = new UInt32[m_indicesCount];

    if (isWired()) {
        constructWired(base1,base2,height,slices,stacks);
    } else {
        constructFilled(base1,base2,height,slices,stacks);
    }
}

void Cylinder::constructWired(
    Float base1,
    Float base2,
    Float height,
    UInt32 slices,
    UInt32 stacks)
{
    // Construct the base begin
    // The begin circle
    Float angle = 0.;
    Float step = 2 * o3d::PI / slices;

    //  The vertice
    for (UInt32 i = 0 ; i < slices ; ++i) {
        const UInt32 verticeIndice = i * 3;
        m_pVertices[verticeIndice] = cos(angle) * base1;
        m_pVertices[verticeIndice + 1] = 0.;
        m_pVertices[verticeIndice + 2] = sin(angle) * base1;

        angle += step;
    }

    // The lines
    for (UInt32 i = 0 ; i < slices ; ++i) {
        const UInt32 triangleIndice = i * 2;
        m_pIndices[triangleIndice] = i;
        m_pIndices[triangleIndice + 1] = i + 1;
    }
    // Loop to the begin
    m_pIndices[(slices - 1) * 2 + 1] = 0;

    // Construct the body
    UInt32 lastOffSet = 0;
    UInt32 offSet = 0;
    const Float yStep = height / Float(stacks + 1);
    Float y = yStep;
    const UInt32 division = (base2 == 0.f) ? stacks : stacks + 1;
    for (UInt32 i = 0 ; i < division ; ++i) {
        lastOffSet = offSet;
        offSet += slices;

        // The vertice
        angle = 0.;
        const Float p = (base2 - base1) / (height - 0.f);
        for (UInt32 j = 0 ; j < slices ; ++j) {
            const UInt32 verticeIndice = (offSet + j) * 3;
            // Linear interpolation
            const Float radius = p * (y - 0.f) + base1;

            m_pVertices[verticeIndice] = cos(angle) * radius;
            m_pVertices[verticeIndice + 1] = y;
            m_pVertices[verticeIndice + 2] = sin(angle) * radius;

            angle += step;
        }
        y += yStep;

        // The lines
        for (UInt32 j = 0 ; j < slices ; ++j) {
            const UInt32 triangleIndice = slices * 2 + (i * slices + j) * 2 * 2;
            // Line of the circle
            m_pIndices[triangleIndice] = offSet + j;
            m_pIndices[triangleIndice + 1] = offSet + j + 1;
            // Line between the circles
            m_pIndices[triangleIndice + 2] = lastOffSet + j;
            m_pIndices[triangleIndice + 3] = offSet + j;

        }
        // Loop to the begin
        const UInt32 loopIndice = slices * 2 + (i * slices + slices - 1) * 2 * 2;
        m_pIndices[loopIndice + 1] = offSet;
    }

    if(base2 == 0.f) {
        lastOffSet = offSet;
        offSet += slices;
        // The end point
        m_pVertices[offSet * 3] = 0.f;
        m_pVertices[offSet * 3 + 1] = height;
        m_pVertices[offSet * 3 + 2] = 0.f;

        // The lines
        for (UInt32 j = 0 ; j < slices ; ++j) {
            const UInt32 triangleIndice = slices * 2 + (division * slices) * 2 * 2 + j * 2;
            // Line between the circles
            m_pIndices[triangleIndice] = lastOffSet + j;
            m_pIndices[triangleIndice + 1] = offSet;

        }
    }
}

void Cylinder::constructFilled(
    Float base1,
    Float base2,
    Float height,
    UInt32 slices,
    UInt32 stacks)
{
    // Construct the base begin
    // The center
    m_pVertices[0] = 0.;
    m_pVertices[1] = 0.;
    m_pVertices[2] = 0.;

    // The begin circle
    Float angle = 0.;
    Float step = 2 * o3d::PI / slices;

    //  The vertices
    for (UInt32 i = 0 ; i < slices ; ++i) {
        const UInt32 verticeIndice = 3 + i * 3;
        m_pVertices[verticeIndice] = sin(angle) * base1;
        m_pVertices[verticeIndice + 1] = 0.;
        m_pVertices[verticeIndice + 2] = cos(angle) * base1;

        angle += step;
    }

    // The triangles
    for (UInt32 i = 0 ; i < slices ; ++i) {
        const UInt32 triangleIndice = i * 3;
        m_pIndices[triangleIndice] = 0;
        m_pIndices[triangleIndice + 1] = i + 2;
        m_pIndices[triangleIndice + 2] = i + 1;
    }

    // Loop to the begin
    m_pIndices[(slices - 1) * 3 + 1] = 1;

    // Construct the body
    UInt32 lastOffSet = 0;
    UInt32 offSet = 0;
    const Float yStep = height / Float(stacks + 1);
    Float y = yStep;
    const UInt32 division = (base2 == 0.f) ? stacks : stacks + 1;
    for (UInt32 i = 0 ; i < division ; ++i) {
        lastOffSet = offSet;
        offSet += slices + 1;

        // The vertices
        angle = -step;
        const Float p = (base2 - base1) / (height - 0.f);
        for (UInt32 j = 0 ; j < slices + 1 ; ++j) {
            const UInt32 verticeIndice = (offSet + j) * 3;
            // Linear interpolation
            const Float radius = p * (y - 0.f) + base1;

            if (j == 0) {
                m_pVertices[verticeIndice] = 0.f;
                m_pVertices[verticeIndice + 2] = 0.f;
            } else {
                m_pVertices[verticeIndice] = sin(angle) * radius;
                m_pVertices[verticeIndice + 2] = cos(angle) * radius;
            }
            m_pVertices[verticeIndice + 1] = y;

            angle += step;
        }
        y += yStep;

        // The triangles
        for (UInt32 j = 0 ; j < slices ; ++j) {
            // First triangle
            const UInt32 triangleIndice = slices * 3 + (i * slices + j) * 3 * 2;
            m_pIndices[triangleIndice] = lastOffSet + j + 1;
            m_pIndices[triangleIndice + 1] = lastOffSet + j + 2;
            m_pIndices[triangleIndice + 2] = offSet + j + 1;
            // Second triangle
            m_pIndices[triangleIndice + 3] = lastOffSet + j + 2;
            m_pIndices[triangleIndice + 4] = offSet + j + 2;
            m_pIndices[triangleIndice + 5] = offSet + j + 1;
        }
        // Loop to the begin
        const UInt32 loopIndice = slices * 3 + (i * slices + slices - 1) * 3 * 2;
        m_pIndices[loopIndice + 1] = lastOffSet + 1;
        m_pIndices[loopIndice + 3] = lastOffSet + 1;
        m_pIndices[loopIndice + 4] = offSet + 1;
    }

    // for cone
    if (base2 == 0.f) {
        lastOffSet = offSet;
        offSet += slices + 1;

        // End point
        m_pVertices[offSet * 3] = 0.f;
        m_pVertices[offSet * 3 + 1] = height;
        m_pVertices[offSet * 3 + 2] = 0.f;

        // The triangles
        for (UInt32 j = 0 ; j < slices ; ++j) {
            // First triangle
            const UInt32 triangleIndice = slices * 3 + (division * slices) * 3 * 2 + j * 3;
            m_pIndices[triangleIndice] = lastOffSet + j + 1;
            m_pIndices[triangleIndice + 1] = lastOffSet + j + 2;
            m_pIndices[triangleIndice + 2] = offSet;
        }
        // Loop to the begin
        const UInt32 loopIndice = slices * 3 + (division * slices) * 3 * 2 + (slices - 1) * 3;
        m_pIndices[loopIndice + 1] = lastOffSet + 1;
    } else {
        // non null base2
        UInt32 triangleIndice = slices * 3 + (stacks * slices + slices) * 3 * 2; // or m_indicesCount - slices * 3;
        lastOffSet += slices + 1;

        // The triangles
        for (UInt32 i = 0 ; i < slices ; ++i)
        {
            m_pIndices[triangleIndice] = offSet;
            m_pIndices[triangleIndice + 1] = lastOffSet + i + 1;
            m_pIndices[triangleIndice + 2] = lastOffSet + i + 2;

            triangleIndice += 3;
        }

        // Loop to the begin
        m_pIndices[triangleIndice - 1] = lastOffSet + 1;
    }
}

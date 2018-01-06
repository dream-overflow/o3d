/**
 * @file sphere.cpp
 * @brief Implementation of sphere.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Julien IBARZ
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/primitive/sphere.h"

#include "o3d/core/vector3.h"
#include "o3d/core/debug.h"

#include <math.h>

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Sphere, ENGINE_SPHERE, Primitive)

Sphere::Sphere(
    Float radius,
    UInt32 slices,
    UInt32 stacks,
    UInt32 flags) :
        Primitive(flags),
            m_radius(radius),
            m_slices(slices),
            m_stacks(stacks)
{
    O3D_ASSERT(radius > 0.);
    O3D_ASSERT(slices >= 3);
    O3D_ASSERT(stacks >= 1);

    if (isWired() && isTexCoords()) {
        O3D_ERROR(E_InvalidParameter("Texture coordinate can not be generated in wireframe mode"));
        return;
    }

    constructVertices(radius, slices, stacks);

    if (isWired()) {
        constructWired(radius, slices, stacks);
    } else {
        constructFilled(radius, slices, stacks);
    }
}

Sphere::TexCoordCorrectionPolicy Sphere::getTexCoordCorrectionPolicy() const
{
    if ((m_capacities & Sphere::FAST_CORRECTION) == Sphere::FAST_CORRECTION) {
        return Sphere::FAST_CORRECTION;
    } else {
        return Sphere::NO_CORRECTION;
    }
}

void Sphere::constructVertices(Float radius, UInt32 slices, UInt32 stacks)
{
    // If texture is enabled, one vertex must be added on each stack.
    UInt32 lVerticesCount = 2 + stacks * slices + (isTexCoords() ? stacks : 0);

    // Reallocate a new buffer only if needed
    if ((m_pVertices != nullptr) && (m_verticesCount != lVerticesCount)) {
        deleteArray(m_pVertices);
        deleteArray(m_pTexCoords);
    }

    m_verticesCount = lVerticesCount;

    if (m_pVertices == nullptr) {
        m_pVertices = new Float[3*m_verticesCount];
    }

    if ((m_pTexCoords == nullptr) && isTexCoords()) {
        m_pTexCoords = new Float[2*m_verticesCount];
    }

    UInt32 offSet = 0;
    // Construct the vertices
    m_pVertices[offSet++] = 0.f;
    m_pVertices[offSet++] = -radius;
    m_pVertices[offSet++] = 0.f;
    m_pVertices[offSet++] = 0.f;
    m_pVertices[offSet++] = radius;
    m_pVertices[offSet++] = 0.f;

    Float lStackAngle = 0.0f;
    Float lStackAngleStep = 0.0f;
    const Float lSliceAngleStep = 2.f * o3d::PI / slices;

    if (getTexCoordCorrectionPolicy() == Sphere::FAST_CORRECTION) {
        // DO NOT forget to modify the threshold in the tex coordinates generation
        const Float lStackCorrectionThreshold = 0.01f;

        Float lStackCorrection = atan(lStackCorrectionThreshold);
        lStackAngle = - 0.5f * o3d::PI + lStackCorrection;
        lStackAngleStep = (o3d::PI - 2.f * lStackCorrection) / (stacks - 1);
    } else {
        lStackAngleStep = o3d::PI / (stacks + 1);
        lStackAngle = - 0.5f * o3d::PI + lStackAngleStep;
    }

    for (UInt32 i = 0 ; i < stacks ; ++i, lStackAngle += lStackAngleStep) {
        Float lSliceAngle = 0.f;
        const Float lStackRadius = radius * cos(lStackAngle);
        const Float lStackPos = radius * sin(lStackAngle);

        for (UInt32 j = 0 ; j < slices ; ++j, lSliceAngle += lSliceAngleStep) {
            m_pVertices[offSet++] = cos(lSliceAngle) * lStackRadius;
            m_pVertices[offSet++] = lStackPos;
            m_pVertices[offSet++] = sin(lSliceAngle) * lStackRadius;
        }

        if (isTexCoords()) {
            m_pVertices[offSet++] = lStackRadius;
            m_pVertices[offSet++] = lStackPos;
            m_pVertices[offSet++] = 0.0f;
        }
    }

    if (isTexCoords()) {
        // Computation of texture coordinates
        offSet = 0;

        m_pTexCoords[offSet++] = 0.5f;
        m_pTexCoords[offSet++] = 1.0f;

        m_pTexCoords[offSet++] = 0.5f;
        m_pTexCoords[offSet++] = 0.0f;

        Float lIStacks = 0.0f;
        Float lISlices = 0.0f;
        Float lV = 0.0f;

        if (getTexCoordCorrectionPolicy() == Sphere::FAST_CORRECTION) {
            const Float lStackCorrectionThreshold = 0.01f;
            Float lStackCorrection = 0.0f;

            lStackCorrection = o3d::IVI_PI * atan(lStackCorrectionThreshold);
            lIStacks = (1.0f - 2.0f * lStackCorrection) / (stacks - 1);
            lISlices = 1.0f / slices;

            lV = 1.0f - lStackCorrection;
        } else {
            lIStacks = 1.0f / (stacks + 1);
            lISlices = 1.0f / slices;

            lV = 1.0f - lIStacks;
        }

        for (UInt32 i = 0 ; i < stacks ; ++i, lV -= lIStacks) {
            Float lU = 1.0f;

            for (UInt32 j = 0 ; j < slices ; ++j, lU -= lISlices) {
                m_pTexCoords[offSet++] = lU;;
                m_pTexCoords[offSet++] = lV;
            }

            m_pTexCoords[offSet++] = 0.0f;
            m_pTexCoords[offSet++] = lV;
        }
    }
}

void Sphere::constructFilled(Float /*radius*/, UInt32 slices, UInt32 stacks)
{
    const UInt32 lRealSlices = slices + (isTexCoords() ? 1 : 0);
    const UInt32 lNewIndicesCount = (2 * lRealSlices * stacks)* 3;

    // Reallocate a new buffer only if needed
    if ((m_pIndices != nullptr) && (m_indicesCount != lNewIndicesCount)) {
        deleteArray(m_pIndices);
    }

    m_indicesCount = lNewIndicesCount;

    if (m_pIndices == nullptr) {
        m_pIndices = new UInt32[m_indicesCount];
    }

    UInt32 offSet = 0;

    // Loop for the first and the end
    for (UInt32 i = 0 ; i < lRealSlices-1 ; ++i) {
        // Begin triangle
        m_pIndices[offSet++] = 0;
        m_pIndices[offSet++] = 2 + i;
        m_pIndices[offSet++] = 2 + i + 1;
        // End triangle
        const UInt32 offSetVertice = m_verticesCount - i - 1;
        m_pIndices[offSet++] = 1;
        m_pIndices[offSet++] = offSetVertice;
        m_pIndices[offSet++] = offSetVertice - 1;
    }

    // Last iteration
    m_pIndices[offSet++] = 0;
    m_pIndices[offSet++] = 2 + lRealSlices-1;
    m_pIndices[offSet++] = 2;
    const UInt32 offSetVertice = m_verticesCount - lRealSlices;
    m_pIndices[offSet++] = 1;
    m_pIndices[offSet++] = offSetVertice;
    m_pIndices[offSet++] = m_verticesCount - 1;

    // Loop
    m_pIndices[offSet - 1] = m_verticesCount - 1;
    m_pIndices[offSet - 4] = 2;

    // Loop between the stacks
    UInt32 verticeOffSet = 2;
    for (UInt32 i = 0 ; i < stacks - 1 ; ++i) {
        const UInt32 verticeBegin = verticeOffSet;
        verticeOffSet += lRealSlices;

        for (UInt32 j = 0 ; j < lRealSlices-1 ; ++j) {
            // First triangle
            m_pIndices[offSet++] = verticeBegin + j;
            m_pIndices[offSet++] = verticeOffSet + j + 1;
            m_pIndices[offSet++] = verticeBegin + j + 1;
            // Second triangle
            m_pIndices[offSet++] = verticeOffSet + j + 1;
            m_pIndices[offSet++] = verticeBegin + j;
            m_pIndices[offSet++] = verticeOffSet + j;
        }

        // Last iteration
        // First triangle
        m_pIndices[offSet++] = verticeBegin + lRealSlices-1;
        m_pIndices[offSet++] = verticeOffSet;
        m_pIndices[offSet++] = verticeBegin;
        // Second triangle
        m_pIndices[offSet++] = verticeOffSet;
        m_pIndices[offSet++] = verticeBegin + lRealSlices-1;
        m_pIndices[offSet++] = verticeOffSet + lRealSlices-1;
    }
}

void Sphere::constructWired(Float /*radius*/, UInt32 slices, UInt32 stacks)
{
    const UInt32 lNewIndicesCount = (stacks * slices + slices * (stacks + 1)) * 2;

    // Reallocate a new buffer only if needed
    if ((m_pIndices != nullptr) && (m_indicesCount != lNewIndicesCount)) {
        deleteArray(m_pIndices);
    }

    m_indicesCount = lNewIndicesCount;

    if (m_pIndices == nullptr) {
        m_pIndices = new UInt32[m_indicesCount];
    }

    UInt32 offSet = 0;
    UInt32 offSetVertice = 2;

    // Loop for the first and the end
    for (UInt32 i = 0 ; i < slices ; ++i) {
        // Begin Line
        m_pIndices[offSet++] = 0;
        m_pIndices[offSet++] = 2 + i;
    }

    for (UInt32 i = 0 ; i < slices ; ++i) {
        // End Line
        m_pIndices[offSet++] = 1;
        m_pIndices[offSet++] = m_verticesCount - 1 - i;
    }

    // The stacks circles
    for (UInt32 i = 0 ; i < stacks ; ++i) {
        const UInt32 verticeBegin = offSetVertice;

        for (UInt32 j = 0 ; j < slices ; ++j) {
            m_pIndices[offSet++] = offSetVertice;
            m_pIndices[offSet++] = ++offSetVertice;
        }
        // Loop to begin
        m_pIndices[offSet - 1] = verticeBegin;
    }

    // Loop between the stacks
    UInt32 verticeOffSet = 2;
    for (UInt32 i = 0 ; i < stacks - 1 ; ++i) {
        const UInt32 verticeBegin = verticeOffSet;
        verticeOffSet += slices;

        for (UInt32 j = 0 ; j < slices ; ++j) {
            // First triangle
            m_pIndices[offSet++] = verticeBegin + j;
            m_pIndices[offSet++] = verticeOffSet + j;
        }
    }
}

/**
 * @file primitive.cpp
 * @brief Implementation of primitive.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Julien IBARZ
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/primitive/primitive.h"

#include "o3d/core/vector3.h"
#include "o3d/core/debug.h"

#include <math.h>

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Primitive, ENGINE_PRIMITIVE, BaseObject)

Primitive::Primitive(UInt32 flags) :
    BaseObject(nullptr),
		m_capacities(flags),
		m_verticesCount(0),
        m_pVertices(nullptr),
		m_indicesCount(0),
        m_pIndices(nullptr),
        m_pTexCoords(nullptr)
{
}

Primitive::Primitive(
	UInt32 _verticesCount,
	UInt32 _indicesCount,
	UInt32 _flags) :
        BaseObject(nullptr),
			m_capacities(_flags),
			m_verticesCount(_verticesCount),
            m_pVertices(nullptr),
			m_indicesCount(_indicesCount),
            m_pIndices(nullptr),
            m_pTexCoords(nullptr)
{
	m_pVertices = new Float[_verticesCount*3];
	O3D_ASSERT(m_pVertices);

	m_pIndices = new UInt32[_indicesCount];
	O3D_ASSERT(m_pIndices);

    if ((m_capacities & GEN_TEX_COORDS) == GEN_TEX_COORDS) {
		m_pTexCoords = new Float[_verticesCount*2];
		O3D_ASSERT(m_pTexCoords);
	}
}

Primitive::Primitive(const Primitive &dup) :
	BaseObject(dup),
		m_capacities(dup.m_capacities),
		m_verticesCount(dup.m_verticesCount),
        m_pVertices(nullptr),
		m_indicesCount(dup.m_indicesCount),
        m_pIndices(nullptr),
        m_pTexCoords(nullptr)
{
}

Primitive::~Primitive()
{
	deleteArray(m_pVertices);
	deleteArray(m_pIndices);
	deleteArray(m_pTexCoords);
}

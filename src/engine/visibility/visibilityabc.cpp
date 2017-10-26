/**
 * @file visibilityabc.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-11-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/visibility/visibilityabc.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(VisibilityABC, ENGINE_VISIBILITY_ABC, SceneEntity)

// default constructor
VisibilityABC::VisibilityABC(
		BaseObject *parent,
		const Vector3 &position,
		const Vector3 &halfSize) :
			SceneEntity(parent),
			m_bbox(position, halfSize)
{
}

// destructor
VisibilityABC::~VisibilityABC()
{
}


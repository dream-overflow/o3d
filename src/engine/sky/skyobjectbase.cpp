/**
 * @file skyobjectbase.cpp
 * @brief Base class for all sky objects
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-05-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/sky/skyobjectbase.h"

#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(SkyObjectBase, ENGINE_SKY_OBJECT_BASE, SceneObject)

//! A constructor
SkyObjectBase::SkyObjectBase(BaseObject * _pParent):
	SceneObject(_pParent)
{
}

//! The destructor
SkyObjectBase::~SkyObjectBase()
{
}

//! @brief Return Scene::DrawSkyObject
UInt32 SkyObjectBase::getDrawType() const
{
	return Scene::DRAW_SKY_OBJECT;
}

//! Call when the object must be updated
void SkyObjectBase::update()
{
}


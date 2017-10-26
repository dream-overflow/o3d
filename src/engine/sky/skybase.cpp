/**
 * @file skybase.cpp
 * @brief Base class for all sky renderers
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-05-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/sky/skybase.h"

#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(SkyBase, ENGINE_SKY_BASE, SceneObject)

SkyBase::SkyBase(BaseObject * _pParent):
	SceneObject(_pParent)
{
}

SkyBase::~SkyBase()
{
}

UInt32 SkyBase::getDrawType() const
{
	return Scene::DRAW_SKY;
}

void SkyBase::setTime(Double)
{
}

Double SkyBase::getTime()
{
	return 0.0;
}


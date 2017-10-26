/**
 * @file cloudlayerbase.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2009-30-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/sky/cloudlayerbase.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

CloudLayerBase::CloudLayerBase(BaseObject * _pParent):
	SceneObject(_pParent)
{
}

CloudLayerBase::~CloudLayerBase()
{
}

UInt32 CloudLayerBase::getDrawType() const
{
	return Scene::DRAW_CLOUD_LAYER;
}

void CloudLayerBase::setTime(Double)
{
}

bool CloudLayerBase::project(const Vector3 & _dir, SmartArrayFloat & _target)
{
	return true;
}


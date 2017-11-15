/**
 * @file scenedrawer.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/scene/scenedrawer.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(SceneDrawer, ENGINE_SCENE_DRAWER, SceneEntity)

SceneDrawer::SceneDrawer(BaseObject *parent) :
    SceneEntity(parent)
{
}


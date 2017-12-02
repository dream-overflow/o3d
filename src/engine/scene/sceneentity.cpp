/**
 * @file sceneentity.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/scene/sceneentity.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(SceneEntity, ENGINE_SCENE_ENTITY, BaseObject)

Bool SceneEntity::hasTopLevelParentTypeOf() const
{
    if (m_topLevelParent) {
		return typeOf<Scene>(m_topLevelParent);
    } else {
		return False;
    }
}

O3D_IMPLEMENT_ABSTRACT_CLASS1(SceneResourceManager, ENGINE_SCENE_RESOURCE_MANAGER, ResourceManager)

Bool SceneResourceManager::hasTopLevelParentTypeOf() const
{
    if (m_topLevelParent) {
		return typeOf<Scene>(m_topLevelParent);
    } else {
		return False;
    }
}

O3D_IMPLEMENT_ABSTRACT_CLASS1(SceneResource, ENGINE_SCENE_RESOURCE, Resource)

Bool SceneResource::hasTopLevelParentTypeOf() const
{
    if (m_topLevelParent) {
		return (typeOf<Scene>(m_topLevelParent));
    } else {
		return False;
    }
}

/**
 * @file engineentity.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/engine.h"
#include "o3d/engine/engineentity.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(EngineEntity, ENGINE_ENGINE_ENTITY, BaseObject)

Bool EngineEntity::hasTopLevelParentTypeOf() const
{
    if (m_topLevelParent)
        return typeOf<Engine>(m_topLevelParent);
    else
        return False;
}

O3D_IMPLEMENT_ABSTRACT_CLASS1(EngineResourceManager, ENGINE_ENGINE_RESOURCE_MANAGER, ResourceManager)

Bool EngineResourceManager::hasTopLevelParentTypeOf() const
{
    if (m_topLevelParent)
        return typeOf<Engine>(m_topLevelParent);
    else
        return False;
}

O3D_IMPLEMENT_ABSTRACT_CLASS1(EngineResource, ENGINE_ENGINE_RESOURCE, Resource)

Bool EngineResource::hasTopLevelParentTypeOf() const
{
    if (m_topLevelParent)
        return (typeOf<Engine>(m_topLevelParent));
    else
        return False;
}


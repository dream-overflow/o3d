/**
 * @file sceneobjectmanager.cpp
 * @brief Implementation of SceneObjectManager.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-12-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/scene/sceneobjectmanager.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/core/classfactory.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(SceneObjectManager, ENGINE_SCENEOBJECT_LIST, SceneEntity)

// Default constructor
SceneObjectManager::SceneObjectManager(BaseObject *parent) :
	SceneEntity(parent),
	m_singleId(0),
    m_indexToObject(nullptr),
	m_indexToObjectSize(0)
{
	m_name = "SceneObjectManager";
}

SceneObjectManager::~SceneObjectManager()
{
	deleteArray(m_indexToObject);
}

// Resize the list of currently importing special effects.
void SceneObjectManager::resizeImportedSceneObject(UInt32 size)
{
	deleteArray(m_indexToObject);
	m_indexToObject = new SceneObject*[size];
	m_indexToObjectSize = size;
}


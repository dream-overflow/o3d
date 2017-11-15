/**
 * @file scenetemplate.cpp
 * @brief Implementation of SceneTemplate.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-04-18
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/scene/scenetemplate.h"

#include "o3d/engine/animation/animatable.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/scene/sceneobjectmanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/hierarchy/hierarchytree.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/datainstream.h"

using namespace o3d;

// class SceneTplInstance
SceneTplInstance::~SceneTplInstance()
{
    if (m_autodelete)
        destroy();
}

void SceneTplInstance::destroy()
{
	if (m_valid)
	{
		for (IT_SceneObjectList it = m_rootObjects.begin(); it != m_rootObjects.end(); ++it)
		{
			(*it)->deleteIt();
		}

		m_rootObjects.clear();

		m_loadedObject.clear();
		m_valid = False;
	}
}

// class SceneTemplate
SceneTemplate::SceneTemplate(
		const String &filename,
		const String &name) :
	m_tplname(name),
	m_isvalid(False),
    m_is(nullptr)
{
    InStream *is = FileManager::instance()->openInStream(filename);

    // it want data in memory
    SmartArrayUInt8 data(is->getAvailable());
    Int64 pos = 0, size = 4096;
    while (!is->isEnd())
    {
        size = o3d::min(4096, is->getAvailable());
        is->read(data.getData() + pos, size);
        pos += size;
    }

    deletePtr(is);

    m_is = new SharedDataInStream(data);
    m_isvalid = True;
}

SceneTemplate::SceneTemplate(
        InStream &is,
        const String &name) :
	m_tplname(name),
	m_isvalid(False),
    m_is(nullptr)
{
    // it want data in memory
    SmartArrayUInt8 data(is.getAvailable());
    Int64 pos = 0, size = 4096;
    while (!is.isEnd())
    {
        size = o3d::min(4096, is.getAvailable());
        is.read(data.getData() + pos, size);
    }

    m_is = new SharedDataInStream(data);
    m_isvalid = True;
}

// dtor
SceneTemplate::~SceneTemplate()
{
    if (m_is)
        deletePtr(m_is);
}

// create a new instance of the template
SceneTplInstance* SceneTemplate::create(Scene *scene, const String &specifyName)
{
    O3D_ASSERT(scene);

    scene->keepImportArrays(True);

    if (!scene->importScene(*m_is, nullptr))
		return NULL;

    SceneTplInstance *instance = new SceneTplInstance(scene);

    instance->m_loadedPlayer = scene->getAnimationPlayerManager()->getImportedPlayers();
	instance->m_valid = True;

	if (specifyName.length())
		instance->m_name = specifyName;
	else
		instance->m_name = m_tplname;

    for (UInt32 i = 0; i < scene->getSceneObjectManager()->getNumImportedSceneObjects(); ++i)
	{
        instance->m_loadedObject[scene->getSceneObjectManager()->getImportedSceneObject(i)->getName()] =
                scene->getSceneObjectManager()->getImportedSceneObject(i);
	}

    instance->m_rootObjects = scene->getHierarchyTree()->getLastImportedObjects();

    scene->clearImportArrays();
    scene->keepImportArrays(False);

	return instance;
}

//---------------------------------------------------------------------------------------
// class SceneTplMgr
//---------------------------------------------------------------------------------------

// instanciation du singleton
SceneTplMgr* SceneTplMgr::m_instance = nullptr;

// Singleton instanciation
SceneTplMgr* SceneTplMgr::instance()
{
	if(!m_instance) m_instance = new SceneTplMgr();
	return m_instance;
}

// Singleton destruction
void SceneTplMgr::destroy()
{
	if (m_instance)
	{
		delete m_instance;
        m_instance = nullptr;
	}
}

// Constructor
SceneTplMgr::SceneTplMgr()
{
	m_instance = (SceneTplMgr*)this; // Used to avoid recursive call when the ctor call himself...
}

// dtor
SceneTplMgr::~SceneTplMgr()
{
	for (O3D_IT_SceneTemplateMap it = m_templateMap.begin(); it != m_templateMap.end(); ++it)
		deletePtr(it->second);
}

// add a template
Bool SceneTplMgr::add(
		const String &filename,
		const String &name)
{
	O3D_CIT_SceneTemplateMap cit = m_templateMap.find(name);
	if (cit != m_templateMap.end())
		O3D_ERROR(E_InvalidParameter(name));

	SceneTemplate *pTpl = new SceneTemplate(filename,name);
	if (!pTpl->isValid())
	{
		deletePtr(pTpl);
		return False;
	}

	m_templateMap[name] = pTpl;

	return True;
}

// create a new instance of the template
SceneTplInstance* SceneTplMgr::create(
		Scene *pScene,
		const String &name,
		const String &specifyName)
{
	O3D_CIT_SceneTemplateMap cit = m_templateMap.find(name);
	if (cit != m_templateMap.end())
	{
		return cit->second->create(pScene, specifyName);
	}

    return nullptr;
}

// find a template by its name
Bool SceneTplMgr::find(const String &name)
{
	O3D_CIT_SceneTemplateMap cit = m_templateMap.find(name);
	if (cit != m_templateMap.end())
	{
		return True;
	}

	return False;
}

// find a template by its filename
Bool SceneTplMgr::findByFileName(const String &filename)
{
	for (O3D_CIT_SceneTemplateMap cit = m_templateMap.begin(); cit != m_templateMap.end(); ++cit)
	{
		if (cit->second->getFileName() == filename)
			return True;
	}

	return False;
}

// delete a template by its name
void SceneTplMgr::revove(const String &name)
{
	O3D_IT_SceneTemplateMap it = m_templateMap.find(name);
	if (it != m_templateMap.end())
	{
		deletePtr(it->second);
		m_templateMap.erase(it);
	}
}

// delete a teplate by its filename
void SceneTplMgr::removeByFileName(const String &filename)
{
	for (O3D_IT_SceneTemplateMap it = m_templateMap.begin(); it != m_templateMap.end(); ++it)
	{
		if (it->second->getFileName() == filename)
		{
			deletePtr(it->second);
			m_templateMap.erase(it);
		}
	}
}


/**
 * @file landscape.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/landscape.h"

#include "o3d/core/error.h"

#include "o3d/engine/object/camera.h"
#include "o3d/engine/landscape/landscape.h"

#include "o3d/engine/landscape/pclod/terrain.h"

#include "o3d/engine/object/camera.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Landscape, ENGINE_LANDSCAPE, SceneEntity)

//---------------------------------------------------------------------------------------
// class Landscape
//---------------------------------------------------------------------------------------
Landscape::Landscape(BaseObject *parent, const String &path) :
	SceneEntity(parent),
		m_terrainManager(this),
		m_terrainDefManager(this, path)
{
}

Landscape::~Landscape()
{
}

//---------------------------------------------------------------------------------------
// Processing
//---------------------------------------------------------------------------------------

void Landscape::draw()
{
	m_terrainManager.draw();
}

void Landscape::update()
{
	m_terrainManager.update();
}

//---------------------------------------------------------------------------------------
// class TerrainManager
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_DYNAMIC_CLASS1(TerrainManager, ENGINE_TERRAIN_MANAGER, SceneEntity)

TerrainManager::TerrainManager(BaseObject *parent) :
	SceneEntity(parent)
{
}

TerrainManager::~TerrainManager()
{
    if (!m_findMap.empty()) {
		String message("Terrains still exists into the manager :\n");

        for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it) {
            message += "    |- " + it->second->getName() + "\n";
			deletePtr(it->second);
		}

		O3D_WARNING(message);
	}
}

// Delete child .
Bool TerrainManager::deleteChild(BaseObject *child)
{
    if (child) {
        if (child->getParent() != this) {
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
        } else {
			// is it a terrain, and is it managed by this
			TerrainBase *terrain = o3d::dynamicCast<TerrainBase*>(child);
            if (terrain && (terrain->m_manager == this)) {
				deleteTerrain(terrain);
				return True;
			}

			// otherwise simply delete the child
			deletePtr(child);
			return True;
		}
	}
	return False;
}

void TerrainManager::addTerrain(TerrainBase *terrain)
{
    if (terrain->m_manager != nullptr) {
		O3D_ERROR(E_InvalidOperation("The given terrain already have a manager"));
    } else {
        FastMutexLocker locker(m_mutex);

        String keyName = terrain->getName();

        // search for the terrain
        IT_FindMap it = m_findMap.find(keyName);
        if (it != m_findMap.end()) {
            O3D_ERROR(E_InvalidParameter("A same terrain with the same name already exists"));
        } else {
            m_findMap.insert(std::make_pair(keyName, terrain));
        }

        O3D_MESSAGE("Add terrain \"" + keyName + "\"");

        // this is the manager and its parent
        terrain->m_manager = this;
        terrain->setParent(this);
        terrain->setId(m_IDManager.getID());
    }
}

void TerrainManager::removeTerrain(TerrainBase *terrain)
{
    if (terrain->m_manager != this) {
		O3D_ERROR(E_InvalidParameter("Terrain manager is not this"));
    } else {
        FastMutexLocker locker(m_mutex);

        String keyName = terrain->getName();

        // remove the terrain object from the manager.
        IT_FindMap it = m_findMap.find(keyName);
        if (it != m_findMap.end()) {
            terrain->m_manager = nullptr;
            terrain->setParent(getScene());

            m_IDManager.releaseID(terrain->getId());

            terrain->setId(-1);

            m_findMap.erase(it);

            O3D_MESSAGE("Remove (not delete) existing terrain : " + keyName);
        }
    }
}

void TerrainManager::deleteTerrain(TerrainBase *terrain)
{
    if (terrain->m_manager != this) {
		O3D_ERROR(E_InvalidParameter("Terrain manager is not this"));
    } else {
        FastMutexLocker locker(m_mutex);

        String keyName = terrain->getName();

        // remove the terrain object from the manager.
        IT_FindMap it = m_findMap.find(keyName);
        if (it != m_findMap.end()) {
            m_IDManager.releaseID(terrain->getId());

            deletePtr(terrain);

            m_findMap.erase(it);

            O3D_MESSAGE("Delete existing terrain : " + keyName);
        }
    }
}

TerrainBase* TerrainManager::findTerrain(UInt32 type, const String &keyName)
{
	FastMutexLocker locker(m_mutex);

	// search the key name into the map, next the terrain given parameters into the element
	CIT_FindMap cit = m_findMap.find(keyName);
    if (cit != m_findMap.end()) {
		// found it ?
		return cit->second;
	}

	// not found
    return nullptr;
}

Bool TerrainManager::isTerrain(const String &name)
{
	String keyName = name;
    return (findTerrain(0, keyName) != nullptr);
}

void TerrainManager::draw()
{
	FastMutexLocker locker(m_mutex);

    for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it) {
        if (getScene()->getActiveCamera() == it->second->getCamera()) {
			(*it).second->draw();
        }
	}
}

void TerrainManager::update()
{
	FastMutexLocker locker(m_mutex);

	// process deferred objects deletion
    // processDeferred();

	// process all terrain updates
    for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it) {
		(*it).second->update();
    }
}

//---------------------------------------------------------------------------------------
// class TerrainDataManager
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_DYNAMIC_CLASS1(TerrainDefManager, ENGINE_TERRAIN_DEF_MANAGER, SceneResourceManager)

TerrainDefManager::TerrainDefManager(
		BaseObject *parent,
		const String &path) :
	SceneResourceManager(parent, path, "*.map")
{
}

TerrainDefManager::~TerrainDefManager()
{
    if (!m_findMap.empty()) {
		String message("Terrain definition still exists into the manager:\n");

        for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it) {
            message += "    |- " + it->second->getKeyName() + "\n";
			deletePtr(it->second);
		}

		O3D_WARNING(message);
	}
}

// Delete child .
Bool TerrainDefManager::deleteChild(BaseObject *child)
{
    if (child) {
        if (child->getParent() != this) {
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
        } else {
			// is it a terrain def, and is it managed by this
			TerrainDef *terrainDef = o3d::dynamicCast<TerrainDef*>(child);
            if (terrainDef && (terrainDef->getManager() == this)) {
				deleteTerrainDef(terrainDef);
				return True;
			}

			// otherwise simply delete the child
			deletePtr(child);
			return True;
		}
	}
	return False;
}

TerrainDef* TerrainDefManager::findTerrainDef(const String &keyName)
{
	FastMutexLocker locker(m_mutex);

	// search the key name into the map, next the texture given parameters into the element
	CIT_FindMap cit = m_findMap.find(keyName);
    if (cit != m_findMap.end()) {
		return cit->second;
    } else {
        return nullptr;
    }
}

// Manage an existing terrain definition.
void TerrainDefManager::addTerrainDef(TerrainDef *terrainDef)
{
    if (terrainDef->getManager() != nullptr) {
		O3D_ERROR(E_InvalidOperation("The given terrain definition already have a manager"));
    } else {
        FastMutexLocker locker(m_mutex);

        // search for the terrain definition key
        IT_FindMap it = m_findMap.find(terrainDef->getKeyName());
        if (it != m_findMap.end()) {
            O3D_ERROR(E_InvalidParameter("The given terrain definition already exists into this manager"));
        }

        m_findMap.insert(std::pair<String, TerrainDef*>(terrainDef->getKeyName(), terrainDef));

        O3D_MESSAGE("Add terrain definition \"" + terrainDef->getKeyName() + "\"");

        // this is the manager and its parent
        terrainDef->setManager(this);
        terrainDef->setParent(this);
        terrainDef->setId(m_IDManager.getID());
    }
}

// Remove an existing terrain definition from the manager.
void TerrainDefManager::removeTerrainDef(TerrainDef *terrainDef)
{
    if (terrainDef->getManager() != this) {
		O3D_ERROR(E_InvalidParameter("Terrain definition manager is not this"));
    } else {
        FastMutexLocker locker(m_mutex);

        // remove the terrain definition object from the manager.
        IT_FindMap it = m_findMap.find(terrainDef->getKeyName());
        if (it != m_findMap.end()) {
            terrainDef->setManager(nullptr);
            terrainDef->setParent(getScene());

            m_IDManager.releaseID(terrainDef->getId());
            terrainDef->setId(-1);

            O3D_MESSAGE("Remove (not delete) existing terrain definition: " + terrainDef->getKeyName());

            m_findMap.erase(it);
        }
    }
}

// Delete a managed terrain definition.
void TerrainDefManager::deleteTerrainDef(TerrainDef *terrainDef)
{
    if (terrainDef->getManager() != this) {
		O3D_ERROR(E_InvalidParameter("Terrain definition manager is not this"));
    } else {
        FastMutexLocker locker(m_mutex);

        // remove the terrain definition object from the manager.
        IT_FindMap it = m_findMap.find(terrainDef->getKeyName());
        if (it != m_findMap.end()) {
            terrainDef->setManager(nullptr);

            m_IDManager.releaseID(terrainDef->getId());
            terrainDef->setId(-1);

            O3D_MESSAGE("Delete terrain definition: " + terrainDef->getKeyName());

            deletePtr(terrainDef);

            m_findMap.erase(it);
        }
    }
}

//---------------------------------------------------------------------------------------
// class TerrainBase
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_ABSTRACT_CLASS1(TerrainBase, ENGINE_TERRAIN_BASE, SceneEntity)

/* Restricted */
TerrainBase::TerrainBase(const TerrainBase &dup) :
    SceneEntity(dup),
    m_pCamera(this,dup.m_pCamera),
    m_manager(nullptr)
{
}

TerrainBase::TerrainBase(BaseObject *parent, Camera *camera) :
	SceneEntity(parent),
	m_pCamera(this, camera),
    m_manager(nullptr)
{
}

TerrainBase & TerrainBase::operator = (const TerrainBase &dup)
{
    BaseObject::operator=(dup);

    m_pCamera = dup.m_pCamera;
    m_manager = nullptr;

    return *this;
}

// Define the viewer camera
void TerrainBase::setCamera(Camera *pCamera)
{
	m_pCamera = pCamera;
}

void TerrainBase::addLight(const LightInfos &)
{
}

void TerrainBase::removeLight(Light *)
{
}

void TerrainBase::updateLight(Light *)
{
}

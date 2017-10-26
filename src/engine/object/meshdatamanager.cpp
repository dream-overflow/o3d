/**
 * @file meshdatamanager.cpp
 * @brief Implementation of MeshDataManager.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2002-09-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/meshdatamanager.h"

#include "o3d/core/debug.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/virtualfilelisting.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(MeshDataManager, ENGINE_MESH_DATA_MANAGER, SceneResourceManager)

MeshDataManager::MeshDataManager(BaseObject *parent, const String &path) :
	SceneResourceManager(parent, path, "*.o3dms"),
	m_isAsynchronous(False)
{
	m_garbageManager.setLifeTime(10000);
}

// Virtual destructor.
MeshDataManager::~MeshDataManager()
{
	if (!m_findMap.empty())
	{
		String message("MeshData still exists into the manager :\n");

		for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it)
		{
            message += "       |- " + it->second->getResourceName() + "\n";
			deletePtr(it->second);
		}

		O3D_WARNING(message);
	}
}

// Delete child .
Bool MeshDataManager::deleteChild(BaseObject *child)
{
	if (child)
	{
		if (child->getParent() != this)
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
		else
		{
			// is it a material, and is it managed by this
			MeshData *meshData = o3d::dynamicCast<MeshData*>(child);
			if (meshData && (meshData->getManager() == this))
			{
				deleteMeshData(meshData);
				return True;
			}

			// otherwise simply delete the child
			deletePtr(child);
			return True;
		}
	}
	return False;
}

// update all dynamic materials
void MeshDataManager::update()
{
	FastMutexLocker locker(m_mutex);

	// process deferred objects deletion
	m_garbageManager.update();
}

// Insert an existing mesh data in the manager.
void MeshDataManager::addMeshData(MeshData *meshData)
{
	if (meshData->getManager() != NULL)
		O3D_ERROR(E_InvalidOperation("The given mesh data already have a manager"));

	// resource name must be defined
	if (meshData->getResourceName().isEmpty())
		O3D_ERROR(E_InvalidPrecondition("Resource name must be defined"));

	addResource(meshData->getResourceName());

	FastMutexLocker locker(m_mutex);

	// define the file name if necessary.
	if (meshData->getFileName().isEmpty())
		meshData->setFileName(m_path + '/' + meshData->getResourceName());

	// search for the mesh data
    IT_FindMap it = m_findMap.find(meshData->getResourceName());
	if (it != m_findMap.end())
		O3D_ERROR(E_InvalidParameter("A same mesh data with the same name already exists"));
	else
        m_findMap.insert(std::make_pair(meshData->getResourceName(), meshData));

    O3D_MESSAGE("Add mesh data \"" + meshData->getResourceName() + "\"");

	// this is the manager and its parent
	meshData->setManager(this);
	meshData->setParent(this);
	meshData->setId(m_IDManager.getID());
}

// Remove an existing mesh data from the manager.
void MeshDataManager::removeMeshData(MeshData *meshData)
{
	if (meshData->getManager() != this)
		O3D_ERROR(E_InvalidParameter("Mesh data manager is not this"));

	FastMutexLocker locker(m_mutex);

	// remove the mesh data object from the manager.
    IT_FindMap it = m_findMap.find(meshData->getResourceName());
	if (it != m_findMap.end())
	{
		meshData->setManager(NULL);
		meshData->setParent(getScene());

		m_IDManager.releaseID(meshData->getId());
		meshData->setId(-1);

		m_findMap.erase(it);

        O3D_MESSAGE("Remove (not delete) existing mesh data: " + meshData->getResourceName());
	}
}

// Delete an existing mesh data from the manager.
void MeshDataManager::deleteMeshData(MeshData *meshData)
{
	if (meshData->getManager() != this)
		O3D_ERROR(E_InvalidParameter("Mesh data manager is not this"));

	FastMutexLocker locker(m_mutex);

	// remove the texture object from the manager.
    IT_FindMap it = m_findMap.find(meshData->getResourceName());
	if (it != m_findMap.end())
	{
        m_garbageManager.add(meshData->getResourceName(), meshData);

		meshData->setManager(NULL);

		m_IDManager.releaseID(meshData->getId());
		meshData->setId(-1);

		m_findMap.erase(it);

        O3D_MESSAGE("Delete (to GC) mesh data: " + meshData->getResourceName());
	}
}

// Purge immediately the garbage manager of its content.
void MeshDataManager::purgeGarbage()
{
	FastMutexLocker locker(m_mutex);
	m_garbageManager.destroy();
}

MeshData* MeshDataManager::findMeshData(
		UInt32 type,
        const String &resourceName)
{
	FastMutexLocker locker(m_mutex);

	// search the key name into the map, next the mesh data given parameters into the element
    CIT_FindMap cit = m_findMap.find(resourceName);
	if (cit != m_findMap.end())
	{
		// found it ?
		return cit->second;
	}

	// maybe the asked mesh data was just deleted, so search it into the garbage collector
	MeshData *meshData = NULL;
    m_garbageManager.remove(resourceName, meshData);

	locker.unlock();

	// if found, reinsert it into the manager
	if (meshData)
		addMeshData(meshData);

	return meshData;
}

// load a mesh-data file (.o3dms) into the mesh-data manager
MeshData* MeshDataManager::addMeshData(const String &filename)
{
    MeshData *meshData = findMeshData(0, filename);
	if (meshData)
		return meshData;

	// create and load the new mesh-data
	meshData = new MeshData(this);
    meshData->setResourceName(filename);

    O3D_MESSAGE("Import mesh data file \"" + meshData->getResourceName() + "\"");
    String absFilename = getFullFileName(meshData->getResourceName());

	// asynchronous loading
	if (m_isAsynchronous)
	{
        MeshDataTask *task = new MeshDataTask(meshData, absFilename);

		TaskManager::instance()->addTask(task);
	}
	// synchronous loading
    else if (!meshData->load(absFilename))
	{
		deletePtr(meshData);
        O3D_ERROR(E_InvalidFormat("Invalid mesh data file " + absFilename));
	}

	meshData->setFileName(absFilename);
	addMeshData(meshData);
	
	return meshData;
}

// Is a mesh data exists.
Bool MeshDataManager::isMeshData(const String &resourceName)
{
    return (findMeshData(0, resourceName) != nullptr);
}

// Read a mesh-data object
MeshData* MeshDataManager::readMeshData(InStream &is)
{
	MeshData *meshData = new MeshData(this);
    is >> *meshData;

    MeshData *found = findMeshData(0, meshData->getResourceName());
	if (found)
	{
		deletePtr(meshData);
		return found;
	}

    O3D_MESSAGE("Import mesh data file \"" + meshData->getResourceName() + "\"");
    String absFilename = getFullFileName(meshData->getResourceName());

	// asynchronous loading
	if (m_isAsynchronous)
	{
        MeshDataTask *task = new MeshDataTask(meshData, absFilename);

		TaskManager::instance()->addTask(task);
	}
	// synchronous loading
	else
	{
		if (!meshData->load(absFilename))
		{
			deletePtr(meshData);
            O3D_ERROR(E_InvalidFormat("Invalid mesh data file \"" + absFilename + "\""));
		}
		else
			meshData->createGeometry();
	}

	meshData->setFileName(absFilename);
	addMeshData(meshData);

	return meshData;
}

// count the total number of vertices of the manager
UInt32 MeshDataManager::countTotalNumVerts()
{
	FastMutexLocker locker(m_mutex);
	UInt32 n = 0;

	for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it)
	{
		n += it->second->getNumVertices();
	}

    return n;
}

// count the total number of faces of the manager
UInt32 MeshDataManager::countTotalNumFaces()
{
	FastMutexLocker locker(m_mutex);
	UInt32 n = 0;

	for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it)
	{
		n += it->second->getNumFaces();
	}

    return n;
}

// Data export
Int32 MeshDataManager::exportMeshData()
{
	FastMutexLocker locker(m_mutex);
	Int32 count = 0;

	String absFileName, resourceName;

	for (IT_FindMap it = m_findMap.begin(); it != m_findMap.end(); ++it)
	{
		MeshData* meshData = it->second;

		// if the file name is not defined
		if (meshData->getFileName().isEmpty())
		{
			absFileName = getFullFileName(meshData->getResourceName());
			meshData->setFileName(absFileName);
		}

		if (meshData->save())
			count++;
	}

    return count;
}

// Enable an asynchronous texture query manager
void MeshDataManager::enableAsynchronous()
{
	FastMutexLocker locker(m_mutex);

	if (!m_isAsynchronous)
		m_isAsynchronous = True;
}

// Disable an asynchronous texture query manager
void MeshDataManager::disableAsynchronous()
{
	FastMutexLocker locker(m_mutex);

	if (m_isAsynchronous)
		m_isAsynchronous = False;
}

// Is asynchronous mesh-data loading enabled.
Bool MeshDataManager::isAsynchronous() const
{
	m_mutex.lock();
	Bool result = m_isAsynchronous;
	m_mutex.unlock();

	return result;
}


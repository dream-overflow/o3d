/**
 * @file meshdatamanager.h
 * @brief Manager of O3DMeshData
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-09-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MESHDATAMANAGER_H
#define _O3D_MESHDATAMANAGER_H

#include "../scene/sceneentity.h"
#include "o3d/core/mutex.h"
#include "o3d/core/stringlist.h"
#include "o3d/core/garbagemanager.h"
#include "o3d/core/idmanager.h"
#include "o3d/core/memorydbg.h"
#include "meshdata.h"

#include "o3d/core/taskmanager.h"

#include <map>

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class MeshDataManager
//-------------------------------------------------------------------------------------
//! Manager of MeshData
//---------------------------------------------------------------------------------------
class O3D_API MeshDataManager : public SceneResourceManager
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(MeshDataManager)

	//! Default constructor.
	MeshDataManager(BaseObject *parent, const String &path = "");

	//! Virtual destructor.
	virtual ~MeshDataManager();

	virtual Bool deleteChild(BaseObject *child);

	//---------------------------------------------------------------------------------------
	// Factory
	//---------------------------------------------------------------------------------------

	//! Insert an existing mesh data in the manager.
	//! @note Once the mesh data is inserted into the manager, it become its parent.
	//! @param meshData A named mesh data object.
	//! @param path The logical (registered) path where the mesh data is located if saved.
	void addMeshData(MeshData *meshData);

	//! Remove an existing mesh data from the manager.
	//! The mesh data is not pushed in the garbage manager, it is simply removed of this manager.
	void removeMeshData(MeshData *meshData);

	//! Delete an existing mesh data from the manager. It is stored temporarily in the
	//! garbage manager, before to be really deleted if the mesh data is not reused
	//! before a defined time.
	void deleteMeshData(MeshData *meshData);

	//! Purge immediately the garbage manager of its content.
	void purgeGarbage();

	//! Creates/returns a mesh data from a file (.o3dms).
    //! @param filename The resource containing the mesh.
	//! @return A pointer on the asked mesh data.
	//! @note If a similar mesh data exists it is returned and shared.
	MeshData* addMeshData(const String &filename);

	//! Read a mesh data object from a file a returns it.
    MeshData* readMeshData(InStream &is);

	//! Is a mesh data exists.
    //! @param resourceName Resource name to search for.
    Bool isMeshData(const String &resourceName);

	//---------------------------------------------------------------------------------------
	// Global
	//---------------------------------------------------------------------------------------

	//! count the total number of vertices of the manager
    UInt32 countTotalNumVerts();

	//! count the total number of faces of the manager
    UInt32 countTotalNumFaces();

	//-----------------------------------------------------------------------------------
	// MeshData query
	//-----------------------------------------------------------------------------------

	//! Enable an asynchronous loading.
	void enableAsynchronous();

	//! Disable an asynchronous loading.
	void disableAsynchronous();

	//! Is asynchronous mesh-data loading enabled.
	Bool isAsynchronous() const;

	//! Update the manager
	void update();

	//! export all mesh-data in separates files
	Int32 exportMeshData();

private:

	typedef std::map<String, MeshData*> T_FindMap;
	typedef T_FindMap::iterator IT_FindMap;
	typedef T_FindMap::const_iterator CIT_FindMap;

	T_FindMap m_findMap;

	//! Manage removed mesh data objects.
	GarbageManager<String, MeshData*> m_garbageManager;

	IDManager m_IDManager;

    MeshData* findMeshData(UInt32 type, const String &resourceName);

	Bool m_isAsynchronous;
};

} // namespace o3d

#endif // _O3D_MESHDATAMANAGER_H


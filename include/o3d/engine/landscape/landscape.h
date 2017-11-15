/**
 * @file landscape.h
 * @brief Landscape global manager.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_LANDSCAPE_H
#define _O3D_LANDSCAPE_H

#include "../scene/sceneentity.h"
#include "o3d/core/idmanager.h"
#include "o3d/core/hashmap.h"
#include "o3d/core/smartpointer.h"
#include "o3d/engine/object/camera.h"

namespace o3d {

class TerrainBase;
class Light;

//---------------------------------------------------------------------------------------
//! @class TerrainManager
//-------------------------------------------------------------------------------------
//! Terrain manager.
//---------------------------------------------------------------------------------------
class O3D_API TerrainManager : public SceneEntity, NonCopyable<>
{
public:

	O3D_DECLARE_CLASS(TerrainManager)

	//! Default constructor.
	TerrainManager(BaseObject *parent);

	//! Virtual destructor.
	virtual ~TerrainManager();

	//! Delete a child object.
	Bool deleteChild(BaseObject *child);

	//! Add a terrain object.
	void addTerrain(TerrainBase *terrain);
	
	//! Remove (not delete) a terrain object.
	//! @note The parent become the scene.
	void removeTerrain(TerrainBase *terrain);

	//! Delete a terrain object.
	void deleteTerrain(TerrainBase *terrain);

	//! Check if a terrain belong to this manager.
	Bool isTerrain(const String &name);

	//---------------------------------------------------------------------------------------
	// Processing
	//---------------------------------------------------------------------------------------

	//! Draw all managed terrains
	void draw();

	//! Update all managed terrains
	void update();

private:

	FastMutex m_mutex;

	typedef std::map<String, TerrainBase*> T_FindMap;
	typedef T_FindMap::iterator IT_FindMap;
	typedef T_FindMap::const_iterator CIT_FindMap;

	T_FindMap m_findMap;

	IDManager m_IDManager;

	TerrainBase* findTerrain(UInt32 type, const String &keyName);
};

//---------------------------------------------------------------------------------------
//! @class TerrainDef
//-------------------------------------------------------------------------------------
//! Terrain (resource) definition.
//---------------------------------------------------------------------------------------
class TerrainDef : public SceneResource, NonCopyable<>
{
public:

	//! Default constructor.
	TerrainDef(BaseObject *parent) : SceneResource(parent) {}

	//! Set the key name used by the manager.
	inline void setKeyName(const String &keyName) { m_keyName = keyName; }
	//! Get the key name used by the manager.
	inline const String& getKeyName() const { return m_keyName; }

protected:

	String m_keyName;       //!< Unique terrain definition key string.
};

//---------------------------------------------------------------------------------------
//! @class TerrainDefManager
//-------------------------------------------------------------------------------------
//! Terrain resource manager.
//---------------------------------------------------------------------------------------
class O3D_API TerrainDefManager : public SceneResourceManager, NonCopyable<>
{
public:

	O3D_DECLARE_CLASS(TerrainDefManager)

	//! Default constructor.
	TerrainDefManager(BaseObject *parent, const String &path = "");

	//! Virtual destructor.
	virtual ~TerrainDefManager();

	virtual Bool deleteChild(BaseObject *child);

	//! Insert an existing terrain definition in the manager.
	//! @note Once the terrain definition is inserted into the manager, it become its parent.
	void addTerrainDef(TerrainDef *terrainDef);

	//! Remove an existing terrain definition from the manager.
	//! The terrain definition is not pushed in the garbage manager, it is simply removed of this manager.
	void removeTerrainDef(TerrainDef *terrainef);

	//! Delete an existing terrain definition from the manager. It is stored temporarily in the
	//! garbage manager, before to be really deleted if the terrain definition is not reused
	//! before a defined time.
	void deleteTerrainDef(TerrainDef *terrainDef);

	//! Create/Return a terrain definition given a file name.
	//! @param filename Terrain definition filename to load.
	//! @return A new or an existing terrain definition object.
	//! @note The manager becomes the parent of the terrain definition.
	TerrainDef* addTerrainDef(const String &filename);

	//! Is a terrain definition exists.
	//! @param filename Terrain definition filename to search for.
	//! @return TRUE of a terrain definition is found.
	Bool isTerrainDef(const String &filename);

private:

	IDManager m_IDManager;

	typedef std::map<String, TerrainDef*> T_FindMap;

	typedef T_FindMap::iterator IT_FindMap;
	typedef T_FindMap::const_iterator CIT_FindMap;

	T_FindMap m_findMap;

	TerrainDef* findTerrainDef(const String &keyName);
};

//---------------------------------------------------------------------------------------
//! @class Landscape
//-------------------------------------------------------------------------------------
//! Landscape manager.
//---------------------------------------------------------------------------------------
class O3D_API Landscape : public SceneEntity, NonCopyable<>
{
public:

	O3D_DECLARE_CLASS(Landscape)

	//! Default constructor
	Landscape(BaseObject *parent, const String &path = "");

	//! Destructor
	virtual ~Landscape();

	// Get the terrain manager.
	TerrainManager& getTerrainManager() { return m_terrainManager; }

	// Get the terrain manager (const version).
	const TerrainManager& getTerrainManager() const { return m_terrainManager; }

	// Get the terrain resource manager.
	TerrainDefManager& getTerrainDefManager() { return m_terrainDefManager; }

	// Get the terrain resource manager (const version).
	const TerrainDefManager& getTerrainDefManager() const { return m_terrainDefManager; }

	//---------------------------------------------------------------------------------------
	// Processing
	//---------------------------------------------------------------------------------------

	//! Draw all managed terrains
	void draw();

	//! Update all managed terrains
	void update();

private:

	TerrainManager m_terrainManager;
	TerrainDefManager m_terrainDefManager;
};

class Camera;

//---------------------------------------------------------------------------------------
//! @class TerrainBase
//-------------------------------------------------------------------------------------
//! Generic terrain object.
//---------------------------------------------------------------------------------------
class O3D_API TerrainBase : public SceneEntity
{
public:

	friend class TerrainManager;

	O3D_DECLARE_ABSTRACT_CLASS(TerrainBase)

	//! Define the type of light. The terrain engine is free to take account of this
	//! information.
	enum LightType
	{
		LIGHT_TYPE_STATIC = 0,   //!< A light whose position is not likely to change
		LIGHT_TYPE_DYNAMIC		 //!< A light whose position will change very often (approx each frame)
	};

	//! Define what kind of lightning you want
	enum LightPolicy
	{
		LIGHT_POLICY_PER_VERTEX = 0,	
		LIGHT_POLICY_PER_PIXEL
	};

	enum LightUpdate
	{
		LIGHT_UPDATE_AUTO = 0,
		LIGHT_UPDATE_MANUAL
	};

	struct LightInfos
	{
		LightType type;
		LightPolicy policy;
		LightUpdate update;

		Light * pLight;
	};

	//! Default constructor
    TerrainBase(BaseObject *pParent, Camera *pCamera = nullptr);

	//! Destructor
	virtual ~TerrainBase() {}

	//---------------------------------------------------------------------------------------
	// Parameters
	//---------------------------------------------------------------------------------------

	//! Define the viewer camera
	void setCamera(Camera *pCamera); 

	//! Get the viewer camera
    inline       Camera* getCamera()       { return m_pCamera.get(); }
	//! Get the viewer camera (const version)
    inline const Camera* getCamera() const { return m_pCamera.get(); }

	//! Add a light 
	virtual void addLight(const LightInfos &);
	virtual void removeLight(Light *);
    virtual void updateLight(Light * = nullptr);

	//---------------------------------------------------------------------------------------
	// Processing
	//---------------------------------------------------------------------------------------

	virtual void draw() = 0;
	virtual void update() = 0;

protected:

	SmartObject<Camera> m_pCamera;

	TerrainManager *m_manager;

	/* Restricted */
    TerrainBase(const TerrainBase &dup);
    TerrainBase & operator = (const TerrainBase &dup);
};

} // namespace o3d

#endif // _O3D_LANDSCAPE_H


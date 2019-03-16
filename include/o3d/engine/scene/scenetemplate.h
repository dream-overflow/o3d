/**
 * @file scenetemplate.h
 * @brief world.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-04-18
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCENETEMPLATE_H
#define _O3D_SCENETEMPLATE_H

#include "../animation/animationplayermanager.h"
#include "../hierarchy/hierarchytree.h"

#include "o3d/core/memorydbg.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

#include <map>

namespace o3d {

class Node;
class SceneObject;
class SceneTemplate;

typedef std::map<String,SceneObject*> T_StringToObjectMap;
typedef T_StringToObjectMap::iterator IT_StringToObjectMap;
typedef T_StringToObjectMap::const_iterator CIT_StringToObjectMap;

//---------------------------------------------------------------------------------------
//! @class SceneTplInstance
//-------------------------------------------------------------------------------------
//! Scene template instance.
//---------------------------------------------------------------------------------------
class O3D_API SceneTplInstance
{
	friend class SceneTemplate;

public:

	//! set/get autodestroy
	inline void setAutoDestroy(Bool set) { m_autodelete = set; }
	inline Bool getAutoDestroy()const { return m_autodelete; }

	//! dtor
    virtual ~SceneTplInstance();

	//! get the instance name
	inline const String& getName()const { return m_name; }

	//! is the instance successfully created
	inline Bool isValid()const { return m_valid; }

	//! get an object by its name
	inline SceneObject* getObject(const String &name)
	{
        if (!m_valid) return nullptr;
		IT_StringToObjectMap it = m_loadedObject.find(name);
		if (it != m_loadedObject.end())
			return it->second;
        return nullptr;
	}
	inline const SceneObject* getObject(const String &name)const
	{
        if (!m_valid) return nullptr;
		CIT_StringToObjectMap cit = m_loadedObject.find(name);
		if (cit != m_loadedObject.end())
			return cit->second;
        return nullptr;
	}

	//! get an animation player by its name
	inline AnimationPlayer* getAnimationPlayer(UInt32 id)
	{
        if (!m_valid) return nullptr;

		if (id >= m_loadedPlayer.size())
            return nullptr;

		return m_loadedPlayer[id];
	}
	inline const AnimationPlayer* getAnimationPlayer(UInt32 id)const
	{
        if (!m_valid) return nullptr;

		if (id >= m_loadedPlayer.size())
            return nullptr;

		return m_loadedPlayer[id];
	}

	//! destroy the instance and all linked objects
	void destroy();

protected:

    Scene *m_pScene;                    //!< the scene where the objects are instancied

    String m_name;                      //!< the name of the instance
    Bool m_valid;                       //!< is the scene loaded properly
    Bool m_autodelete;                  //!< auto delete objects when this is destructed

    T_StringToObjectMap m_loadedObject; //!< number and list of created objects
    T_PlayerVector m_loadedPlayer;      //!< number and list of created animation players

	T_SceneObjectList m_rootObjects;

	// restricted ctors
	SceneTplInstance(Scene *pScene) :
		m_pScene(pScene),
		m_valid(False),
		m_autodelete(True)
	{}

	SceneTplInstance(const SceneTplInstance& dup) {}
};


//---------------------------------------------------------------------------------------
//! @class SceneTemplate
//---------------------------------------------------------------------------------------
//! Scene template loading. Used for managed a list of objets mostly created in the
//! world.
//---------------------------------------------------------------------------------------
class O3D_API SceneTemplate
{
public:

    /**
     * @brief SceneTemplate
     * @param filename Resource template file name. The content is duplicated in a memory
     *                 stream.
     * @param name Template name
     */
	SceneTemplate(const String &filename, const String &name);
    /**
     * @brief SceneTemplate
     * @param is Valid stream to be duplicate in memory stream.
     * @param name Template name
     */
    SceneTemplate(InStream &is, const String &name);

	//! dtor
	virtual ~SceneTemplate();

	//! create a new instance of the template
    SceneTplInstance* create(Scene *scene, const String &specifyName = "");

	//! get the template name
	inline const String& getName()const { return m_tplname; }

	//! is the template is valid
	inline Bool isValid()const { return m_isvalid; }

	//! get the filename
    inline String getFileName()const { return m_filename;  }

private:

	String m_tplname;
	Bool m_isvalid;

    String m_filename;
    InStream *m_is;
};

typedef std::map<String,SceneTemplate*> T_SceneTemplateMap;
typedef T_SceneTemplateMap::iterator O3D_IT_SceneTemplateMap;
typedef T_SceneTemplateMap::const_iterator O3D_CIT_SceneTemplateMap;


//---------------------------------------------------------------------------------------
//! @class SceneTplMgr
//-------------------------------------------------------------------------------------
//! Scene template manager.
//---------------------------------------------------------------------------------------
class O3D_API SceneTplMgr
{
public:

	//! Singleton instance
    static SceneTplMgr* instance();

    //! Delete the singleton instance
    static void destroy();

	//! Add a template
	Bool add(const String &filename, const String &name);

	//! Create a new instance of the template for a specific scene
	//! The instance name can be specified with 'specifyName' otherwise.
	//! Otherwise, template name is used for the instance name
	SceneTplInstance* create(
		Scene *pScene,
		const String &name,
		const String &specifyName = "");

	//! Find a template by its name
	Bool find(const String &name);

	//! Find a template by its filename
	Bool findByFileName(const String &filename);

	//! Delete a template by its name
	void revove(const String &name);

	//! Delete a template by its filename
	void removeByFileName(const String &filename);

protected:

	T_SceneTemplateMap m_templateMap;

    static SceneTplMgr* m_instance;

    SceneTplMgr();
    virtual ~SceneTplMgr();
    SceneTplMgr(const SceneTplMgr& dup);
    void operator=(const SceneTplMgr& dup);
};

} // namespace o3d

#endif // _O3D_SCENETEMPLATE_H

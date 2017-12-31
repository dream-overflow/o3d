/**
 * @file sceneobjectmanager.h
 * @brief Scene object mapping.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-12-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCENEOBJECTMANAGER_H
#define _O3D_SCENEOBJECTMANAGER_H

#include "o3d/core/idmanager.h"
#include "o3d/core/memorydbg.h"
#include "sceneobject.h"
#include "sceneentity.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SceneObjectManager
//-------------------------------------------------------------------------------------
//! Scene object manager and mapping.
//! This manager is mainly used as a reference table and auto-clean unused object.
//---------------------------------------------------------------------------------------
class O3D_API SceneObjectManager : public SceneEntity
{
public:

	friend class SceneObject;

	O3D_DECLARE_DYNAMIC_CLASS(SceneObjectManager)

	//! Default constructor.
	SceneObjectManager(BaseObject *parent);

	//! Virtual destructor. Object map is deleted but object are not.
	virtual ~SceneObjectManager();

	//! Add an existing element and attribute to him a new id
	//! @return the new element key id.
	//! @note Parent stay unchanged
	Int32 addElement(SceneObject *element)
	{
        O3D_ASSERT(element != nullptr);
		O3D_ASSERT(element->getId() == -1);

		if (!element)
			return -1;

		if (element->getId() != -1)
		{
			O3D_ERROR(E_InvalidParameter("Given element has already an unique identifier"));
		}

	    Int32 newID = m_IDManager.getID();

		m_idMap.insert(std::pair<UInt32,SceneObject*>(newID,element));

		element->setId(newID);

		return newID;
	}

	//! Remove an existing element of the manager
	void removeElement(SceneObject *element)
	{
		IT_IdMap it = m_idMap.find(element->getId());

		if (it != m_idMap.end())
		{
			O3D_ASSERT(element == it->second);

			m_IDManager.releaseID(element->getId());

			m_idMap.erase(it);
		}
	}

	//! Find an element by its identifier
	SceneObject* get(Int32 id)
	{
		IT_IdMap it = m_idMap.find(id);
		if (it != m_idMap.end())
			return it->second;
		else
            return nullptr;
	}

	//! Find an element by its identifier
	const SceneObject* get(Int32 id) const
	{
		CIT_IdMap it = m_idMap.find(id);
		if (it != m_idMap.end())
			return it->second;
		else
            return nullptr;
	}

	//! Find an element by its name
	SceneObject* searchName(const String &name)
	{
		for (IT_IdMap it = m_idMap.begin(); it != m_idMap.end(); ++it)
		{
			if (it->second->getName() == name)
				return it->second;
		}
        return nullptr;
	}

	//! Find an element by its name
	const SceneObject* searchName(const String &name) const
	{
		for (CIT_IdMap it = m_idMap.begin(); it != m_idMap.end(); ++it)
		{
			if (it->second->getName() == name)
				return it->second;
		}
        return nullptr;
	}

	//-----------------------------------------------------------------------------------
	// IO
	//-----------------------------------------------------------------------------------

	//! Define an imported scene object for a given serialize id.
	inline void setImportedSceneObject(UInt32 index, SceneObject *sceneObject)
	{
		if (index < m_indexToObjectSize)
			m_indexToObject[index] = sceneObject;
	}

	//! Return an imported scene object for a given serialize id.
	inline SceneObject* getImportedSceneObject(UInt32 index) const
	{
		return m_indexToObject[index];
	}

	//! Resize the list of scene object to import.
	void resizeImportedSceneObject(UInt32 size);

	//! Return the number of last imported scene object.
	inline UInt32 getNumImportedSceneObjects() const { return m_indexToObjectSize; }

	//! Reset the single id generator.
	inline void resetSingleId() { m_singleId = 0; }

	//! Return the next non-assigned unique id for serialization.
	inline UInt32 getSingleId()
	{
		++m_singleId;
		return m_singleId-1;
	}

protected:

	typedef stdext::hash_map<UInt32, SceneObject*> T_IdMap;
	typedef T_IdMap::iterator IT_IdMap;
	typedef T_IdMap::const_iterator CIT_IdMap;

	T_IdMap m_idMap;

	IDManager m_IDManager;

	UInt32 m_singleId;

	SceneObject **m_indexToObject; //!< used for import export
	UInt32 m_indexToObjectSize;
};

} // namespace o3d

#endif // _O3D_SCENEOBJECTMANAGER_H


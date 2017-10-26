/**
 * @file templatemanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEMPLATEMANAGER_H
#define _O3D_TEMPLATEMANAGER_H

#include "memorydbg.h"
#include "string.h"
#include "hashmapid.h"

namespace o3d {

/**
 * @brief Parent hashmap base object template manager.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2002-06-25
 * Map with name and id finding for objects. Use BaseObject based class.
 */
template<class T>
class O3D_API_TEMPLATE TemplateManager : public HashMapID<T>
{
public:

	//! Default constructor
	TemplateManager(BaseObject *parent) :
		HashMapID<T>(parent)
	{
	}

	//! Delete all elements owned by the manager.
	//! Shared objects are automatically detached from their users.
	virtual ~TemplateManager()
	{
		// delete each element in the list
		for (IT_TemplateManager it = HashMapID<T>::m_MyMap.begin(); it != HashMapID<T>::m_MyMap.end(); ++it)
		{
			T* pObj = (*it).second;
            O3D_ASSERT(pObj != nullptr);

			// delete it
			deletePtr(pObj);
		}

		HashMapID<T>::m_MyMap.clear();
	}

	//! Add an existing element and select it by default.
	//! @return The new element key id.
	Int32 addElement(T *newElement, Int32 forceKey = -1)
	{
        O3D_ASSERT(newElement != nullptr);

	    Int32 newID = -1;

		if (forceKey != -1)
		{
            Bool rep = HashMapID<T>::m_IDManager.forceID(forceKey);
            if(rep)
				newID = forceKey;
        }

		if (newID == -1)
			newID = HashMapID<T>::m_IDManager.getID();

		HashMapID<T>::m_MyMap[newID] = newElement;
	
		// parent it
		newElement->setId(newID);
		newElement->setParent(this);

		return newID;
	}

	//! Delete an element given its identifier.
	//! All users are detached from the identified object.
	//! @return False if the element is not found.
	Bool deleteElement(Int32 ID)
	{
		IT_TemplateManager it = HashMapID<T>::m_MyMap.find(ID);

		if (it != HashMapID<T>::m_MyMap.end())
		{
			T* pObj = HashMapID<T>::m_MyMap[ID];
            O3D_ASSERT(pObj != nullptr);

			// and delete it
			deletePtr(pObj);

			// release the id to the manager
			HashMapID<T>::m_IDManager.releaseID(ID);

			HashMapID<T>::m_MyMap.erase(it);

			return True;
		}
		return False;
	}

	//! Delete a child object
	virtual Bool deleteChild(BaseObject *pChild)
	{
		if (pChild && (pChild->getParent() == this))
		{
			m_deferredDeletetList.push_back((T*)pChild);
			return True;
		}
		else
		{
			deletePtr(pChild);
			return True;
		}
	}

	//! delete an element return False if not found
	//! All users are detached from the identified object.
	//! @return False if the element is not found.
	Bool deleteElementPtr(T* pObj)
	{
        O3D_ASSERT(pObj != nullptr);
		Int32 ID = pObj->getId();

		// we have the ID specified in the object, so no need to find him!
		if (ID != -1)
		{
			IT_TemplateManager it = HashMapID<T>::m_MyMap.find(ID);

			if ((it != HashMapID<T>::m_MyMap.end()) && (pObj == it->second))
			{
				// and delete it
				deletePtr(pObj);

				// release the id
				HashMapID<T>::m_IDManager.releaseID(ID);

				HashMapID<T>::m_MyMap.erase(it);

				return True;
			}
		}
		// we don't have the ID. Mistake... Maybe we can try to find him:
		else
		{
			for (IT_TemplateManager it = HashMapID<T>::m_MyMap.begin(); it != HashMapID<T>::m_MyMap.end(); ++it)
			{
				if ((*it).second == pObj)
				{
					// and delete it
					deletePtr(pObj);

					// release the id of the manager
					HashMapID<T>::m_IDManager.releaseID((*it).first);

					HashMapID<T>::m_MyMap.erase(it);

					return True;
				}
			}
		}
		// we can't find him
		return False;
	}

	//! Delete all elements owned by the manager.
	//! Shared objects are automatically detached from their users.
	void destroy()
	{
		for (IT_TemplateManager it = HashMapID<T>::m_MyMap.begin(); it != HashMapID<T>::m_MyMap.end(); ++it)
		{
			T* pObj = (*it).second;
            O3D_ASSERT(pObj != nullptr);

			// and delete it
			deletePtr(pObj);
		}

		HashMapID<T>::m_MyMap.clear();
		HashMapID<T>::m_IDManager.releaseAll();
	}

	//! Delete all unused objects
	//! More exactly, it delete all persistent objects that are no longer used
	void deleteUnusedElements()
	{
		IT_TemplateManager it = HashMapID<T>::m_MyMap.begin();

		while (it != HashMapID<T>::m_MyMap.end())
		{
			T* pObj = (*it).second;
            O3D_ASSERT(pObj != nullptr);

			// delete if this is only the manager whose use it
			if (pObj && pObj->noLongerUsed())
			{
				// and delete it
				deletePtr(pObj);

				HashMapID<T>::m_IDManager.releaseID((*it).first);

				IT_TemplateManager it2 = it++;
				HashMapID<T>::m_MyMap.erase(it2);
			}
			else
			{
				++it;
			}
		}
	}

	//! return object pointer by its name. return NULL if not found
	T* searchName(const String &name)
	{
		for (IT_TemplateManager it=HashMapID<T>::m_MyMap.begin(); it!=HashMapID<T>::m_MyMap.end(); ++it)
		{
			T* pObj = (*it).second;
            O3D_ASSERT(pObj != nullptr);

			if (pObj->getName() == name)
				return pObj;
		}
		return NULL;
	}

	//! make all name are single (add _X suffix if needed, where X mean a number).
	void makeSingleName()
	{
		// change similar name
		for (IT_TemplateManager it=HashMapID<T>::m_MyMap.begin() ; it!=HashMapID<T>::m_MyMap.end() ; ++it)
		{
			T* pElt = it->second;
            O3D_ASSERT(pElt != nullptr);

			// rename shader that have similar name with an _X extension
			UInt32 count = 0;
			for (IT_TemplateManager itbis=HashMapID<T>::m_MyMap.begin() ; itbis!=HashMapID<T>::m_MyMap.end() ; ++itbis)
			{
				T* pEltBis = itbis->second;
                O3D_ASSERT(pEltBis != nullptr);

				if ((pEltBis != pElt) && (pEltBis->getName() == pElt->getName()))
				{
					String newname(pElt->getName() + "_");
					newname.concat(count); count++;
					pEltBis->setName(newname);
				}
			}
		}
	}

	//! process the deferred object deletion list
	void processDeferred()
	{
		// delete each element of the list
		for (IT_DeleteList it = m_deferredDeletetList.begin(); it != m_deferredDeletetList.end(); ++it)
		{
			T *pObj = (*it);

			// check for the object validity
			// it is necessary because an object can be reused before it
			// is really removed. In this case simply ignore to delete it.
			if (pObj && pObj->noLongerUsed())
				deleteElementPtr(pObj);
		}

		// all entries are processed
		m_deferredDeletetList.clear();
	}

protected:

	typedef typename HashMapID<T>::IT_HashMapID   IT_TemplateManager;
	typedef typename HashMapID<T>::CIT_HashMapID  CIT_TemplateManager;

	typedef typename std::list<T*> T_DeleteList;
	typedef typename std::list<T*>::iterator IT_DeleteList;

	T_DeleteList m_deferredDeletetList;      //!< Deferred child deletion
};

} // namespace o3d

#endif // _O3D_TEMPLATEMANAGER_H


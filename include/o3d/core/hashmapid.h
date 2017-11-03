/**
 * @file hashmapid.h
 * @brief Integer identifier to template object hash map.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-04-20
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_HASHMAPID_H
#define _O3D_HASHMAPID_H

#include "idmanager.h"
#include "baseobject.h"
#include "hashmap.h"
#include "memorydbg.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class HashMapID
//-------------------------------------------------------------------------------------
//! Integer identifier to template object hash map.
//---------------------------------------------------------------------------------------
template<class T>
class O3D_API_TEMPLATE HashMapID : public BaseObject
{
public:

	typedef typename stdext::hash_map<Int32, T*> T_HashMapID;
	typedef typename stdext::hash_map<Int32,T*>::iterator IT_HashMapID;
	typedef typename stdext::hash_map<Int32,T*>::const_iterator CIT_HashMapID;

	//! Default constructor
	HashMapID(BaseObject *pParent) :
		BaseObject(pParent) {}

	//! destructor
	virtual ~HashMapID()
	{
		// delete each element in the list
		for (IT_HashMapID it = m_MyMap.begin(); it != m_MyMap.end(); ++it)
		{
			deletePtr((*it).second);
		}
	}

	//! add an element, return his key ID and select it
	Int32 addElement(T* pNewElement,Int32 force_key = -1)
	{
	    Int32 newID=-1;
		if (force_key!=-1)
		{
            Bool rep = m_IDManager.forceID(force_key);
            if (rep)
				newID=force_key;
        }
        if (newID == -1)
			newID=m_IDManager.getID();

		m_MyMap[newID] = pNewElement;

		return newID;
	}

	//! delete an element return False if error
	Bool deleteElement(Int32 ID)
	{
		IT_HashMapID it = m_MyMap.find(ID);

		if (it != m_MyMap.end())
		{
			T* pObj = m_MyMap[ID];
            deletePtr(pObj);

			m_IDManager.releaseID(ID);

			m_MyMap.erase(it);

			return True;
		}
		return False;
	}

	//! delete all element in the manager
	void destroy()
	{
		for (IT_HashMapID it = m_MyMap.begin(); it != m_MyMap.end(); ++it)
		{
			deletePtr((*it).second);
			m_IDManager.releaseID((*it).first);
		}

		m_MyMap.clear();
	}

	//! remove (not delete) an element in the manager.
	Bool removeElement(T* element)
	{
		for (IT_HashMapID it = m_MyMap.begin(); it != m_MyMap.end(); ++it)
		{
			if ((*it).second == element)
			{
				m_IDManager.releaseID((*it).first);

				m_MyMap.erase(it);
				return True;
			}
		}
		return False;
	}

	//! search an element by it's key id
	T* searchElement(Int32 ID)const
	{
		CIT_HashMapID it = m_MyMap.find(ID);
		if (it != m_MyMap.end())
			return (*it).second;

		return NULL;
	}

	const T* get(Int32 ID)const
	{
		CIT_HashMapID it = m_MyMap.find(ID);
		if (it != m_MyMap.end())
			return (*it).second;

		return NULL;
	}

	T* get(Int32 ID)
	{
		IT_HashMapID it = m_MyMap.find(ID);
		if (it != m_MyMap.end())
			return (*it).second;

		return NULL;
	}

	const T* getFirstElement()const
	{
		CIT_HashMapID it=m_MyMap.begin();
		if (it != m_MyMap.end()) return (*it).second;
		return NULL;
	}

	T* getFirstElement()
	{
		IT_HashMapID it=m_MyMap.begin();
		if (it != m_MyMap.end()) return (*it).second;
		return NULL;
	}

	//! does the manager contains an element
	Bool containsElement(T* element)
	{
		for (IT_HashMapID it=m_MyMap.begin(); it!=m_MyMap.end(); ++it)
		{
			if ((*it).second == element)
				return True;
		}
		return False;
	}

	//! get nbr element
	Int32 getNumElt() const { return (Int32)m_MyMap.size(); }

	//! get a begin const iterator
	IT_HashMapID begin() { return m_MyMap.begin(); }

	//! get a end const iterator
	IT_HashMapID end() { return m_MyMap.end(); }

	//! get a begin const iterator
	CIT_HashMapID begin() const { return m_MyMap.begin(); }

	//! get a end const iterator
	CIT_HashMapID end() const { return m_MyMap.end(); }

protected:

	T_HashMapID m_MyMap;
	IDManager m_IDManager;
};

} // namespace o3d

#endif // _O3D_HASHMAPID_H

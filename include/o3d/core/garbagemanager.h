/**
 * @file garbagemanager.h
 * @brief Class in charge of deleting unused objects.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-09-26
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GARBAGEMANAGER_H
#define _O3D_GARBAGEMANAGER_H

#include "o3d/core/base.h"
#include <map>


namespace o3d {

//---------------------------------------------------------------------------------------
//! @class GarbageManager
//-------------------------------------------------------------------------------------
//! Class in charge of deleting unused objects. This class avoid useless disk access.
//! You have to take CARE ABOUT ONE THING: OBJ MUST be a pointer. This class has all access
//! on the object, so they must not be shared with others managers.
//---------------------------------------------------------------------------------------
template <class KEY, class OBJ>
class O3D_API_TEMPLATE GarbageManager
{
	struct GarbageObject
	{
		OBJ object;           //!< Object stored.
		UInt32 lifetime;  //!< Time elapsed from the insertion into this garbage manager.
	};

	typedef std::multimap<KEY, GarbageObject> T_GarbageManagerMap;
	typedef typename std::multimap<KEY, GarbageObject>::iterator IT_GarbageManager;
	typedef typename std::multimap<KEY, GarbageObject>::const_iterator CIT_GarbageManager;

public:

	static const UInt32 GARBAGE_DEFAULT_LIFETIME = 1000;
	static const UInt32 GARBAGE_DEFAULT_MAXSIZE = 0;

	//! Default constructor.
	GarbageManager() :
		m_lifetime(GARBAGE_DEFAULT_LIFETIME),
		m_maxsize(GARBAGE_DEFAULT_MAXSIZE),
		m_container()
	{}

	//! Virtual destructor.
	virtual ~GarbageManager() { destroy(); }

	//! Refresh function.
	void update()
	{
		if (m_lifetime == 0)
			return;

        Int64 current_time = this->getCurrentTime();
		IT_GarbageManager it = m_container.begin();

		while (it != m_container.end())
			if (current_time - it->second.lifetime > m_lifetime)
			{
				deletePtr(it->second.object);

				IT_GarbageManager it2 = it++;
				m_container.erase(it2);
			}
			else it++;
	}

	//! The following function destroy all element of the garbage.
	void destroy()
	{
		for (IT_GarbageManager it = m_container.begin(); it != m_container.end(); it++)
		{
			deletePtr(it->second.object);
		}

		m_container.clear();
	}

	//! Find an object and return True if "_which" exists
    inline Bool exists(const KEY & _which)
	{
		IT_GarbageManager it = m_container.find(_which);

		return (it != m_container.end());
	}

	//! same function but return a pointer on the object (if found)
	inline Bool find(const KEY & _which, OBJ & _obj)
	{
		IT_GarbageManager it = m_container.find(_which);

		if (it != m_container.end())
		{
			_obj = it->second.object;
			it->second.lifetime = this->getCurrentTime();
			return True;
		}
		else return False;
	}

	//! Remove and delete an object
	inline Bool remove(const KEY & _which)
	{
		IT_GarbageManager it = m_container.find(_which);

		if (it != m_container.end())
		{
			deletePtr(it->second.object);
			m_container.erase(it);
			return True;
		}
		else return False;
	}

	//! Same function but return the object instead of destroying it
	inline Bool remove(const KEY & _which, OBJ & _obj)
	{
		IT_GarbageManager it = m_container.find(_which);

		if (it != m_container.end())
		{
			_obj = it->second.object;
			m_container.erase(it);
			return True;
		}
		else return False;
	}

	//! Add an object to the garbage manager
	Bool add(const KEY & _which, const OBJ & _object)
	{
        if (exists(_which))
			return False;

		if ((m_container.size() == m_maxsize) && (m_maxsize > 0))
		{
			IT_GarbageManager it;
			Bool ret = getOldestObject(it);
			O3D_ASSERT(ret);

			deletePtr(it->second.object);
			m_container.erase(it);
		}

		GarbageObject garbage = { _object, this->getCurrentTime() };

		m_container.insert(std::make_pair(_which, garbage));

		return True;
	}

	//! Parameters access
	inline void setLifeTime(UInt32 _time) { m_lifetime = _time; }
	inline UInt32 getLifeTime() const { return m_lifetime; }

	inline void setMaxSize(UInt32 _maxsize) { m_maxsize = _maxsize; }
	inline UInt32 getMaxSize() const { return m_maxsize; }

private:

	// Members
	UInt32 m_lifetime;		//!< default : 1000ms (0 means infinite time)
	UInt32 m_maxsize;		//!< default : 0 (means no max size)

	T_GarbageManagerMap m_container;

	//! Restricted constructor
	GarbageManager(const GarbageManager & _which);

	//! Get time in ms
	inline UInt32 getCurrentTime() const { return System::getMsTime(); }

	//! Get oldest object
	inline Bool getOldestObject(IT_GarbageManager & it_min)
	{
		if (m_container.size() == 0) return False;

		it_min = m_container.begin();

		for (IT_GarbageManager it = m_container.begin() ; it  != m_container.end() ; it++)
			if (it->second.lifetime < it_min->second.lifetime)
				it_min = it;

		return True;
	}
};

} // namespace o3d

#endif // _O3D_GARBAGEMANAGER_H


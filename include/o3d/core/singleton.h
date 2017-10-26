/**
 * @file singleton.h
 * @brief Template for singleton object.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2004-12-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SINGLETON_H
#define _O3D_SINGLETON_H

#include "memorydbg.h"
#include "base.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Singleton
//-------------------------------------------------------------------------------------
//! Singleton template object
//---------------------------------------------------------------------------------------
template <class T>
class O3D_API_TEMPLATE Singleton
{
public:

	//! destructor
    virtual ~Singleton<T>()
    {
        m_instance = nullptr;
    }

	//! get unique instance
    static T* instance()
	{
		if (!m_instance)
			m_instance = new T();

		return m_instance;
    }

	//! delete unique instance
    static void destroy()
	{
		if (m_instance)
            deletePtr(m_instance);
	}

protected:

	//! no copy possible
	Singleton<T>(const Singleton&) {}

	//! static instance
	static T* m_instance;

	//! default constructor
	Singleton<T>()
	{
		m_instance = (T*)this;		// Used to avoid recursive call when the ctor call himself...
	}
};

//! instance of the singleton
template <class T> T *Singleton<T>::m_instance = nullptr;

} // namespace o3d

#endif // _O3D_SINGLETON_H


/**
 * @file smartpointer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SMARTPOINTER_H
#define _O3D_SMARTPOINTER_H

#include "smartcounter.h"
#include "memorydbg.h"

namespace o3d {

/**
 * @brief Pointer manager with counter and auto-destruction
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-07-31
 * Pointer manager with counter and auto-destruction for SmartCounter inherited data.
 */
template<class T>
class O3D_API_TEMPLATE SmartPtr
{
public:

	//! default constructor
    SmartPtr() : m_pData(nullptr) {}

	//! copy constructor
	SmartPtr(const SmartPtr& dup)
	{
		m_pData = dup.m_pData;
		if (m_pData) m_pData->useIt();
	}

	//! Initialization constructor
	SmartPtr(T *pData)
	{
		m_pData = pData;
		if (m_pData) m_pData->useIt();
	}

	//! desctructor
	~SmartPtr() { releaseCheckAndDelete(); }

	//! check if the object is valid (not null)
    inline Bool isValid() const { return (m_pData != nullptr); }

	//! Return the object reference
	inline T& operator*()const { return *m_pData; }

	//! Return the object reference
	inline T* operator->()const { return m_pData; }

	//! Return the pointer
	inline T* get() const {	return m_pData; }

	//! Affectation
	inline SmartPtr& operator=(const SmartPtr& ptr)
	{
		// Check if it's the same data
		if (m_pData != ptr.m_pData)
		{
			// If not, release the old pointer
			releaseCheckAndDelete();

			// Assign the new pointer
			m_pData = ptr.m_pData;
			if (m_pData) m_pData->useIt();
		}
		return *this;
	}

	//! Affectation from an O3DSmartCounter pointer object
	inline SmartPtr& operator=(T *pData)
	{
		// Check if it's the same data
		if (m_pData != pData)
		{
			// If not, release the old pointer
			releaseCheckAndDelete();

			// Assign the new pointer
			m_pData = pData;
			if (m_pData) m_pData->useIt();
		}
		return  *this;
	}

	//! casting operator (read/write)
	inline operator T*() { return m_pData; }
	//! casting operator (read only)
	inline operator const T*() const { return m_pData; }

	//! Cast to boolean for check
	inline operator Bool() const { return isValid(); }

	//! Check if the object is totally released and then delete it
	//! @return True if the object is no longer used and deferred deletion.
	Bool releaseCheckAndDelete()
	{
        if (m_pData == nullptr)
			return False;

		m_pData->releaseIt();

		if (m_pData->canRemove())
		{
			m_pData->deleteIt();
			return True;
		}

        m_pData = nullptr;
		return False;
	}

	//! Compare two smart pointer, and return true if they have the same objects
	inline Bool operator==(const SmartPtr& cmp) const { return (m_pData == cmp.m_pData); }
	//! Compare two smart pointer, and return true if they have not the same objects
	inline Bool operator!=(const SmartPtr& cmp) const { return (m_pData != cmp.m_pData); }

private:

    T *m_pData;  //!< data pointer (must inherit from SmartCounter)
};


/**
 * @brief Simple pointer manager. Auto delete and assign.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-07-31
 * Simple pointer manager. Auto delete and assign.
 */
template<class T>
class O3D_API_TEMPLATE AutoPtr : NonCopyable<>
{
public:

	//! default constructor
    AutoPtr() : m_pData(nullptr) {}

	//! Initialization constructor
	AutoPtr(T *pData) : m_pData(pData) {}

	//! Destructor
	~AutoPtr() { release(); }

	//! check if the object is valid (not null)
    inline Bool isValid() const { return (m_pData != nullptr); }

    //! check if the object is valid (not null)
    inline Bool isNull() const { return (m_pData == nullptr); }

	//! Return the object reference
	inline T& operator*() const { return *m_pData; }

	//! Return the object reference
	inline T* operator->() const { return m_pData; }

	//! Return the pointer
	inline T* get() const {	return m_pData; }

	//! Affectation from a pointer object
	inline AutoPtr& operator=(T *pData)
	{
		// Check if it's the same data
		if (m_pData != pData)
		{
			// If not, release the old pointer
			release();

			// Assign the new pointer
			m_pData = pData;
		}
		return  *this;
	}

	//! casting operator (read/write)
	inline operator T*() { return m_pData; }
	//! casting operator (read only)
	inline operator const T*() const { return m_pData; }

	//! Cast to boolean for check
	inline operator Bool() const { return isValid(); }

	//! check if the object is totally released and then delete it
	inline void release()
	{
		deletePtr(m_pData);
	}

private:

	T *m_pData;  //!< data pointer
};

} // namespace o3d

#endif // _O3D_SMARTPOINTER_H


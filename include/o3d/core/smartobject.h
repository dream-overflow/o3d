/**
 * @file smartobject.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SMARTOBJECT_H
#define _O3D_SMARTOBJECT_H

#include "baseobject.h"
#include "memorydbg.h"

namespace o3d {

/**
 * @brief Shared part of a template smart object.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-04-03
 */
class O3D_API BaseSmartObject
{
public:

	//! Default constructor
    BaseSmartObject() :
        m_pData(nullptr),
        m_pUser(nullptr)
    {
    }

	//! Copy constructor
	BaseSmartObject(const BaseSmartObject &dup) :
        m_pData(dup.m_pData),
        m_pUser(dup.m_pUser)
    {
    }

	//! Assign operator
	BaseSmartObject& operator=(const BaseSmartObject &dup)
	{
		m_pData = dup.m_pData;
		m_pUser = dup.m_pUser;

		return *this;
	}

    void *m_pData;         //!< pointed data (base pointer).
    BaseObject *m_pUser;   //!< pointer user (must inherit from BaseObject).
};

/**
 * @brief Sharable object usage.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-04-03
 * Auto usage/release/deferred delete. The referer must be a BaseObject.
 */
template<class T>
class O3D_API_TEMPLATE SmartObject
{
public:

	//-----------------------------------------------------------------------------------
	// Constructions methods
	//-----------------------------------------------------------------------------------

	//! default constructor (neither user nor data)
	SmartObject() {}

	//! User constructor
	//! @param pUser the user object (must be non null)
	SmartObject(BaseObject *pUser)
	{
		O3D_ASSERT(pUser);

		// Initialize user object
        m_base.m_pUser = pUser;
	}

	//! Special copy constructor
	//! It duplicate the pointer and use the same user. This is useful for containers.
	//! Take care when using this copy constructor. Normally the unique usage is
	//! for container (ie std::list<SmartObject> and such others).
	SmartObject(const SmartObject &dup) :
        m_base(dup.m_base)
	{
		// And use it
        if (m_base.m_pData && m_base.m_pUser)
            getSharable()->useIt(m_base);

		// At this time there is two registrations for the same user. This is not
        // a great problem because the original SmartObject will be deleted quickly.
	}

	//! User and data constructor
	//! @param pUser the user object (must be non null)
	//! @param pData the data object to use (must be non null)
	SmartObject(BaseObject *pUser, T *pData)
	{
		O3D_ASSERT(pUser);

		// Initialize user and data objects
        m_base.m_pUser = pUser;
        m_base.m_pData = reinterpret_cast<void*>(pData);

		// Finally use it
        if (m_base.m_pData && m_base.m_pUser)
            getSharable()->useIt(m_base);
	}

	//! User and copy constructor
	//! @param pUser the user object (must be non null)
	//! @param dup the data object to use (take only the data from dup not the user)
	SmartObject(BaseObject *pUser, const SmartObject &dup)
	{
		O3D_ASSERT(pUser);

		// Initialize user and data objects
        m_base.m_pUser = pUser;
        m_base.m_pData = dup.m_base.m_pData;

		// Finally use it
        if (m_base.m_pData && m_base.m_pUser)
            getSharable()->useIt(m_base);
	}

	//! assign operator (a user must be defined)
	//! @param dup the data object to use (take only the data, not the user)
	SmartObject& operator=(const SmartObject &dup)
	{
        O3D_ASSERT(m_base.m_pUser);

		// Check if it is the same data
        if (m_base.m_pData != dup.m_base.m_pData)
		{
			// Release the old object
            if (m_base.m_pData && m_base.m_pUser)
			{
                if (getSharable()->releaseIt(m_base))
                    getSharable()->deleteIt();
			}

			// Assign the new object...
            m_base.m_pData = dup.m_base.m_pData;

			// ...and use it
            if (m_base.m_pData && m_base.m_pUser)
                getSharable()->useIt(m_base);
		}
		return  *this;
	}

	//! define the user (generally done in the user object constructor)
	inline void setUser(BaseObject *pUser)
	{
		O3D_ASSERT(pUser);
        m_base.m_pUser = pUser;
	}

	//! Define the user and the data objects
	inline void set(BaseObject *pUser, T *pData)
	{
		O3D_ASSERT(pUser);

		// return directly if it is the same user and data
        if ((m_base.m_pData == pData) && (m_base.m_pUser == pUser))
			return;

		// release the previous data if present
        if (m_base.m_pData && m_base.m_pUser)
		{
            if (getSharable()->releaseIt(m_base))
                getSharable()->deleteIt();
		}

		// set the new user and data
        m_base.m_pUser = pUser;
        m_base.m_pData = pData;

		// and use it
        if (m_base.m_pData && m_base.m_pUser)
            getSharable()->useIt(m_base);
	}

	//! use a given object (the user must be non null)
	//! @param pData The object to use
	inline SmartObject& operator=(T *pData)
	{
        O3D_ASSERT(m_base.m_pUser);

		// Check if it is the same data
        if (m_base.m_pData != pData)
		{
			// Release the old object
            if (m_base.m_pData && m_base.m_pUser)
			{
                if (getSharable()->releaseIt(m_base))
                    getSharable()->deleteIt();
			}

			// Assign the new object...
            m_base.m_pData = reinterpret_cast<void*>(pData);

			// ...and use it
            if (m_base.m_pData && m_base.m_pUser)
                getSharable()->useIt(m_base);
		}
        return *this;
	}

    //! Swap the object from this with the object of b.
    //! It avoid the cost of constructors. Only the data pointers
    //! are switched, not the users objects.
    //! This method takes care that the two objects are kept during the swap process.
    static inline void swap(SmartObject<T> &a, SmartObject<T> &b)
    {
        O3D_ASSERT(a.m_base.m_pUser);
        O3D_ASSERT(b.m_base.m_pUser);

        if (a.m_base.m_pData)
        {
            if (b.m_base.m_pUser)
                a.getSharable()->useIt(b.m_base);

            if (a.m_base.m_pUser)
                a.getSharable()->releaseIt(a.m_base);
        }

        if (b.m_base.m_pData)
        {
            if (a.m_base.m_pUser)
                b.getSharable()->useIt(a.m_base);

            if (b.m_base.m_pUser)
                b.getSharable()->releaseIt(b.m_base);
        }

        void *tmp = a.m_base.m_pData;
        a.m_base.m_pData = b.m_base.m_pData;
        b.m_base.m_pData = tmp;
    }


	//-----------------------------------------------------------------------------------
	// Destruction
	//-----------------------------------------------------------------------------------

	//! Destructor
    ~SmartObject()
    {
        releaseCheckAndDelete();
    }

	//! Check if the object is totally released and then delete it
	//! @return True if the object is gone for deletion.
	Bool releaseCheckAndDelete()
	{
        if (!m_base.m_pData || !m_base.m_pUser)
			return False;

        if (getSharable()->releaseIt(m_base))
		{
			// it is no longer used so we inform to its parent for a deletion
            getSharable()->deleteIt();
            m_base.m_pData = nullptr;
			return True;
		}

        m_base.m_pData = nullptr;
		return False;
	}


	//-----------------------------------------------------------------------------------
	// Accessors
	//-----------------------------------------------------------------------------------

    //! Cast base data pointer to Sharable*.
    inline Sharable* getSharable()
    {
        return static_cast<Sharable*>(reinterpret_cast<T*>(m_base.m_pData));
    }

    //! Cast base data pointer to Sharable* (const version).
    inline const Sharable* getSharable() const
    {
        return static_cast<const Sharable*>(reinterpret_cast<const T*>(m_base.m_pData));
    }

	//! Check if the object is valid (not null).
    inline Bool isValid() const { return (m_base.m_pData != nullptr); }

	//! Check if the user is valid (not null).
    inline Bool isValidUser() const { return (m_base.m_pUser != nullptr); }

	//! Get the user object.
    inline       BaseObject* getUser()       { return m_base.m_pUser; }
	//! Get the user object (read only).
    inline const BaseObject* getUser() const { return m_base.m_pUser; }

	//! Return the object reference.
    inline T& operator*() const { return *reinterpret_cast<T*>(m_base.m_pData); }

	//! Return the pointer.
    inline T* operator->() const { return reinterpret_cast<T*>(m_base.m_pData); }
	//! Return the pointer.
    inline T* get() const {	return reinterpret_cast<T*>(m_base.m_pData); }

	//! Casting operator (read/write).
	//inline operator T*() { return (T*)m_BaseSmartObject.m_pData; }
	//! Casting operator (read only).
	//inline operator const T*() const { return (T*)m_BaseSmartObject.m_pData; }

	//! Cast to boolean for check.
	inline operator Bool() const { return isValid(); }


	//-----------------------------------------------------------------------------------
	// Compare
	//-----------------------------------------------------------------------------------

	//! Compare two smart objects.
	//! @return true if they use the same object.
    inline Bool operator==(const SmartObject& cmp) const { return (m_base.m_pData == cmp.m_base.m_pData); }

	//! Compare two smart objects.
	//! @return true if they use not the same object.
    inline Bool operator!=(const SmartObject& cmp) const { return (m_base.m_pData != cmp.m_base.m_pData); }

	//! Lesser operator for containers.
	//! @return true if this data ptr is lesser than cmp data ptr.
    inline Bool operator<(const SmartObject& cmp) const { return (m_base.m_pData < cmp.m_base.m_pData); }

	//! Compare the smart object to a data pointer.
	//! @return true if they use the same object.
    inline Bool operator==(const T *pData) const { return (m_base.m_pData == pData); }

	//! Compare the smart object to a data pointer.
	//! @return true if they use not the same object.
    inline Bool operator!=(const T *pData) const { return (m_base.m_pData != pData); }

private:

    BaseSmartObject m_base;  //!< data linker
};

} // namespace o3d

#endif // _O3D_SMARTOBJECT_H


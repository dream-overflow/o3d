/**
 * @file smartcounter.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SMARTCOUNTER_H
#define _O3D_SMARTCOUNTER_H

#include "base.h"
#include "memorydbg.h"

namespace o3d {

/**
 * @brief A smart counter is used to known if an object is actually in use or not.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-09-23
 * A smart counter is used to known if an object is actually in use or not. UseIt add
 * an user, and release it, remove an user of the object. If the counter is equal to
 * zero the object can be deleted.
 */
template <class T>
class O3D_API_TEMPLATE SmartCounter
{
public:

	//! Default constructor.
	SmartCounter() : m_counter(0), m_id(-1) {}

	//! Copy constructor.
    SmartCounter(const SmartCounter&) :
		m_counter(0),
		m_id(-1) {}

	//! Assign.
    SmartCounter& operator= (const SmartCounter&)
	{
		m_counter = 0;
		m_id = -1;
		return *this;
	}

	//! Virtual destructor.
	virtual ~SmartCounter() {}

	//! use object/release object
	inline void useIt() { m_counter++; }
	inline void releaseIt() { m_counter--; O3D_ASSERT(m_counter >= 0); }

	//! get counter value
	inline Int32 getReferenceCounter() const { return m_counter; }

	//! return true if counter value is 0. If the object is totally released
	inline Bool noLongerUsed() const { return (m_counter == 0); }
	//! return true if it can be deleted
	inline Bool canRemove() const { return ((m_counter <= 1 && m_id != -1) || (m_counter == 0 && m_id == -1)); }
	//! is it owned by a manager
	inline Bool isOwned() const { return (m_id != -1); }

	//! Define the unique identifier (-1 mean no manager).
	inline void setId(Int32 id) { m_id = id; }
	//! Get the unique object identifier
	inline Int32 getId() const { return m_id; }

	//! Delete the object
	virtual Bool deleteIt()
	{
		deletePtr(const_cast<SmartCounter<T>*>(this));
		return True;
	}

	//! Release, next, check if the object is totally release,
	//! and finally delete it if it is no longer used.
	template<class U>
	static Bool checkAndDelete(SmartCounter<U> *pCounter)
	{
        if (pCounter == nullptr)
			return False;

		pCounter->releaseIt();

		if (pCounter->canRemove())
			return pCounter->deleteIt();

		return False;
	}

protected:

	Int32 m_counter;               //!< Usages counter
	Int32 m_id;                    //!< Object unique identifier
};

} // namespace o3d

#endif // _O3D_SMARTCOUNTER_H

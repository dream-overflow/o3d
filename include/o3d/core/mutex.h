/**
 * @file mutex.h
 * @brief Mutex synchronization objects and helpers.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-09-28
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MUTEX_H
#define _O3D_MUTEX_H

#include "base.h"
#include "memorydbg.h"

#if defined(O3D_POSIX_SYS)
	#include <pthread.h>
#endif

namespace o3d {

#ifdef O3D_WINAPI_SYS
	#include "architecture.h"
	typedef void* _Mutex;
	typedef CRITICAL_SECTION _FastMutex;
#elif defined(O3D_POSIX_SYS)
	typedef void* _Mutex;
	typedef pthread_mutex_t _FastMutex;
#else
	#error "<< Unknown architecture ! >>"
#endif // O3D_WINAPI_SYS

// locking return codes
#define O3D_ABANDONED  -1
#define O3D_LOCKED      0
#define O3D_WAIT_SIGNAL 0
#define O3D_TIMEOUT     1

/**
 * @brief Simple mutex with time out.
 */
class O3D_API Mutex : NonCopyable<>
{
public:

	//! Default constructor
	Mutex();

	//! destructor
	~Mutex();

	//! Lock the mutex using a timeout
	//! @param timeout in ms. If zero, the function tests the object's state and returns immediately.
	//! @return True if success.
	Bool lock(UInt32 timeout) const;

	//! Lock the mutex (infinite timeout).
	//! @param timeout in ms. If zero, the function tests the object's state and returns immediately.
	//! @return True if success.
	Bool lock() const;

	//! Try to lock the mutex (immediate return, no timeout).
	//! @param timeout in ms. If zero, the function tests the object's state and returns immediately.
	//! @return True if success.
	Bool tryLock() const;

	//! Unlock the mutex if previously locked
	Bool unlock() const;

private:

	_Mutex m_handle;  //!< mutex handle
};

/**
 * @brief A fast mutex. This kind of mutex is the faster and is non recursive.
 * @note On Windows it use CriticalSection and it is recursive.
 */
class O3D_API FastMutex : NonCopyable<>
{
	friend class WaitCondition;

public:

	//! Default constructor
	FastMutex();
	//! destructor
	~FastMutex();

	//! Lock the mutex.
	void lock() const;
	//! Unlock the mutex if previously locked.
	void unlock() const;

	//! Attempts to enter a critical section without blocking.
	//! @return True if successfully locked.
	Bool tryLock() const;

	//! Is the mutex locked. It try to enter a critical section, and
	//! if it cannot so it is locked. If it can it unlock just after.
	inline Bool isLocked() const
	{
        if (tryLock()) {
			unlock();
			return False;
		}
		return True;
	}

private:

	_FastMutex m_handle;  //!< mutex handle
};

/**
 * @brief A recursive mutex.
 * @note On Windows it use CriticalSection
 */
class O3D_API RecursiveMutex : NonCopyable<>
{
public:

	//! Default constructor
	RecursiveMutex();
	//! destructor
	~RecursiveMutex();

	//! Lock the mutex.
	void lock() const;
	//! Unlock the mutex if previously locked.
	void unlock() const;

	//! Attempts to enter a critical section without blocking.
	//! @return True if successfully locked.
	Bool tryLock() const;

	//! Is the mutex locked. It try to enter a critical section, and
	//! if it cannot so it is locked. If it can it unlock just after.
	inline Bool isLocked() const
	{
        if (tryLock()) {
			unlock();
			return False;
		}
		return True;
	}

private:

	_FastMutex m_handle;  //!< mutex handle
};

/**
 * @brief Object in charge of Locking and Unlocking an O3DMutex (useful with C++ exceptions)
 */
class O3D_API MutexLocker : NonCopyable<>
{
public:

	//! Constructor. Try to lock the mutex.
	MutexLocker(const Mutex & _mutex) :
		m_mutex(_mutex),
		m_acquired(False)
	{
		m_acquired = (_mutex.lock() == O3D_LOCKED);
	}

	//! Destructor. Unlock the mutex.
	~MutexLocker() { unlock(); }

	//! Release a locked mutex.
	Bool unlock() { return (isOk() && m_mutex.unlock() && ((m_acquired = False) == False)); }

	//! Relock the mutex
	void relock() { m_acquired = (m_mutex.lock() == O3D_LOCKED); }

	//! Return true if the mutex was acquired in the constructor.
	Bool isOk() const { return m_acquired; }

	//! Get the locker mutex
    const Mutex& getMutex() const { return m_mutex; }

private:

	const Mutex &m_mutex;
	Bool m_acquired;
};

/**
 * @brief Object in charge of Locking and Unlocking an FastMutex.
 */
class O3D_API FastMutexLocker : NonCopyable<>
{
public:

	//! Constructor. Lock the mutex.
	FastMutexLocker(const FastMutex & _mutex) :
		m_mutex(_mutex)
	{
        _mutex.lock();
	}

	//! Destructor. Unlock the mutex.
    ~FastMutexLocker() { unlock(); }

	//! Release a locked mutex
	void unlock() { m_mutex.unlock(); }

	//! Relock the mutex.
	void relock() { m_mutex.lock(); }

	//! Get the locked mutex.
    const FastMutex& getMutex() const { return m_mutex; }

private:

	const FastMutex &m_mutex;
};

/**
 * @brief Object in charge of Locking and Unlocking an RecursiveMutex.
 */
class O3D_API RecurMutexLocker : NonCopyable<>
{
public:

	//! Constructor. Lock the mutex.
	RecurMutexLocker(const RecursiveMutex & _mutex) :
		m_mutex(_mutex)
	{
		_mutex.lock();
	}

	//! Destructor. Unlock the mutex.
	~RecurMutexLocker() { unlock(); }

	//! Release a locked mutex
	void unlock() { m_mutex.unlock(); }

	//! Relock the mutex.
	void relock() { m_mutex.lock(); }

	//! Get the locked mutex.
    const RecursiveMutex& getMutex() const { return m_mutex; }

private:

	const RecursiveMutex &m_mutex;
};

/**
 * @class PMutex
 * Class to simplify the use of mutexed data using Mutex.
 */
template <class T>
class O3D_API_TEMPLATE PMutex
{
public:

	//! default constructor
	PMutex() : m_obj(new T()) {}

	//! init constructor. Take an already created object (will be destroyed to at destructor)
	PMutex(T* obj) : m_obj(obj) {}

	//! destructor
	~PMutex() { destroy(); }

	//! destroy the object
	void destroy() { m_mutex.lock(); deletePtr(m_obj); m_mutex.unlock(); }

	//! get if the object is valid (ptr not null)
	inline Bool isValid() const { if (m_obj) return True; else return False; }

	//! lock the object and return its object reference
	inline T& operator*() { m_mutex.lock(); return *m_obj; }

	//! lock the object and return its object reference
	inline const T& operator*() const { m_mutex.lock(); return *m_obj; }

	//! lock the object and return its object ptr
	inline T* operator->() { m_mutex.lock(); return m_obj; }

	//! lock the object and return its object ptr
	inline const T* operator->() const { m_mutex.lock(); return m_obj; }

	//! unlock the object after a * or -> operator
	inline void operator!() { m_mutex.unlock(); }

	//! return a copy of the object
	inline T operator()()
	{
		m_mutex.lock();

		if (m_obj)
		{
			T copy = T(*m_obj);
			m_mutex.unlock();
			return copy;
		}

		m_mutex.unlock();
		return T();
	}

	//! lock manually the object
	inline Int32 lock(UInt32 timeout = O3D_INFINITE) { return m_mutex.lock(timeout); }

	//! return the object without protection (/!\ take care with invalid result)
	//! You can use this method with manual lock/unlock processus methods
	inline       T* get()       { return m_obj; }
	inline const T* get() const { return m_obj; }

	//! unlock manually the object
	inline Bool unlock() { return m_mutex.unlock(); }

	//! change the value of the mutexed object
	inline void set(T &value)
	{
		m_mutex.lock();
		*m_obj = value;
		m_mutex.unlock();
	}

private:

	Mutex m_mutex;     //!< the mutex
	T* m_obj;          //!< the mutexed object
};

/**
 * @class PFastMutex
 * Class to simplify the use of mutexed data using FastMutex.
 */
template <class T>
class O3D_API_TEMPLATE PFastMutex
{
public:

	//! default constructor
	PFastMutex() : m_obj(new T()) {}

	//! init constructor. Take an already created object (will be destroyed to at destructor)
	PFastMutex(T* obj) : m_obj(obj) {}

	//! destructor
	~PFastMutex() { destroy(); }

	//! destroy the object
	void destroy() { m_mutex.lock(); deletePtr(m_obj); m_mutex.unlock(); }

	//! get if the object is valid (ptr not null)
	inline Bool isValid() const { if (m_obj) return True; else return False; }

	//! lock the object and return its object reference
	inline T& operator*() { m_mutex.lock(); return *m_obj; }

	//! lock the object and return its object reference
	inline const T& operator*() const { m_mutex.lock(); return *m_obj; }

	//! lock the object and return its object ptr
	inline T* operator->() { m_mutex.lock(); return m_obj; }

	//! lock the object and return its object ptr
	inline const T* operator->() const { m_mutex.lock(); return m_obj; }

	//! unlock the object after a * or -> operator
	inline void operator!() { m_mutex.unlock(); }

	//! return a copy of the object
	inline T operator()()
	{
		m_mutex.lock();

		if (m_obj)
		{
			T copy = T(*m_obj);
			m_mutex.unlock();
			return copy;
		}

		m_mutex.unlock();
		return T();
	}

	//! lock manually the object
	inline void lock() { m_mutex.lock(); }

	//! return the object without protection (/!\ take care with invalid result)
	//! You can use this method with manual lock/unlock processus methods
	inline       T* get()       { return m_obj; }
	inline const T* get() const { return m_obj; }

	//! unlock manually the object
	inline void unlock() { m_mutex.unlock(); }

	//! change the value of the mutexed object
	inline void set(T &value)
	{
		m_mutex.lock();
		*m_obj = value;
		m_mutex.unlock();
	}

private:

	FastMutex m_mutex;  //!< the mutex
	T* m_obj;           //!< the mutexed object
};

} // namespace o3d

#endif // _O3D_MUTEX_H

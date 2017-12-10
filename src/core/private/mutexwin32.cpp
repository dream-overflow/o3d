/**
 * @file mutexwin32.cpp
 * @brief Implementation of Mutex.h (WIN32 part)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author  Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-09-28
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/mutex.h"

// ONLY IF O3D_WIN32_SYS IS SELECTED
#ifdef O3D_WIN32_SYS

#include "o3d/core/debug.h"
#include "o3d/core/architecture.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// Mutex
//---------------------------------------------------------------------------------------

// create a new mutex
Mutex::Mutex() :
    m_handle(NULL)
{
    if ((m_handle = CreateMutexA(NULL,FALSE,NULL)) == NULL)
		O3D_ERROR(E_InvalidAllocation("Null mutex handle"));
}

// destructor
Mutex::~Mutex()
{
	if (m_handle)
		CloseHandle(m_handle);
}

// lock/unlock this mutex
Bool Mutex::lock(UInt32 timeout) const
{
	DWORD result = WaitForSingleObject(m_handle,timeout);

	switch (result)
	{
		case WAIT_OBJECT_0:
			return True;
		case WAIT_TIMEOUT:
			return False;
		default:
			return False;
	}
}

// lock/unlock this mutex
Bool Mutex::lock() const
{
	DWORD result = WaitForSingleObject(m_handle,O3D_INFINITE);

	switch (result)
	{
		case WAIT_OBJECT_0:
			return True;
		case WAIT_TIMEOUT:
			return False;
		default:
			return False;
	}
}

// lock/unlock this mutex
Bool Mutex::tryLock() const
{
	DWORD result = WaitForSingleObject(m_handle,0);

	switch (result)
	{
		case WAIT_OBJECT_0:
			return True;
		case WAIT_TIMEOUT:
			return False;
		default:
			return False;
	}
}

Bool Mutex::unlock() const
{
	return (ReleaseMutex(m_handle));
}


//---------------------------------------------------------------------------------------
// FastMutex
//---------------------------------------------------------------------------------------

// default constructor
FastMutex::FastMutex()
{
	InitializeCriticalSection(&m_handle);
}

// destructor
FastMutex::~FastMutex()
{
	DeleteCriticalSection(&m_handle);
}

// lock/unlock this mutex
void FastMutex::lock() const
{
    EnterCriticalSection(const_cast<LPCRITICAL_SECTION>(&m_handle));
}

void FastMutex::unlock() const
{
    LeaveCriticalSection(const_cast<LPCRITICAL_SECTION>(&m_handle));
}

// Attempts to enter a critical section without blocking
Bool FastMutex::tryLock() const
{
	return TryEnterCriticalSection(const_cast<LPCRITICAL_SECTION>(&m_handle));
}


//---------------------------------------------------------------------------------------
// RecursiveMutex
//---------------------------------------------------------------------------------------

// default constructor
RecursiveMutex::RecursiveMutex()
{
	InitializeCriticalSection(&m_handle);
}

// destructor
RecursiveMutex::~RecursiveMutex()
{
	DeleteCriticalSection(&m_handle);
}

// lock/unlock this mutex
void RecursiveMutex::lock() const
{
    EnterCriticalSection(const_cast<LPCRITICAL_SECTION>(&m_handle));
}

void RecursiveMutex::unlock() const
{
    LeaveCriticalSection(const_cast<LPCRITICAL_SECTION>(&m_handle));
}

// Attempts to enter a critical section without blocking
Bool RecursiveMutex::tryLock() const
{
	return TryEnterCriticalSection(const_cast<LPCRITICAL_SECTION>(&m_handle));
}

#endif // O3D_WIN32_SYS

/**
 * @file mutexposix.cpp
 * @brief Implementation of Mutex.h (POSIX part).
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-09-28
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/mutex.h"

// ONLY IF O3D_POSIX_SYS IS SELECTED
#ifdef O3D_POSIX_SYS

#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/time.h>
#include "o3d/core/debug.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// Mutex
//---------------------------------------------------------------------------------------

// create a new mutex
Mutex::Mutex() :
	m_handle(NULL)
{
#ifdef __APPLE__
	char name[16+2];
	sprintf(name, "/%.16llx", (long long unsigned int)this);

	m_handle = (void*) sem_open(name, O_CREAT, 0, 1);
#else
	m_handle = malloc(sizeof(sem_t));

	if (sem_init((sem_t*)m_handle,0,1) != 0)
	{
		free(m_handle);
		O3D_ERROR(E_InvalidAllocation("Null mutex handle"));
	}
#endif
}

// destructor
Mutex::~Mutex()
{
	if (m_handle)
	{
#ifdef __APPLE__
		sem_close((sem_t*)m_handle);
#else
		sem_destroy((sem_t*)m_handle);
#endif
	}
}

// lock/unlock this mutex
Bool Mutex::lock(UInt32 timeout) const
{
#ifdef __MACOSX__
	return (sem_wait((sem_t*)m_handle) == 0);
#else
	if (timeout == 0)
	{
		return (sem_trywait((sem_t*)m_handle) == 0);
	}
	else if (timeout == O3D_INFINITE)
	{
		return (sem_wait((sem_t*)m_handle) == 0);
	}
	else
	{
		struct timeval curTime;
                gettimeofday(&curTime, NULL);

		struct timespec absTime;
		absTime.tv_nsec = (curTime.tv_usec + (timeout % 1000) * 1000) * 1000;
		absTime.tv_sec = curTime.tv_sec + (timeout / 1000) + (absTime.tv_nsec / 1000000000);
		absTime.tv_nsec %= 1000000000;

		int result;

		tryagain:
		result = sem_timedwait((sem_t*)m_handle,&absTime);
		switch (result)
		{
			case EINTR:
				goto tryagain;
				break;
			case ETIMEDOUT:
				return False;
			case 0:
				return True;
			default:
				return False;
		}
	}
#endif
}

// lock/unlock this mutex
Bool Mutex::lock() const
{
	return (sem_wait((sem_t*)m_handle) == 0);
}

// lock/unlock this mutex
Bool Mutex::tryLock() const
{
	return (sem_trywait((sem_t*)m_handle) == 0);
}

Bool Mutex::unlock() const
{
	return (sem_post((sem_t*)m_handle) == 0);
}


//---------------------------------------------------------------------------------------
// FastMutex
//---------------------------------------------------------------------------------------

// default constructor
FastMutex::FastMutex()
{
	// use PTHREAD_MUTEX_DEFAULT
	if (pthread_mutex_init(&m_handle, NULL) != 0)
	{
		O3D_ERROR(E_InvalidAllocation("Unable to init the mutex"));
	}
}

// destructor
FastMutex::~FastMutex()
{
	pthread_mutex_destroy(&m_handle);
}

// lock/unlock this mutex
void FastMutex::lock() const
{
	pthread_mutex_lock(const_cast<pthread_mutex_t*>(&m_handle));
}

void FastMutex::unlock() const
{
	pthread_mutex_unlock(const_cast<pthread_mutex_t*>(&m_handle));
}

// Attempts to enter a critical section without blocking
Bool FastMutex::tryLock() const
{
	return (pthread_mutex_trylock(const_cast<pthread_mutex_t*>(&m_handle)) != EBUSY);
}


//---------------------------------------------------------------------------------------
// RecursiveMutex
//---------------------------------------------------------------------------------------

// default constructor
RecursiveMutex::RecursiveMutex()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

	if (pthread_mutex_init(&m_handle, &attr) != 0)
	{
		O3D_ERROR(E_InvalidAllocation("Unable to init the recursive mutex"));
	}

    pthread_mutexattr_destroy(&attr);
}

// destructor
RecursiveMutex::~RecursiveMutex()
{
	pthread_mutex_destroy(&m_handle);
}

// lock/unlock this mutex
void RecursiveMutex::lock() const
{
	pthread_mutex_lock(const_cast<pthread_mutex_t*>(&m_handle));
}

void RecursiveMutex::unlock() const
{
	pthread_mutex_unlock(const_cast<pthread_mutex_t*>(&m_handle));
}

// Attempts to enter a critical section without blocking
Bool RecursiveMutex::tryLock() const
{
	return (pthread_mutex_trylock(const_cast<pthread_mutex_t*>(&m_handle)) != EBUSY);
}


#endif // O3D_POSIX_SYS


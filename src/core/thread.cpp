/**
 * @file thread.cpp
 * @brief Implementation of Thread.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2005-02-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/thread.h"

#include "o3d/core/debug.h"

using namespace o3d;

Runnable::~Runnable()
{

}

//---------------------------------------------------------------------------------------
// ThreadManager
//---------------------------------------------------------------------------------------

Int32 ThreadManager::m_mCount=0;
FastMutex ThreadManager::m_mutexCount;
Bool ThreadManager::m_mExitCode;
FastMutex ThreadManager::m_mutexExitCode;

Bool ThreadManager::getExitCode()
{
	Bool temp = False;

	m_mutexExitCode.lock();
	temp = m_mExitCode;
	m_mutexExitCode.unlock();

	return temp;
}

void ThreadManager::setExitCode(Bool exitCode)
{
	m_mutexExitCode.lock();
	m_mExitCode = exitCode;
	m_mutexExitCode.unlock();
}

void ThreadManager::init()
{
    // initialize the main thread id
    m_mainThreadId = getThreadId();
}

void ThreadManager::addThread()
{
	m_mutexCount.lock();
	++m_mCount;
	m_mutexCount.unlock();
}

void ThreadManager::removeThread()
{
	m_mutexCount.lock();
	--m_mCount;
	m_mutexCount.unlock();
}

void ThreadManager::waitEndThreads()
{
	Bool end = False;

	while (!end)
	{
		m_mutexCount.lock();

		if (m_mCount == 0)
			end = True;

		m_mutexCount.unlock();

        System::waitMs(10);
	}
}


//---------------------------------------------------------------------------------------
// Thread
//---------------------------------------------------------------------------------------

// constructor
Thread::Thread(Runnable *runnable) :
	m_id(0),
	m_running(False),
    m_pData(nullptr),
	m_runnable(runnable),
#ifdef O3D_POSIX_SYS
	m_pThread(0),
#else
    m_pThread(nullptr),
#endif
	m_priority(PRIORITY_NORMAL),
	m_result(-1)
{
}

// Start the thread.
void Thread::start(void *data)
{
    if (!m_runnable) {
		O3D_ERROR(E_InvalidParameter("Runnable must be valid"));
    }

	create(new CallbackMethod<Runnable> (m_runnable, &Runnable::run), data);
}

Thread::ThreadPriority Thread::getPriority() const
{
	// avoid several GetPriority methods to be called by concurrent threads simultaneously
	m_mutex.lock();
	Thread::ThreadPriority priority = m_priority;
	m_mutex.unlock();
	return priority; 
}

Thread::~Thread()
{
}


//---------------------------------------------------------------------------------------
// Synchronize
//---------------------------------------------------------------------------------------

// Constructor
Synchronize::Synchronize():
	m_begin(0,1),
	m_end(0,1),
	m_waiting()
{
}

// Destructor
Synchronize::~Synchronize() noexcept(false)
{
    if (m_begin.getValue() != 0) {
		O3D_ERROR(E_InvalidResult("Critical : Synchronize object not released"));
    }

    if (m_end.getValue() != 0) {
		O3D_ERROR(E_InvalidResult("Critical : Synchronize object still waiting"));
    }
}

//  Wait for the main thread
Bool Synchronize::beginSync(UInt32 timeout)
{
    if (m_begin.waitSignal(timeout) == O3D_WAIT_SIGNAL) {
		return True;
    } else {
		return False;
    }
}

// Call if you want to give back the control to the main thread
void Synchronize::endSync()
{
	FastMutexLocker locker(m_waiting);

	m_end.postSignal();
}

// Call by the main thread to check if a thread is waiting.
Bool Synchronize::sync()
{
	FastMutexLocker locker(m_waiting);

    if (m_begin.postSignal()) {
		m_end.waitSignal();
		return True;
    } else {
		return False;
    }
}


//---------------------------------------------------------------------------------------
// ReadWriteLocker
//---------------------------------------------------------------------------------------

// Acquire the read lock
ReadWriteLocker::ReadWriteLocker(ReadWriteLock & _semaphore, LockerType _type) :
	m_semaphore(_semaphore),
	m_type(_type),
	m_locked(False)
{
	O3D_ASSERT((_type == WRITE_LOCKER) || (_type == READ_LOCKER));

	lock(m_type);
}

// Acquire the read lock
ReadWriteLocker::~ReadWriteLocker()
{
    if (isLocked()) {
		unlock();
    }
}

// Lock the semaphore
void ReadWriteLocker::lock(LockerType _type)
{
    if (isLocked()) {
		O3D_ERROR(E_InvalidOperation("Attempt to lock twice the same locker"));
    }

    if ((_type == WRITE_LOCKER) || (_type == READ_LOCKER)) {
		m_type = _type;
    }

    switch(Int32(m_type)) {
	case WRITE_LOCKER:
		m_semaphore.lockWrite();
		break;
	case READ_LOCKER:
		m_semaphore.lockRead();
		break;
	default:
		O3D_ASSERT(0);
		break;
	}

	m_locked = True;
}

// Unlock the semaphore
void ReadWriteLocker::unlock()
{
    if (!isLocked()) {
		O3D_ERROR(E_InvalidOperation("Attempt to unlock an unlocked locker"));
    }

	m_locked = False;

    switch(Int32(m_type)) {
        case WRITE_LOCKER:
            m_semaphore.unlockWrite();
            break;
        case READ_LOCKER:
            m_semaphore.unlockRead();
            break;
        default:
            O3D_ASSERT(0);
            break;
    }
}

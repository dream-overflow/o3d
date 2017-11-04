/**
 * @file threadwin32.cpp
 * @brief Implementation of Thread.h (WIN32 part)
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2005-02-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/thread.h"

// ONLY IF O3D_WIN32_SYS IS SELECTED
#ifdef O3D_WIN32_SYS

#include "o3d/core/debug.h"
#include "o3d/core/architecture.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
  initialize the main thread id
---------------------------------------------------------------------------------------*/
UInt32 ThreadManager::m_mainThreadId = ThreadManager::getThreadId();

/*---------------------------------------------------------------------------------------
  return current thread id
---------------------------------------------------------------------------------------*/
UInt32 ThreadManager::getThreadId()
{
	return UInt32(GetCurrentThreadId());
}

/*---------------------------------------------------------------------------------------
  running thread function
---------------------------------------------------------------------------------------*/
struct O3D_ThreadArgs
{
	Int32 *pResult;
    Bool *pRunning;
	Callback *pCallBack;
	void *pData;
	Semaphore semaphore;
};

static DWORD WINAPI O3D_RunThread(LPVOID data)
{
	O3D_ThreadArgs *args = (O3D_ThreadArgs*)data;

	Int32 *pResult;
	Callback *pCallBack;
	void *pData;
    Bool *pRunning;

	// count one more thread
	ThreadManager::addThread();

	pCallBack = args->pCallBack;
	pData = args->pData;
	pResult = args->pResult;
    pRunning = args->pRunning;

	// wake up the parent thread
	args->semaphore.postSignal();

	*pResult = pCallBack->call(pData);
    *pRunning = False;

	// count one less thread
	ThreadManager::removeThread();

	// delete the callback
	deletePtr(pCallBack);

	return 0;
}

// create a new thread with a mutex or none
void Thread::create(Callback* pFunc,void *pData)
{
	// avoid several Create methods to be called by concurrent threads simultaneously
	FastMutexLocker locker(m_mutex);

	DWORD id;

	O3D_ThreadArgs *args;

	args = new O3D_ThreadArgs;

	args->pData = pData;
	args->pCallBack = pFunc;
	args->pResult = &m_result;
    args->pRunning = &m_running;

	// create the new thread
	if ((m_pThread = CreateThread(NULL,0/*(SIZE_T)stackSize*/,O3D_RunThread,args,0,&id)) == NULL)
	{
		deletePtr(args);
		O3D_ERROR(E_InvalidAllocation("Null thread handle"));
	}

	setPriority(m_priority);

	m_id = id;
	m_pData = pData;

	// wait for the thread use argument
	args->semaphore.waitSignal();

	m_running = True;

    deletePtr(args);
}

// wait for the thread finish, and return the returned thread value
Int32 Thread::waitFinish()
{
	// avoid several WaitFinish methods to be called by concurrent threads simultaneously
	FastMutexLocker locker(m_mutex);

	// return immediately if not running
	if (!m_pThread)
		return -1;

	WaitForSingleObject(m_pThread,O3D_INFINITE);
	CloseHandle(m_pThread);

	m_id = 0;
    m_pThread = nullptr;
    m_pData = nullptr;
	m_running = False;

	return m_result;
}

// kill the thread
void Thread::kill()
{
	// avoid several Kill methods to be called by concurrent threads simultaneously
	FastMutexLocker locker(m_mutex);

	// return immediately if not running
	if (!m_pThread)
		return;

	TerminateThread(m_pThread,0);
	CloseHandle(m_pThread);

	// one less thread
	ThreadManager::removeThread();

	m_id = 0;
    m_pThread = nullptr;
    m_pData = nullptr;
	m_running = False;
}

// set the thread priority (not supported by SDL)
void Thread::setPriority(ThreadPriority _priority)
{
	// avoid several SetPriority methods to be called by concurrent threads simultaneously
	FastMutexLocker locker(m_mutex);

	m_priority = _priority;

	// If the thread is currently running, we change its priority immediately
	if (isThread())
	{
		Int32 prior = THREAD_PRIORITY_NORMAL;

		switch(_priority)
		{
		case PRIORITY_IDLE:
			prior = THREAD_PRIORITY_IDLE;
			break;
		case PRIORITY_LOWEST:
			prior = THREAD_PRIORITY_LOWEST;
			break;
		case PRIORITY_LOW:
			prior = THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case PRIORITY_NORMAL:
			prior = THREAD_PRIORITY_NORMAL;
			break;
		case PRIORITY_HIGH:
			prior = THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case PRIORITY_HIGHEST:
			prior = THREAD_PRIORITY_HIGHEST;
			break;
		case PRIORITY_TIMECRITICAL:
			prior = THREAD_PRIORITY_TIME_CRITICAL;
			break;
		};

		SetThreadPriority(m_pThread, prior);
	}
}

// Define the thread CPU affinity (not supported by SDL).
void Thread::setCPUAffinity(const std::list<UInt32> &cpuIds)
{
	DWORD cpuset = 0;

	for (std::list<UInt32>::const_iterator it = cpuIds.begin(); it != cpuIds.end(); ++it)
		cpuset |= 1 << (*it);

	m_cpuAffinity = cpuIds;

	int result = SetThreadAffinityMask(m_pThread, cpuset);
	if (result == 0)
		O3D_ERROR(E_InvalidOperation("Unable to define the CPU affinity for the thread"));
}

//---------------------------------------------------------------------------------------
// Semaphore
//---------------------------------------------------------------------------------------

// constructor
Semaphore::Semaphore(UInt32 initialValue, UInt32 maxValue) :
	m_value(0),
	m_handle(NULL)
{
	if ((m_handle = CreateSemaphoreW(NULL,initialValue,maxValue,NULL)) == NULL)
	{
		O3D_ERROR(E_InvalidAllocation("Null semaphore handle"));
	}

	m_value = initialValue;
}

// destroy the semaphore
Semaphore::~Semaphore()
{
	if (m_handle)
		CloseHandle(m_handle);
}

// lock the semaphore
Bool Semaphore::waitSignal(UInt32 timeout)
{
	DWORD result = WaitForSingleObject(m_handle,timeout);

	switch(result)
	{
	case WAIT_OBJECT_0:
		m_value--;
		return True;
		break;
	case WAIT_TIMEOUT:
		return False;
		break;
	default:
		return False;
		break;
	}
}

// Wait for a post signal
void Semaphore::waitSignal()
{
	DWORD result = WaitForSingleObject(m_handle,O3D_INFINITE);

	if (result == WAIT_OBJECT_0)
		m_value--;
}

// Try to Wait for a post signal
Bool Semaphore::tryWaitSignal()
{
	DWORD result = WaitForSingleObject(m_handle,0);

	switch(result)
	{
	case WAIT_OBJECT_0:
		m_value--;
		return True;
		break;
	default:
		return False;
		break;
	}
}

// unlock the semaphore
Bool Semaphore::postSignal()
{
	m_value++;

	if (!ReleaseSemaphore(m_handle,1,NULL))
	{
		m_value--;
		return False;
	}

	return True;
}

// get the semaphore value
UInt32 Semaphore::getValue() const
{
	return m_value;
}


//---------------------------------------------------------------------------------------
// WaitCondition
//---------------------------------------------------------------------------------------

// default constructor
WaitCondition::WaitCondition() :
	m_semWait(0),
	m_semDone(0),
	m_numWaiters(0),
	m_numSignals(0)
{
}

// destructor
WaitCondition::~WaitCondition()
{
}

// Releases the locked mutex and wait on the wait condition.
Bool WaitCondition::wait(FastMutex &mutex, UInt32 timeout)
{
	Bool result = True;

	// Obtain the protection mutex and increment the number of waiters.
	// This allows the signal mechanism to only perform a signal if there are waiting threads.
	m_mutex.lock();
	++m_numWaiters;
	m_mutex.unlock();

	// Unlock the mutex, as is required by condition variable semantics
	mutex.unlock();

	// Wait for a signal
	if (timeout == O3D_INFINITE)
		m_semWait.waitSignal();
	else
		result = m_semWait.waitSignal(timeout);

	// Let the signaler know we have completed the wait, otherwise the signaler can race 
	// ahead and get the condition semaphore if we are stopped between the mutex unlock and 
	// semaphore wait, giving a deadlock.
	m_mutex.lock();
	if (m_numSignals > 0)
	{
		// If we timed out, we need to eat a condition signal
		if (!result)
			m_semWait.waitSignal();

		// We always notify the signal thread that we are done
		m_semDone.postSignal();

		// Signal complete
		--m_numSignals;
	}
	--m_numWaiters;
	m_mutex.unlock();

	// Lock the mutex, as is required by condition variable semantics
	mutex.lock();

    return result;
}

// Releases the locked mutex and wait on the wait condition.
void WaitCondition::wait(FastMutex &mutex)
{
	wait(mutex, O3D_INFINITE);
}

// Wakes one thread waiting on the wait condition.
void WaitCondition::wakeOne()
{
    // If there are waiting threads not already signaled, then signal the condition
	// and wait for the thread to respond
	m_mutex.lock();
	if (m_numWaiters > m_numSignals)
	{
		++m_numSignals;
		m_semWait.postSignal();
		m_mutex.unlock();
		m_semDone.waitSignal();
	}
	else
	{
		m_mutex.unlock();
	}
}

// Wakes all threads waiting on the wait condition.
void WaitCondition::wakeAll()
{
    // If there are waiting threads not already signaled, then signal the condition
	// and wait for the thread to respond
	m_mutex.lock();
	if (m_numWaiters > m_numSignals)
	{
		UInt32 i, numWaiting;

		numWaiting = m_numWaiters - m_numSignals;
		m_numSignals = m_numWaiters;

		for (i = 0; i < numWaiting; ++i)
		{
			m_semWait.postSignal();
		}

		// Now all released threads are blocked here, waiting for us.
		m_mutex.unlock();

		for (i = 0; i < numWaiting; ++i)
		{
			m_semDone.waitSignal();
		}
	}
	else
	{
		m_mutex.unlock();
	}
}


//---------------------------------------------------------------------------------------
// ReadWriteLock
//---------------------------------------------------------------------------------------

ReadWriteLock::ReadWriteLock() :
	m_readerCounter(0)
{
	InitializeCriticalSection(&m_readerHandle);
	InitializeCriticalSection(&m_writerHandle);
}

// destructor
ReadWriteLock::~ReadWriteLock()
{
	DeleteCriticalSection(&m_readerHandle);
	DeleteCriticalSection(&m_writerHandle);
}

// Acquire the write lock
void ReadWriteLock::lockWrite()
{
	EnterCriticalSection(&m_writerHandle);
}

// Release the write lock
void ReadWriteLock::unlockWrite()
{
	LeaveCriticalSection(&m_writerHandle);
}

// Acquire the read lock
void ReadWriteLock::lockRead()
{
	EnterCriticalSection(&m_readerHandle);

	if (++m_readerCounter == 1)
		EnterCriticalSection(&m_writerHandle);

	LeaveCriticalSection(&m_readerHandle);
}

// Release the read lock
void ReadWriteLock::unlockRead()
{
	EnterCriticalSection(&m_readerHandle);

    if (--m_readerCounter == 0)
		LeaveCriticalSection(&m_writerHandle);

	LeaveCriticalSection(&m_readerHandle);
}

// Try to lock in read
Bool ReadWriteLock::tryLockRead()
{
	if (TryEnterCriticalSection(&m_readerHandle))
	{
	    if ((++m_readerCounter == 1) && !TryEnterCriticalSection(&m_writerHandle))
		{
			--m_readerCounter;
			LeaveCriticalSection(&m_readerHandle);
			return False;
		}

		LeaveCriticalSection(&m_readerHandle);
		return True;
	}
	else
		return False;
}

// Try to lock in write
Bool ReadWriteLock::tryLockWrite()
{
	return TryEnterCriticalSection(&m_writerHandle);
}

#endif // O3D_WIN32_SYS


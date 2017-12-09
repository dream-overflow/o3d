/**
 * @file threadposix.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/thread.h"

// ONLY IF O3D_POSIX_SYS IS SELECTED
#ifdef O3D_POSIX_SYS

#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h> 
#include <unistd.h>

#ifndef __APPLE__
#include <fcntl.h>
#endif

#include "o3d/core/debug.h"

using namespace o3d;

// List of signals to mask in the sub-threads
static int sigList[] = {
    SIGHUP, SIGINT, SIGQUIT, SIGPIPE, SIGALRM, SIGTERM,
	SIGCHLD, SIGWINCH, SIGVTALRM, SIGPROF, 0 };

// initialize the main thread id
UInt32 ThreadManager::m_mainThreadId = ThreadManager::getThreadId();

// return current thread id
UInt32 ThreadManager::getThreadId()
{
	return UInt32(pthread_self());
}

// running thread function
struct O3D_ThreadArgs
{
    Int32 *pResult;
    Bool *pRunning;
	Callback *pCallBack;
	void *pData;
	Semaphore semaphore;
	UInt32 *threadId;
};

static void setupThread()
{
	sigset_t mask;

	// Mask asynchronous signals for this thread
	sigemptyset(&mask);

    for (Int32 i = 0; sigList[i]; ++i) {
		sigaddset(&mask, sigList[i]);
	}
	pthread_sigmask(SIG_BLOCK, &mask, 0);

	// Allow ourselves to be asynchronously canceled
	int oldstate;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldstate);
}

static void* O3D_RunThread(void *data)
{
	setupThread();

	O3D_ThreadArgs *args = (O3D_ThreadArgs*)data;

	Int32 *pResult;
	Callback *pCallBack;
	void *pData;
    Bool *pRunning;

	// one more thread
	ThreadManager::addThread();

	pCallBack = args->pCallBack;
	pData = args->pData;
	pResult = args->pResult;
    pRunning = args->pRunning;
	*args->threadId = ThreadManager::getThreadId();

	// wake up the parent thread
	args->semaphore.postSignal();

    *pResult = pCallBack->call(pData);
    *pRunning = False;

	// one less thread
	ThreadManager::removeThread();

	// delete the callback
    deletePtr(pCallBack);

	pthread_exit(NULL);
    return nullptr;
}

// create a new thread
void Thread::create(Callback* pFunc, void *pData)
{
	// avoid several Create methods to be called by concurrent threads simultaneously
	FastMutexLocker locker(m_mutex);

	O3D_ThreadArgs *args;

	args = new O3D_ThreadArgs;

	args->pData = pData;
	args->pCallBack = pFunc;
	args->pResult = &m_result;
    args->pRunning = &m_running;
    args->threadId = &m_id;

	pthread_attr_t threadAttr;

	// Set the thread attributes
	if (pthread_attr_init(&threadAttr) != 0)
	{
		deletePtr(args);
		O3D_ERROR(E_InvalidResult("Couldn't initialize pthread attributes"));
	}

	pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
	//pthread_attr_setstacksize(&threadAttr, (size_t)stackSize);

	// Create the thread
	if (pthread_create(&m_pThread, &threadAttr, O3D_RunThread, args) != 0)
	{
		pthread_attr_destroy(&threadAttr);
		deletePtr(args);
		O3D_ERROR(E_InvalidResult("Not enough resources to create thread"));
	}

	pthread_attr_destroy(&threadAttr);

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

    if (!m_pThread) {
		return -1;
    }

	Int32 result = 0;

    pthread_join(m_pThread, nullptr);

	m_pThread = 0;
    m_pData = nullptr;
	m_running = False;

	return result;
}

// kill the thread
void Thread::kill()
{
	// avoid several Kill methods to be called by concurrent threads simultaneously
	FastMutexLocker locker(m_mutex);

	if (!m_pThread)
		return;

	pthread_cancel(m_pThread);//pthread_kill(&m_pThread, SIGKILL);
	pthread_join(m_pThread, NULL);

	// one less thread
	ThreadManager::removeThread();

	m_pThread = 0;
    m_pData = nullptr;
	m_running = False;
}

// set the thread priority (not supported by POSIX)
void Thread::setPriority(ThreadPriority _priority)
{
	//m_priority = PRIORITY_NORMAL;
	// not supported
}

// Define the thread CPU affinity (not supported by SDL).
void Thread::setCPUAffinity(const std::list<UInt32> &cpuIds)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);

	for (std::list<UInt32>::const_iterator it = cpuIds.begin(); it != cpuIds.end(); ++it)
		CPU_SET(*it, &cpuset);

	m_cpuAffinity = cpuIds;

	int result = pthread_setaffinity_np(m_pThread, sizeof(cpu_set_t), &cpuset);
	if (result != 0)
		O3D_ERROR(E_InvalidOperation("Unable to define the CPU affinity for the thread"));
}

//---------------------------------------------------------------------------------------
// Semaphore
//---------------------------------------------------------------------------------------

// constructor
Semaphore::Semaphore(UInt32 initialValue, UInt32 maxValue) :
	m_handle(NULL)
{
#ifdef __APPLE__
	char name[16+2];
	sprintf(name, "/%.16llx", (long long unsigned int)this);

	m_handle = (void*) sem_open(name, O_CREAT, 0, initialValue);
#else
	m_handle = malloc(sizeof(sem_t));

	if (sem_init((sem_t*)m_handle,0,initialValue) != 0)
	{
		free(m_handle);
		m_handle = NULL;

		O3D_ERROR(E_InvalidAllocation("Null semaphore handle"));
	}
#endif
}

// destroy the semaphore
Semaphore::~Semaphore()
{
	if (m_handle)
	{
#ifdef __APPLE__
		sem_close((sem_t*)m_handle);
#else
		sem_destroy((sem_t*)m_handle);
		free(m_handle);
#endif
	}
}

// lock the semaphore
Bool Semaphore::waitSignal(UInt32 timeout)
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

// Wait for a post signal
void Semaphore::waitSignal()
{
	sem_wait((sem_t*)m_handle);
}

// Try to Wait for a post signal
Bool Semaphore::tryWaitSignal()
{
	return (sem_trywait((sem_t*)m_handle) == 0);
}

// unlock the semaphore
Bool Semaphore::postSignal()
{
	if (sem_post((sem_t*)m_handle) != 0)
		return False;

	return True;
}

// get the semaphore value
UInt32 Semaphore::getValue() const
{
	Int32 value;
	sem_getvalue((sem_t*)m_handle,&value);

	return (value < 0 ? (UInt32)0 : (UInt32)value);
}


//---------------------------------------------------------------------------------------
// WaitCondition
//---------------------------------------------------------------------------------------

// default constructor
WaitCondition::WaitCondition()
{
	if (pthread_cond_init(&m_handle, NULL) != 0)
	{
		O3D_ERROR(E_InvalidAllocation("Unable to init the wait condition"));
	}
}

// destructor
WaitCondition::~WaitCondition()
{
	pthread_cond_destroy(&m_handle);
}

// Releases the locked mutex and wait on the wait condition.
Bool WaitCondition::wait(FastMutex &mutex, UInt32 timeout)
{
	struct timeval curTime;
	gettimeofday(&curTime, NULL);

	struct timespec absTime;
	absTime.tv_nsec = (curTime.tv_usec + (timeout % 1000) * 1000) * 1000;
	absTime.tv_sec = curTime.tv_sec + (timeout / 1000) + (absTime.tv_nsec / 1000000000);
	absTime.tv_nsec %= 1000000000;

	int result;

	tryagain:
	result = pthread_cond_timedwait(&m_handle, &mutex.m_handle, &absTime);
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

// Releases the locked mutex and wait on the wait condition.
void WaitCondition::wait(FastMutex &mutex)
{
	// @todo is spurious wakeup case to perform ?
	pthread_cond_wait(&m_handle, &mutex.m_handle);
}

// Wakes one thread waiting on the wait condition.
void WaitCondition::wakeOne()
{
	pthread_cond_signal(&m_handle);
}

// Wakes all threads waiting on the wait condition.
void WaitCondition::wakeAll()
{
	pthread_cond_broadcast(&m_handle);
}


//---------------------------------------------------------------------------------------
// ReadWriteLock
//---------------------------------------------------------------------------------------

ReadWriteLock::ReadWriteLock()
{
	// init the rwlock
    if (pthread_rwlock_init(&m_readerHandle, nullptr) != 0)	{
		O3D_ERROR(E_InvalidResult("Not enough resources to create rwlock"));
	}
}

// A destructor
ReadWriteLock::~ReadWriteLock()
{
	pthread_rwlock_destroy(&m_readerHandle);
}

// Acquire the write lock
void ReadWriteLock::lockWrite() const
{
    Int32 r = pthread_rwlock_wrlock(const_cast<pthread_rwlock_t*>(&m_readerHandle));
    O3D_ASSERT(r == 0);
}

// Release the write lock
void ReadWriteLock::unlockWrite() const
{
    Int32 r = pthread_rwlock_unlock(const_cast<pthread_rwlock_t*>(&m_readerHandle));
	O3D_ASSERT(r == 0);
}

// Acquire the read lock
void ReadWriteLock::lockRead() const
{
    Int32 r = pthread_rwlock_rdlock(const_cast<pthread_rwlock_t*>(&m_readerHandle));
	O3D_ASSERT(r == 0);
}

// Release the read lock
void ReadWriteLock::unlockRead() const
{
    Int32 r = pthread_rwlock_unlock(const_cast<pthread_rwlock_t*>(&m_readerHandle));
	O3D_ASSERT(r == 0);
}

// Try to lock in read
Bool ReadWriteLock::tryLockRead() const
{
    return (pthread_rwlock_tryrdlock(const_cast<pthread_rwlock_t*>(&m_readerHandle)) == 0);
}

// Try to lock in write
Bool ReadWriteLock::tryLockWrite() const
{
    return (pthread_rwlock_trywrlock(const_cast<pthread_rwlock_t*>(&m_readerHandle)) == 0);
}

#endif // O3D_POSIX_SYS

/**
 * @file thread.h
 * @brief Thread, thread manager, and semaphores.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @author  Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2005-02-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_THREAD_H
#define _O3D_THREAD_H

#include "mutex.h"
#include "string.h"
#include "memorydbg.h"
#include "callback.h"
#include "runnable.h"

#include <list>

namespace o3d {

#ifdef O3D_WIN32_SYS
	typedef void* _Thread;
	typedef void* _Semaphore;
	typedef void* _WaitCondition;
	typedef CRITICAL_SECTION _ReaderHandle;
	typedef CRITICAL_SECTION _WriterHandle;
#elif defined(O3D_POSIX_SYS)
	#include <pthread.h>
	typedef pthread_t _Thread;
	typedef void* _Semaphore;
	typedef pthread_cond_t _WaitCondition;
	typedef pthread_rwlock_t _ReaderHandle;
#else
	#error "<< Unknown architecture ! >>"
#endif // O3D_WIN32_SYS


//---------------------------------------------------------------------------------------
//! @class ThreadManager
//-------------------------------------------------------------------------------------
//! Manager of threads
//---------------------------------------------------------------------------------------
class O3D_API ThreadManager
{
public:

	static void addThread();
	static void removeThread();
	static Bool getExitCode();
	static void setExitCode(Bool exitCode);
	static void waitEndThreads();

	static UInt32 getMainThreadId() { return m_mainThreadId; }
	static UInt32 getThreadId();

private:

	static Int32 m_mCount;
	static FastMutex m_mutexCount;
	static Bool m_mExitCode;
	static FastMutex m_mutexExitCode;
	static UInt32 m_mainThreadId;
};


//---------------------------------------------------------------------------------------
//! @class Thread
//-------------------------------------------------------------------------------------
//! A thread object
//---------------------------------------------------------------------------------------
class O3D_API Thread : NonCopyable<>
{
public:

	//! the thread priority types
	enum ThreadPriority
	{
		PRIORITY_IDLE = 0,       //!< Idle the thread
		PRIORITY_LOWEST,         //!< The Lowest priority
		PRIORITY_LOW,            //!< Low than normal priority
		PRIORITY_NORMAL,         //!< normal (default) priority
		PRIORITY_HIGH,           //!< High than normal priority
		PRIORITY_HIGHEST,        //!< The Highest priority
		PRIORITY_TIMECRITICAL    //!< Real time priority
	};

	//! Default constructor
	//! @param runnable Object to run. Can be NULL be Start method will not works.
	//! @param data Optional data pointer.
	Thread(Runnable *runnable);

	//! Destructor. Let the thread running.
	~Thread();

	//! Start the thread.
	//! @param data An optional data pointer.
	void start(void *data = NULL);
	//! Start thread with a callback passed with a data.
	void create(Callback* callback, void *data = NULL);

	//! Kill the thread.
	void kill();

	//! Set the thread priority (not supported by SDL).
	void setPriority(ThreadPriority priority);
	//! Get the thread priority (not supported by SDL).
	ThreadPriority getPriority() const;

	//! Define the thread CPU affinity (not supported by SDL).
	//! @param cpuIds List defining CPU ids.
	void setCPUAffinity(const std::list<UInt32> &cpuIds);

	//! Get the list of CPU affinity (not support by SDL).
	//! @return A list containing CPU ids, empty means undefined.
	const std::list<UInt32>& getCPUAffinity() const { return m_cpuAffinity; }

	//! Wait for the thread finish, and return the returned thread value.
	Int32 waitFinish();
	//! Stop is a synonym for wait finish.
	inline Int32 stop() { return waitFinish(); }

	//! Get the thread ID.
	inline UInt32 getThreadID()const { return m_id; }
	//! Is the thread running.
	inline Bool isThread()const { return m_running; }

    //! Get runnable object.
    Runnable* getRunnable() { return m_runnable; }

private:

	UInt32 m_id;          //!< System thread identifier.
	Bool m_running;       //!< TRUE if thread is running.

	std::list<UInt32> m_cpuAffinity;  //!< CPU affinity, empty mean undefined.

	void *m_pData;            //!< Data associated with this thread (NULL if none).
	FastMutex m_mutex;

	Runnable *m_runnable;  //!< Current runnable.

	_Thread m_pThread;     //!< Thread handle.
	ThreadPriority m_priority;//!< Thread priority.

	Int32 m_result;       //!< Value returned by the thread.
};

//! list of threads
typedef std::list<Thread*> T_ThreadList;
typedef T_ThreadList::iterator IT_ThreadList;
typedef T_ThreadList::const_iterator CIT_ThreadList;

//---------------------------------------------------------------------------------------
//! @class Semaphore
//-------------------------------------------------------------------------------------
//! A semaphore is generally used to pause the execution of some threads (WaitSignal)
//! waiting for a signal (PostSignal). Wait signal decrement a counter, while post signal 
//! increment it. When the counter reach zero, all waiting thread are wake-up.
//---------------------------------------------------------------------------------------
class O3D_API Semaphore : NonCopyable<>
{
public:

	//! default constructor. Create the semaphore (max_value only for WIN32).
	Semaphore(UInt32 initialValue = 0, UInt32 maxValue = 32768);
	//! destructor. destroy the semaphore
	~Semaphore();

	//! Wait for a post signal. Suspends the calling thread until either the semaphore
	//! has a value greater than 0.
	//! @return true if success, false if timed-out
	Bool waitSignal(UInt32 timeout);

	//! Wait for a post signal with infinite timeout. Suspends the calling thread until
	//! either the semaphore has a value greater than 0.
	void waitSignal();

	//! Try to Wait for a post signal. Suspends the calling thread until either the semaphore
	//! has a value greater than 0.
	//! @return true if successfully immediately acquired
	Bool tryWaitSignal();

	//! Post a signal to the semaphore.
	//! @return true if success
	Bool postSignal();

	//! get the semaphore current value
	UInt32 getValue() const;

private:

#ifdef O3D_WIN32_SYS
	UInt32 volatile m_value;     //!< current value (only for win32)
#endif

	_Semaphore m_handle;        //!< the semaphore handle
};


//---------------------------------------------------------------------------------------
//! @class WaitCondition
//-------------------------------------------------------------------------------------
//! Provides a condition variable for synchronizing threads. 
//---------------------------------------------------------------------------------------
class O3D_API WaitCondition : NonCopyable<>
{
public:

	//! default constructor
	WaitCondition();

	//! destructor
	~WaitCondition();

	//! Releases the locked mutex and wait on the wait condition. The mutex must be
	//! initially locked by the calling thread. If mutex is not in a locked state, this
	//! function returns immediately.
	//! @return true if success, false if timed-out
	Bool wait(FastMutex &mutex, UInt32 timeout);

	//! Releases the locked mutex and wait on the wait condition. The mutex must be
	//! initially locked by the calling thread. If mutex is not in a locked state, this
	//! function returns immediately.
	void wait(FastMutex &mutex);

	//! Wakes one thread waiting on the wait condition.
	void wakeOne();

	//! Wakes all threads waiting on the wait condition.
	void wakeAll();

private:

#ifdef O3D_WIN32_SYS
	FastMutex m_mutex;
	Semaphore m_semWait;
	Semaphore m_semDone;
	UInt32 m_numWaiters;
	UInt32 m_numSignals;
#else
	_WaitCondition m_handle;
#endif
};


//---------------------------------------------------------------------------------------
//! @class Synchronize
//-------------------------------------------------------------------------------------
//! This object can be used if you want a thread to become the main thread temporarily.
//---------------------------------------------------------------------------------------
class O3D_API Synchronize : NonCopyable<>
{
public:

	//! constructor
	Synchronize();
	//! destructor
	~Synchronize();

	//! Wait for the main thread
	//! Return false if timeout
	Bool beginSync(UInt32 timeout = O3D_INFINITE);

	//! Call if you want to give back the control to the main thread
	void endSync();

	//! Call by the main thread to check if a thread is waiting.
	//! If no thread in waiting, this function return immediately.
	//! Return true if a thread was waiting.
	Bool sync();

private:

	Semaphore m_begin;
	Semaphore m_end;

	FastMutex m_waiting;
};


//---------------------------------------------------------------------------------------
//! @class ReadWriteLock
//-------------------------------------------------------------------------------------
//! This class is an implementation of the readers/writers. It's used to protect
//! shared data of multiple write-read accesses. Only one writer can access the shared
//! data at the same time. But the number of readers is unlimited.
//!
//! Note that this semaphore is not recursive. For example, you're not allowed to lock
//! in writing and to lock it in reading at the same time.
//---------------------------------------------------------------------------------------
class O3D_API ReadWriteLock : NonCopyable<>
{
public:

	//! Default constructor
	ReadWriteLock();
	//! A destructor
	~ReadWriteLock();

	//! Acquire the write lock
	void lockWrite();
	//! Release the write lock
	void unlockWrite();

	//! Acquire the read lock
	void lockRead();
	//! Release the read lock
	void unlockRead();

	//! Try to lock in read
	Bool tryLockRead();
	//! Try to lock in write
	Bool tryLockWrite();

private:

	// Members
	_ReaderHandle m_readerHandle;  //<! Critical section on windows, mutex on SDL, rw_lock on POSIX
#ifndef O3D_POSIX_SYS
	_WriterHandle m_writerHandle;  //<! Critical section on windows, condition variable
#endif

	Int32 m_readerCounter;  //<! Equal to the number of readers if positive, -1 means one writers.
};


//---------------------------------------------------------------------------------------
//! @class ReadWriteLocker
//-------------------------------------------------------------------------------------
//! This class is use to lock and release automatically a readers/writers, but
//! the current scope ended.
//---------------------------------------------------------------------------------------
class O3D_API ReadWriteLocker : NonCopyable<>
{
public:

	//! Defines the type of the locker
	enum LockerType
	{
		WRITE_LOCKER,
		READ_LOCKER
	};

	//! A constructor
	//! @param _semaphore the object you want to lock
	//! @param _type is the type of the locker.
	ReadWriteLocker(ReadWriteLock & _semaphore, LockerType _type);
	//! A destructor
	~ReadWriteLocker();

	//! @brief Lock the semaphore
	//! This function lock the semaphore and allows you to change the type of the locker
	//! @param _type is the type of locker you want. By default, the last type will be used
	//! @assertion E_InvalidOperation if the semaphore was already locked
	void lock(LockerType _type = LockerType(-1));

	//! @brief Unlock the semaphore
	//! This function unlocks the semaphore
	//! @assertion E_InvalidOperation if the semaphore was not locked
	void unlock();

	//! @brief Return the state of the semaphore
	//! @return True if the locker is locked, False otherwise
	Bool isLocked() const { return m_locked; }

private:
	
	// Members

	ReadWriteLock & m_semaphore;

	LockerType m_type;
	Bool m_locked;
};

} // namespace o3d

#endif // _O3D_THREAD_H


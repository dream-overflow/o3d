/**
 * @file timerstd.cpp
 * @brief simple timer with callback (SDL part)
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-03-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/timer.h"
#include "o3d/core/thread.h"
#include "o3d/core/application.h"

// ONLY IF O3D_STD_TIMER
#ifdef O3D_STD_TIMER

#include "o3d/core/debug.h"

using namespace o3d;

IDManager Timer::m_sigManager(1);

// constructor
Timer::Timer(
	UInt32 timeout,
	TimerMode mode,
	Callback *callback,
	void *data) :
		BaseObject(),
		m_type(NON_THREADED),
		m_timeout(timeout),
		m_mode(mode),
		m_pCallback(callback),
#ifdef O3D_POSIX_SYS
		m_handle(0),
#else
		m_handle(NULL),
#endif
		m_threadId(0),
		m_thread(this)
{
 	if (m_pCallback)
		create(m_timeout, m_mode, m_pCallback);
}

//---------------------------------------------------------------------------------------
// create the new timer
//---------------------------------------------------------------------------------------
Bool Timer::create(
		UInt32 timeout,
		TimerMode mode,
		Callback *callback,
		void *data)
{
	m_type = NON_THREADED;

	m_mode = mode;
	m_timeout = timeout;

	m_handle = m_sigManager.getID();
	m_counter.set(timeout);

	m_threadId = ThreadManager::getThreadId();

	if (m_handle < 0)
		O3D_ERROR(E_InvalidAllocation("Unable to create a non-threaded timer"));

	m_thread.start();

	return True;
}

//---------------------------------------------------------------------------------------
// delete the time (kill it)
//---------------------------------------------------------------------------------------
void Timer::destroy()
{
	killTimer();
     
	m_timeout = 0;
	m_threadId = 0;

	deletePtr(m_pCallback);
}

// Timer thread
Int32 Timer::run(void*)
{
	for (;;)
	{
		if (m_counter.isTimedOut())
		{
			Application::pushEvent(-m_handle, 0, this);

			m_counter.reset();
		}

		if (m_mode == TIMER_ONCE)
			break;

		// don't waste the CPU
        //System::waitMs(1);
	}

	return 0;
}

//---------------------------------------------------------------------------------------
// Throw a timer to process
//---------------------------------------------------------------------------------------
void Timer::throwTimer(UInt32 timeout)
{
	if (!m_handle)
		m_handle = m_sigManager.getID();

	m_timeout = timeout;
	m_counter.set(timeout);
}

//---------------------------------------------------------------------------------------
// Kill a timer in process
//---------------------------------------------------------------------------------------
void Timer::killTimer()
{
	if (m_thread.isThread())
	{
		TimerMode oldMode = m_mode;

		m_mode = Timer::TIMER_ONCE;
		m_counter.set(0);

		m_thread.stop();

		m_mode = oldMode;
	}

	if (m_handle)
	{
		m_counter.reset();
		m_sigManager.releaseID(m_handle);
		m_handle = 0;
	}
}

//---------------------------------------------------------------------------------------
// O3DTimerManager
//---------------------------------------------------------------------------------------

// Call a non-threaded timer
void TimerManager::callTimer(Timer *timer)
{
	O3D_ASSERT(timer);
	
	if (timer->getTimerMode() == Timer::TIMER_ONCE)
	{
		Int32 timeOut = timer->call();
		if (timeOut != (Int32)timer->getTimeout())
		{
			timer->m_counter.set(timeOut);
			timer->m_timeout = timeOut;

			// and restart it
			timer->m_thread.start();
		}
	}
	else
	{
		if (timer->call() == -1)
			timer->killTimer();
	}
}

#endif // O3D_STD_TIMER


/**
 * @file timerwin32.cpp
 * @brief simple timer with callback (WIN32 part)
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-08-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/timer.h"

// ONLY IF O3D_WIN32 IS SELECTED
#ifdef O3D_WIN32

#include "o3d/core/debug.h"
#include "o3d/core/architecture.h"

using namespace o3d;

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
        m_handle(0),
		m_threadId(0)
{
    if (m_pCallback) {
		create(m_timeout, m_mode, m_pCallback);
    }
}

// the windows time function
static void CALLBACK O3D_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (idEvent != 0) {
		Timer *pTimer = TimerManager::instance()->getTimerInternal(idEvent);

        if (pTimer) {
			Timer::TimerMode mode = pTimer->getTimerMode();
            if (mode == Timer::TIMER_ONCE) {
				Int32 timeOut = pTimer->call();
                if (timeOut != (Int32)pTimer->getTimeout()) {
					pTimer->killTimer();

                    if (timeOut > 0) {
						pTimer->throwTimer((UInt32)timeOut);
                    }
				}
            } else {
				if (pTimer->call() == -1)
					pTimer->killTimer();
			}
		}
	}
}

// create the new timer
Bool Timer::create(
		UInt32 timeout,
		TimerMode mode,
		Callback *pCallback,
		void *pData)
{
	m_type = THREADED;

	m_mode = mode;
	m_timeout = timeout;

	m_threadId = ThreadManager::getThreadId();

    if ((m_handle = (_Timer)SetTimer(NULL, 0, timeout, O3D_TimerProc)) == 0) {
		O3D_ERROR(E_InvalidAllocation("Unable to create a system timer"));
    }

	TimerManager::instance()->addTimerInternal(this);
	return True;
}

// delete the time (kill it)
void Timer::destroy()
{
    if (m_handle) {
        ::KillTimer(NULL, m_handle);
		TimerManager::instance()->removeTimerInternal(this);
		m_handle = 0;
	}

	m_timeout = 0;
	m_threadId = 0;

	deletePtr(m_pCallback);
}

// Throw a timer to process
void Timer::throwTimer(UInt32 timeout)
{
    if (m_handle) {
		return;
    }

	m_timeout = timeout;

    if ((m_handle = (_Timer)SetTimer(NULL, 0, m_timeout, O3D_TimerProc)) == 0) {
		O3D_ERROR(E_InvalidAllocation("Unable to create a system timer"));
    } else {
		TimerManager::instance()->addTimerInternal(this);
    }
}

// Kill a timer in process
void Timer::killTimer()
{
    if (m_handle) {
        ::KillTimer(NULL, m_handle);
		TimerManager::instance()->removeTimerInternal(this);
		m_handle = 0;
	}
}

#endif // O3D_WIN32

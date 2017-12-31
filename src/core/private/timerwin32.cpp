/**
 * @file timerwin32.cpp
 * @brief simple timer with callback (WIN32 part)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-08-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/timer.h"

// ONLY IF O3D_WINDOWS IS SELECTED
#ifdef O3D_WINDOWS

#include "o3d/core/debug.h"
#include "o3d/core/architecture.h"

using namespace o3d;

// the windows time function
static void CALLBACK O3D_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (idEvent != 0) {
        Timer *pTimer = TimerManager::instance()->getTimerByHandleInternal(idEvent);

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
    if (getId() < 0 && pCallback) {
        m_mode = mode;
        m_timeout = timeout;

        if (m_pCallback != pCallback) {
            deletePtr(m_pCallback);
            m_pCallback = pCallback;
        }

        if ((m_handle = (_Timer)SetTimer(nullptr, 0, timeout, O3D_TimerProc)) == 0) {
            O3D_ERROR(E_InvalidAllocation("Unable to create a system timer"));
        }

        TimerManager::instance()->addTimerInternal(this);
        return True;
    } else {
        // already running
        return False;
    }
}

// delete the time (kill it)
void Timer::destroy()
{
    if (m_handle) {
        ::KillTimer(nullptr, m_handle);
		TimerManager::instance()->removeTimerInternal(this);
		m_handle = 0;
	}

	m_timeout = 0;
	deletePtr(m_pCallback);
}

// Throw a timer to process
void Timer::throwTimer(UInt32 timeout)
{
    if (m_handle || !m_pCallback) {
		return;
    }

	m_timeout = timeout;

    if ((m_handle = (_Timer)SetTimer(nullptr, 0, m_timeout, O3D_TimerProc)) == 0) {
		O3D_ERROR(E_InvalidAllocation("Unable to create a system timer"));
    } else {
		TimerManager::instance()->addTimerInternal(this);
    }
}

// Kill a timer in process
void Timer::killTimer()
{
    if (m_handle) {
        ::KillTimer(nullptr, m_handle);
		TimerManager::instance()->removeTimerInternal(this);
		m_handle = 0;
	}
}

Timer* TimerManager::getTimerByHandleInternal(_Timer h)
{
    auto it = m_handlesMap.find(h);
    if (it != m_handlesMap.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

Int32 TimerManager::run(void*)
{
    return 0;
}

#endif // O3D_WINDOWS

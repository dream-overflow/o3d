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

#ifndef O3D_WIN32

#include "o3d/core/debug.h"

using namespace o3d;

Bool Timer::create(UInt32 timeout, TimerMode mode, Callback *callback, void *data)
{
    if (getId() < 0 && callback) {
        m_mode = mode;
        m_timeout = timeout;

        if (m_pCallback != callback) {
            deletePtr(m_pCallback);
            m_pCallback = callback;
        }

        m_counter.set(m_timeout);
        m_counter.start();

        TimerManager::instance()->addTimerInternal(this);
        return True;
    } else {
        // already running
        return False;
    }
}

void Timer::destroy()
{
	killTimer();
     
	m_timeout = 0;

	deletePtr(m_pCallback);
}

void Timer::throwTimer(UInt32 timeout)
{
    if (getId() < 0 && m_pCallback) {
        m_timeout = timeout;

        m_counter.set(m_timeout);
        m_counter.start();

        TimerManager::instance()->addTimerInternal(this);
    }
}

void Timer::killTimer()
{
    if (getId() >= 0) {
        m_counter.stop();
        TimerManager::instance()->removeTimerInternal(this);
	}
}

//
// TimerManager
//

// Call a non-threaded timer
void TimerManager::callTimer(Timer *timer)
{
    O3D_ASSERT(timer);

    if (!timer) {
        return;
    }

    if (timer->getTimerMode() == Timer::TIMER_ONCE)	{
        Int32 timeOut = timer->call();
        if (timeOut != (Int32)timer->getTimeout()) {
            timer->m_counter.set((UInt32)timeOut);
            timer->m_timeout = timeOut;
            timer->m_counter.start();
        }
    } else {
        if (timer->call() == -1) {
            timer->killTimer();
        } else {
            // timeout from now
            timer->m_counter.set((UInt32)timer->m_timeout);
        }
    }
}

// Timer thread
Int32 TimerManager::run(void*)
{
    Int32 size = 0;
    IT_TemplateManager it;
    Timer *timer = nullptr;
    Bool running = False;
    Int32 wait = 10;

    for (;;) {
        m_mutex.lock();
        running = m_running;
        size = getNumElt();
        m_mutex.unlock();

        // stopped or nothing to process
        if (!size or !running) {
            break;
        }

        // @todo running queue
        m_mutex.lock();
        it = begin();
        while (it != end()) {
            m_mutex.unlock();

            if (it->second->getTimerType() == Timer::NON_THREADED) {
                timer = static_cast<Timer*>(it->second);
            }

            if (!timer) {
                continue;
            }

            // killed
            if (!timer->m_counter.isRunning()) {
                break;
            }

            timer->m_counter.update();

            if (timer->m_counter.isTimedOut()) {
                // wait the caller that will reset the counter once the callback is done
                // timer->m_counter.reset();

                // push event to the main thread
                // and become of the responsibility of TimerManager::callTimer and the main loop
                Application::pushEvent(Application::EVENT_STD_TIMER, 0, timer);

                // once timer processed, break
                if (timer->getTimerMode() == Timer::TIMER_ONCE) {
                    break;
                }
            }

            wait = o3d::min(wait, timer->m_counter.getRemaining());

            m_mutex.lock();
            ++it;
        }
        m_mutex.unlock();

        // don't waste the CPU, but this can delay of 10 ms a new entry of timer
        if (wait == 0) {
            System::waitMs(0);  // simple yeld
        } else if (wait <= 5) {
            System::waitMs(1);  // keep precision
        } else if (wait <= 10) {
            System::waitMs(2);  // keep precision
        } else {
            System::waitMs(10);
        }
    }

    return 0;
}

#endif // !O3D_WIN32

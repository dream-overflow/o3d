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
    if (callback && getId() < 0 && !m_counter.isRunning()) {
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
void TimerManager::timerCall(Int32 id, Timer* timer)
{
    if (!timer || id < 0) {
        return;
    }

    BaseTimer *ltimer = nullptr;
    Bool wakeup = False;

    m_mutex.lock();
    ltimer = get(id);
    m_mutex.unlock();

    // deleted or invalid timer
    if (!ltimer || ltimer != timer) {
        return;
    }

    if (timer->getTimerMode() == Timer::TIMER_ONCE)	{
        Int32 timeOut = timer->call();
        if (timeOut != (Int32)timer->getTimeout()) {
            timer->m_counter.set((UInt32)timeOut);
            timer->m_timeout = timeOut;
            timer->m_counter.start();

            wakeup = True;
        }
    } else {
        if (timer->call() == -1) {
            timer->killTimer();
        } else {
            // throw again
            m_mutex.lock();
            m_queue.insert(std::pair<Int32, Timer*>(Int32(timer->getTimeout() + System::getMsTime()), timer));
            m_mutex.unlock();

            wakeup = True;
        }
    }

    // wakeup
    if (!m_thread.isThread() && wakeup) {
        m_running = True;
        m_thread.start();
    }
}

Bool TimerManager::isRunningTimerInternal(Timer *timer)
{
    FastMutexLocker locker(m_mutex);
    return m_running && timer != nullptr && timer == m_currentTimer;
}

// Timer thread
Int32 TimerManager::run(void*)
{
    Int32 size = 0;
    std::multimap<Int32, Timer*>::iterator head;
    Bool running = False;
    Int32 wait = 10;

    for (;;) {
        wait = 10;

        m_mutex.lock();
        running = m_running;
        size = m_queue.size();
        m_mutex.unlock();

        // stopped or nothing to process
        if (!size || !running) {
            break;
        }

        m_mutex.lock();
        head = m_queue.begin();
        if (head != m_queue.end()) {
            m_currentTimer = head->second;
            running = m_currentTimer->m_counter.isRunning();

            if (!running) {
                m_queue.erase(head);
                m_mutex.unlock();
            } else if (head->first <= System::getMsTime()) {
                // process if expired
                m_queue.erase(head);
                m_mutex.unlock();

                // signal
                onTimerCall(m_currentTimer->getId(), m_currentTimer);
            } else {
                m_mutex.unlock();
            }

            // compute wait
            m_mutex.lock();
            head = m_queue.begin();
            if (head != m_queue.end()) {
                wait = o3d::max(0, head->first - System::getMsTime());
            }
            m_currentTimer = nullptr;
            m_mutex.unlock();
        } else {
            m_mutex.unlock();
        }

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

/**
 * @file timer.cpp
 * @brief simple timer with callback (common part)
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-03-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/timer.h"
#include "o3d/core/application.h"

#include "o3d/core/debug.h"

using namespace o3d;

//
// class BaseTimer
//

BaseTimer::BaseTimer(UInt32 timeout, BaseTimer::TimerMode mode, Callback *callback, void *data) :
    BaseObject(),
    m_timeout(timeout),
    m_mode(mode),
    m_pCallback(callback)
{
}

// Destructor
BaseTimer::~BaseTimer()
{
    destroy();
}

void BaseTimer::destroy()
{

}

Int32 BaseTimer::call()
{
    //! manually call of the callback (used by the manager, musn't be used in other way)
    if (TimerManager::instance()->getActivity() && m_pCallback) {
        return m_pCallback->call(nullptr);
    }
    return 0;
}

UInt32 BaseTimer::getRunningThreadId() const
{
    return ThreadManager::getMainThreadId();
}

//
// class Timer
//

// constructor
Timer::Timer(UInt32 timeout, TimerMode mode, Callback *callback, void *data) :
    BaseTimer(timeout, mode, callback, data)
#ifdef O3D_WIN32
    ,m_handle(NULL)
#endif
{
    if (m_pCallback) {
        create(m_timeout, m_mode, m_pCallback, data);
    }
}

Timer::~Timer()
{
    destroy();
}

Timer::TimerType Timer::getTimerType() const
{
   return NON_THREADED;
}

//
// class TimerThread
//

// constructor
TimerThread::TimerThread(UInt32 timeout, TimerMode mode, Callback *callback, void *data) :
    BaseTimer(timeout, mode, callback, data),
    m_thread(this)
{
    if (m_pCallback) {
        create(m_timeout, m_mode, m_pCallback, data);
    }
}

TimerThread::~TimerThread()
{
    destroy();
}

UInt32 TimerThread::getRunningThreadId() const
{
    return m_thread.getThreadID();
}

Timer::TimerType TimerThread::getTimerType() const
{
   return THREADED;
}

Bool TimerThread::create(UInt32 timeout, TimerMode mode, Callback *callback, void *data)
{
    if (!m_thread.isThread() && callback && getId() < 0) {
        m_mode = mode;
        m_timeout = timeout;

        if (m_pCallback != callback) {
            deletePtr(m_pCallback);
            m_pCallback = callback;
        }

        TimerManager::instance()->addTimerInternal(this);

        m_counter.set(timeout);
        m_counter.start();
        m_thread.start();

        return True;
    } else {
        // already running
        return False;
    }
}

void TimerThread::destroy()
{
    killTimer();

    m_timeout = 0;
    deletePtr(m_pCallback);
}

// Timer thread
Int32 TimerThread::run(void*)
{
    Int32 result = 0;
    Int32 remaining = 0;
    Bool running = True;
    Bool timedOut = False;

    for (;;) {
        m_mutex.lock();
        running = m_counter.isRunning();
        m_counter.update();
        timedOut = m_counter.isTimedOut();
        m_mutex.unlock();

        // killed
        if (!running) {
            break;
        }

        if (timedOut) {
            // called from the thread
            result = m_pCallback->call(nullptr);

            // stop the timer (once or continue)
            if (result < 0) {
                break;
            }

            // reset once callback done
            m_mutex.lock();

            m_counter.set(m_timeout);  // m_counter.reset();
            // remaining = m_counter.getRemaining();
            remaining = m_timeout;

            // once timer can be throw again if result is different from previous timeout
            if (m_mode == TIMER_ONCE) {
                if (result != (Int32)m_timeout) {
                    // once again
                    m_counter.set((UInt32)result);
                } else {
                    running = False;
                }
            }

            m_mutex.unlock();
        }

        // once done
        if (!running) {
            break;
        }

        // don't waste the CPU
        if (remaining == 0) {
            System::waitMs(0);  // simple yeld
        } else if (remaining <= 5) {
            System::waitMs(1);  // keep precision
        } else if (remaining <= 10) {
            System::waitMs(2);  // keep precision
        } else {
            System::waitMs(10);
        }
    }

    return 0;
}

void TimerThread::throwTimer(UInt32 timeout)
{
    if (!m_thread.isThread() && m_pCallback && getId() < 0) {
        TimerManager::instance()->addTimerInternal(this);

        m_timeout = timeout;
        m_counter.set(timeout);
        m_counter.start();

        m_thread.start();
    }
}

void TimerThread::killTimer()
{
    if (m_thread.isThread() && getId() >= 0) {
        TimerManager::instance()->removeTimerInternal(this);

        m_mutex.lock();
        m_counter.stop();
        m_mutex.unlock();

        m_thread.waitFinish();
    }
}

//
// class TimerManager
//

// Singleton instantiation
TimerManager* TimerManager::m_instance = nullptr;

TimerManager* TimerManager::instance()
{
    if (m_instance == nullptr) {
		m_instance = new TimerManager();
    }

    return m_instance;
}

// Singleton destruction
void TimerManager::destroy()
{
	// Must be destroyed manually since the destructor of O3DTimer objects needs the
	// TimerManager to have a valid hash map.
    if (m_instance != nullptr) {
        delete m_instance;
        m_instance = nullptr;
    }
}

TimerManager::~TimerManager()
{
#ifndef O3D_WIN32
    m_mutex.lock();
    m_running = False;
    m_mutex.unlock();
#endif

    m_mutex.lock();
    while (getNumElt()) {
        m_mutex.unlock();

        BaseTimer* timer = getFirstElement();
        m_mutex.unlock();

        deletePtr(timer);

        m_mutex.lock();
    };
    m_mutex.unlock();

#ifndef O3D_WIN32
    m_thread.waitFinish();
#endif
}

// constructor
TimerManager::TimerManager() :
    TemplateManager<BaseTimer>(nullptr),
    m_useCallbacks(true)
#ifndef O3D_WIN32
    ,m_thread(this),
    m_running(False),
    m_currentTimer(nullptr)
#endif
{
	m_instance = (TimerManager*)this;

    // listen to himself for asynchronous timer event of non-threaded timer
    onTimerCall.connect(this, &TimerManager::timerCall, EvtHandler::CONNECTION_ASYNCH);
}

// remove a timer by it's Id
void TimerManager::deleteTimer(Int32 Id)
{
	FastMutexLocker locker(m_mutex);
	deleteElement(Id);
}

// Get a timer by it's Id
BaseTimer* TimerManager::getTimer(Int32 Id)
{
	FastMutexLocker locker(m_mutex);
	return get(Id);
}

void TimerManager::addTimerInternal(BaseTimer* pTimer)
{
    if (!pTimer || pTimer->getId() > 0) {
        return;
    }

    Bool wakeup = False;

    m_mutex.lock();
    addElement(pTimer);

    if (pTimer->getTimerType() == Timer::NON_THREADED) {
        Timer *ltimer = static_cast<Timer*>(pTimer);
        m_queue.insert(std::pair<Int32, Timer*>(Int32(ltimer->getTimeout() + System::getMsTime()), ltimer));
        wakeup = True;
    }

    m_mutex.unlock();

#ifdef O3D_WIN32
    if (pTimer->getTimerType() == Timer::NON_THREADED) {
        Timer *timer = static_cast<Timer*>(pTimer);
        m_handlesMap[timer->m_handle] = timer;
    }
#else
    if (!m_thread.isThread() && wakeup) {
        m_running = True;
        m_thread.start();
    }
#endif
}

void TimerManager::removeTimerInternal(BaseTimer* pTimer)
{
    if (!pTimer || pTimer->getId() < 0) {
        return;
    }

    m_mutex.lock();
    removeElement(pTimer);

    if (pTimer->getTimerType() == Timer::NON_THREADED) {
        Timer *ltimer = static_cast<Timer*>(pTimer);

        auto it = m_queue.begin();
        while (it != m_queue.end()) {
            if (it->second == ltimer) {
                m_queue.erase(it);
                break;
            }

            ++it;
        }
    }

#ifdef O3D_WIN32
    if (pTimer->getTimerType() == Timer::NON_THREADED) {
        Timer *timer = static_cast<Timer*>(pTimer);

        auto it = m_handlesMap.find(timer->m_handle);
        if (it != m_handlesMap.end()) {
            m_handlesMap.erase(it);
        }
    }
#endif

    m_mutex.unlock();
}

void TimerManager::setActivity(Bool state)
{
    m_useCallbacks = state;
}

Bool TimerManager::getActivity() const
{
    return m_useCallbacks;
}

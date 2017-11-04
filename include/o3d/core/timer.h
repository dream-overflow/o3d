/**
 * @file timer.h
 * @brief Simple timer with callback
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-03-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TIMER_H
#define _O3D_TIMER_H

#include "memorydbg.h"
#include "callback.h"
#include "baseobject.h"
#include "templatemanager.h"
#include "thread.h"
#include "templatearray.h"

#include <map>

namespace o3d {

#ifdef O3D_WIN32
	typedef unsigned int _Timer;
#endif // O3D_WIN32

class TimerManager;

/**
 * @brief Base timer class.
 */
class O3D_API BaseTimer : public BaseObject
{
    friend class TimerManager;

public:

    //! The timer behavior
    //! Notice a TimerTimeOut can cause undefined behaviors in debugging (assert,breakpoint...)
    enum TimerType
    {
        NON_THREADED,   //!< non-threaded timer (using event pool and a master thread)
        THREADED        //!< threaded timer (using decicated thread)
    };

    //! The timer behavior
    //! Notice a TimerTimeOut can cause undefined behaviors in debugging (assert,breakpoint...)
    enum TimerMode
    {
        TIMER_TIMEOUT, //!< Default behavior, the timer is called at a defined timeout
                       //!< If the callback method return -1 then the timer is killed.
        TIMER_ONCE	   //!< Timer is restarted at the end of each call if the callback return a
                       //!< value which differ from the previous timeout. A zero or -1 value
                       //!< has for effect to do not restart the timer.
    };

    //! Constructor
    explicit BaseTimer(
        UInt32 timeout = 0,
        TimerMode mode = TIMER_TIMEOUT,
        Callback *callback = nullptr,
        void *data = nullptr);

    //! Destructor
    virtual ~BaseTimer();

    //! Create the new timer
    virtual Bool create(UInt32 timeout, TimerMode mode, Callback *pCallback, void *pData = nullptr) = 0;

    //! delete the time (kill it)
    virtual void destroy();

    //! Get the timer type
    virtual TimerType getTimerType() const = 0;

    //! Get running thread id, for non threaded timer return the main thread id, which is normally running
    //! the main loop.
    //! @todo and if the main loop in running in a different thread ?
    virtual UInt32 getRunningThreadId() const;

    //! Throw (start) a non running timer.
    virtual void throwTimer(UInt32 timeout) = 0;

    //! Kill if running, but does not destroy data.
    virtual void killTimer() = 0;

    //! manually call of the callback (used by the manager, musn't be used in other way)
    Int32 call();

    //! Get the timer timeout
    inline UInt32 getTimeout() const { return m_timeout; }

    //! Get the timer mode
    inline TimerMode getTimerMode() const { return m_mode; }

protected:

    UInt32 m_timeout;        //!< the time-out
    TimerMode m_mode;        //!< Timer call mode

    Callback *m_pCallback;   //!< the executed callback at time-out
};

/**
 * @brief Internal timer time management helper.
 * Can manage the time shifting, but it is not enabled by default.
 */
class TimerCounter
{
public:

    TimerCounter() :
        run(False),
        remaining(0),
        timeOut(0),
        nextTime(0)
    {
    }

    inline void update()
    {
        remaining = nextTime - System::getMsTime();

        // avoid infinite rethrow when processing is to long
        if (remaining > timeOut) {
            remaining = timeOut;
        } else if (remaining < -timeOut) {
            remaining = -timeOut;
        }
    }

    inline Bool isTimedOut() const {
        return remaining <= 0;
    }

    inline void reset() {
        // avoid time shifting due to lack of precision and of time of processing
        nextTime += timeOut + remaining;
    }

    inline void set(UInt32 ms) {
        timeOut = (Int32)ms;

        // next time to call the timer
        nextTime = System::getMsTime() + timeOut;
    }

    inline Int32 getRemaining() const {
        return remaining;
    }

    /**
     * @brief stop must be mutex protected
     */
    inline void stop() {
        run = False;
    }

    /**
     * @brief stop must be mutex protected
     */
    inline void start() {
        run = True;
    }

    /**
     * @brief stop must be mutex protected
     */
    inline Bool isRunning() const {
        return run;
    }

private:

    Bool run;
    Int32 remaining;
    Int32 timeOut;
    Int32 nextTime;
};

/**
 * @brief Simple non-threaded timer (using event pool) with callback.
 */
class O3D_API Timer : public BaseTimer
{
    friend class TimerManager;

public:

    explicit Timer(
            UInt32 timeout = 0,
            TimerMode mode = TIMER_TIMEOUT,
            Callback *callback = nullptr,
            void *data = nullptr);

	virtual ~Timer();

    virtual Bool create(
		UInt32 timeout,
		TimerMode mode,
		Callback *pCallback,
        void *pData = nullptr) override;

    virtual void destroy() override;
    virtual TimerType getTimerType() const override;
    virtual void throwTimer(UInt32 timeout) override;
    virtual void killTimer() override;

private:

#ifdef O3D_WIN32
	_Timer m_handle;         //!< Timer handle.
#else
    FastMutex m_mutex;
    TimerCounter m_counter;
#endif
};

/**
 * @brief Threaded timer (using a decicated thread) with callback.
 */
class O3D_API TimerThread : public BaseTimer, public Runnable
{
    friend class TimerManager;

public:

    //! Constructor
    explicit TimerThread(
        UInt32 timeout = 0,
        TimerMode mode = TIMER_TIMEOUT,
        Callback *callback = nullptr,
        void *data = nullptr);

    //! Destructor
    virtual ~TimerThread();

    virtual Bool create(
        UInt32 timeout,
        TimerMode mode,
        Callback *pCallback,
        void *pData = nullptr) override;

    virtual void destroy() override;
    virtual TimerType getTimerType() const override;
    virtual void throwTimer(UInt32 timeout) override;
    virtual void killTimer() override;
    virtual UInt32 getRunningThreadId() const override;

private:

    FastMutex m_mutex;
    Thread m_thread;
    TimerCounter m_counter;

    //! Runnable
    Int32 run(void*);
};

/**
 * @brief A singleton manager for timers (threader or not)
 */
class O3D_API TimerManager : public TemplateManager<BaseTimer>, public Runnable
{
public:

	static TimerManager* instance();
	static void destroy();

    virtual ~TimerManager();

    //! Delete a currently running timer by it's unique identifier
	void deleteTimer(Int32 Id);

    //! Get a currently running timer by it's unique identifier
    BaseTimer* getTimer(Int32 Id);

	//! Add the timer handle given a timer object
    void addTimerInternal(BaseTimer* pTimer);

	//! Remove the timer handle given a timer
    void removeTimerInternal(BaseTimer* pTimer);

    //! Set the bypass state of calling timer callback
    void setActivity(Bool state);

    //! Get the bypass state of calling timer callback
    Bool getActivity() const;

#ifdef O3D_WIN32
    //! Get timer by its internal WIN32 handler
    Timer* getTimerByHandleInternal(_Timer h);
#else
	//! Call a timer.
    void callTimer(void *data);

    //! Is the timer currently performed.
    Bool isRunningTimerInternal(Timer* timer);
#endif

public:

    //! Called when a timer as to be processed into the application thread
    Signal<Int32, Timer*> onTimerCall{this};

    void timerCall(Int32 id, Timer* timer);

private:

    static TimerManager *m_instance;

    TimerManager();
    TimerManager(const TimerManager& tim);
    void operator=(const TimerManager& tim);

    FastMutex m_mutex;
    Bool m_useCallbacks;

#ifdef O3D_WIN32
    stdext::hash_map<_Timer, Timer*> m_handlesMap;
#else
    //! For non threaded timers
    Thread m_thread;
    Bool m_running;

    std::multimap<Int32, Timer*> m_queue;
    Timer *m_currentTimer;

    struct TimerEvent
    {
        Timer *timer;
        Int32 id;
    };
#endif

    //! Runnable
    Int32 run(void*);
};

} // namespace o3d

#endif // _O3D_TIMER_H

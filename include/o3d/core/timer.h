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

#include <map>

#ifndef O3D_WIN32
	#include "idmanager.h"
#endif // O3D_WIN32

namespace o3d {

#ifdef O3D_WIN32
	typedef unsigned int _Timer;
#else
	typedef Int32 _Timer;
	#define O3D_STD_TIMER
#endif // O3D_WIN32

class TimerManager;

//---------------------------------------------------------------------------------------
//! @class Timer
//-------------------------------------------------------------------------------------
//! Simple non-threaded timer (using event pool) with callback.
//---------------------------------------------------------------------------------------
#ifdef O3D_STD_TIMER
class O3D_API Timer : public BaseObject, public Runnable
#else
class O3D_API Timer : public BaseObject
#endif
{
public:

	friend class TimerManager;

	//! The timer behavior
	//! Notice a TimerTimeOut can cause undefined behaviors in debugging (assert,breakpoint...)
	enum TimerType
	{
		NON_THREADED,   //!< non-threaded timer (using event pool)
        THREADED        //!< threaded timer (using Thread)
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
	explicit Timer(
		UInt32 timeout = 0,
		TimerMode mode = TIMER_TIMEOUT,
        Callback *callback = nullptr,
        void *data = nullptr);

	//! Destructor
	virtual ~Timer();

	//! Create the new timer
	Bool create(
		UInt32 timeout,
		TimerMode mode,
		Callback *pCallback,
        void *pData = nullptr);

	//! delete the time (kill it)
	void destroy();

	//! Get the timer type
	inline TimerType getType() const { return m_type; }

    //! Throw a timer to process. Can restart a killed timer but cannot restart it if destroyed.
	//! @param timeout The new timeout value
	//! @warning Not supported in SDL mode.
	void throwTimer(UInt32 timeout);

    //! Kill a timer in process. Can kill a created or thrown timer. After that the timer
    //! can be restarted using throwTimer() but not by using create(...).
	//! @warning Not supported in SDL mode.
	void killTimer();

	//! manually call of the callback (used by the manager, musn't be used in other way)
	inline Int32 call()
	{
        if (m_enabled && m_pCallback) {
            return m_pCallback->call(nullptr);
        }
		return 0;
	}

	//! Return the internal system timer handle
	inline _Timer getHandle() const { return m_handle; }

    //! Define the internal system timer handle
	inline void setHandle(_Timer sig) { m_handle = sig; }

	//! Get the timer timeout
	inline UInt32 getTimeout() const { return m_timeout; }

	//! Get the timer mode
	inline TimerMode getTimerMode() const { return m_mode; }

	//! Get owner thread.
	inline UInt32 getOwnerThread() const { return m_threadId; }

	//! Set the bypass state of calling timer callback
	static void setTimersActivity(Bool state) { m_enabled = state; }

	//! Get the bypass state of calling timer callback
	static Bool getTimersActivity() { return m_enabled; }

private:

    TimerType m_type;        //!< type of the timer

    UInt32 m_timeout;        //!< the time-out
    TimerMode m_mode;        //!< Timer call mode

	Callback *m_pCallback;   //!< the executed callback at time-out

	_Timer m_handle;         //!< Timer handle.
    UInt32 m_threadId;       //!< Thread that own this timer.

	static Bool m_enabled;  //!< true mean the timer call are processed, false bypass their call

#ifdef O3D_STD_TIMER
	class Counter
	{
	public:

		Counter() :
			timeOut(0),
			lastTime(0)
		{
		}

		inline Bool isTimedOut()
		{
            if (lastTime == 0) {
				lastTime = System::getMsTime();
				return False;
            } else {
				UInt32 elapsed = System::getMsTime() - lastTime;
				return (elapsed >= timeOut);
			}
		}

		inline void reset() { lastTime = System::getMsTime(); }
		inline void set(UInt32 ms) { timeOut = ms; }

	private:

		UInt32 timeOut;
		UInt32 lastTime;
	};

	Counter m_counter;
	static IDManager m_sigManager;

	//! Runnable
	Int32 run(void*);

	Thread m_thread;
#endif // O3D_STD_TIMER
};

//---------------------------------------------------------------------------------------
//! @class TimerManager
//-------------------------------------------------------------------------------------
//! A singleton manager for timers (threader or not)
//---------------------------------------------------------------------------------------
class O3D_API TimerManager : TemplateManager<Timer>
{
public:

	static TimerManager* instance();
	static void destroy();

	//! create a new (non-threaded) timer using callback and return it's new Id
	//! @param timeout Timeout in ms
	//! @param mode Once time or repeated
	//! @param callback Called function or method at timeout
	//! @param data Extra data sent to the callback in parameter
	Int32 addTimer(
		UInt32 timeout,
		Timer::TimerMode mode,
		Callback *callback,
		void *data = NULL);

	//! Remove a timer by it's unique identifier
	void deleteTimer(Int32 Id);

	//! Get a timer by it's unique identifier
	Timer* getTimer(Int32 Id);

	//! Retrieve a timer given its intem_handlesMaprnal system handle
	Timer* getTimerInternal(_Timer handle);

	//! Add the timer handle given a timer object
	inline void addTimerInternal(Timer* pTimer) { m_handlesMap[pTimer->getHandle()] = pTimer; }

	//! Remove the timer handle given a timer
	inline void removeTimerInternal(Timer* pTimer) { m_handlesMap.erase(pTimer->getHandle()); }

#ifdef O3D_STD_TIMER

	//! Call a timer.
	void callTimer(Timer *timer);

#endif // O3D_STD_TIMER

private:

    static TimerManager *m_instance;

    stdext::hash_map<_Timer,Timer*> m_handlesMap;

    TimerManager();
    TimerManager(const TimerManager& tim);
    void operator=(const TimerManager& tim);

    FastMutex m_mutex;
};

} // namespace o3d

#endif // _O3D_TIMER_H

/**
 * @file evtmanager.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-11-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_EVTMANAGER_H
#define _O3D_EVTMANAGER_H

#include "o3d/core/types.h"

#include <map>
#include <list>

namespace o3d {

class Thread;
class EvtFunctionAsyncBase;
class EvtHandler;
class FastMutex;
class Callback;

/**
 * @brief Event singleton manager.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-11-14
 *
 * For each thread it is possible to have a pool of asynchronous events, that are
 * called at a specific time.
 * The event manager can be used with the o3d::Application event loop in way
 * to wake-up the application event loop when there is a new posted event to process.
 */
class O3D_API EvtManager
{
private:

    typedef std::list<EvtFunctionAsyncBase*> T_EventList;
    typedef T_EventList::iterator            IT_EventList;
    typedef T_EventList::const_iterator      CIT_EventList;

	typedef std::map<Thread *, T_EventList>	T_PoolMap;
	typedef T_PoolMap::iterator				IT_PoolMap;
	typedef T_PoolMap::const_iterator		CIT_PoolMap;

	// Members
	T_PoolMap m_poolMap;

    mutable FastMutex m_mutex;

	Bool m_mainMessage;
	Bool m_isAutoWakeUp;

    Callback *m_wakeUpCallback;

	static EvtManager * m_pInstance;

	// Restricted
	EvtManager(const EvtManager &);
	EvtManager & operator = (const EvtManager &);

	IT_PoolMap getPoolMap(UInt32);
	CIT_PoolMap getPoolMap(UInt32) const;

	// Constructors
	EvtManager();
	~EvtManager();

public:

	static EvtManager * instance();
	static void destroy();

    //! Post a event. It will be delivered when the function processEvent will be called from the thread
	//! associated to the object.
	//! Event posted in a invalid thread will be automatically deleted.
    void postEvent(EvtFunctionAsyncBase *);

	//! Register a thread. A thread must call this function if it wants to be able to deliver events
	void registerThread(Thread *);
	void unRegisterThread(Thread *);
	Bool isThreadRegistered(Thread *) const;

	//! Process all events concerning the current thread
	//! @return the number of event processed
	UInt32 processEvent();

	//! Process all events concerning a specific thread.
	//! This function must be used only if you know exactly what you're doing.
	//! @return the number of event processed
	UInt32 processEvent(Thread *);

	//! Process only events concerning a specific object in the current thread
	//! @return the number of event processed
	UInt32 processEvent(EvtHandler *);

	//! Process all events concerning a specific thread and a specific object
	//! This function must be used only if you know exactly what you're doing.
	//! @return the number of event processed
	UInt32 processEvent(EvtHandler *, Thread *);

	//! Remove all pending events concerning the current thread
	void deletePendingEvents();

	//! Remove all pending events concerning an object
	void deletePendingEvents(EvtHandler*);

	//! Return TRUE if there are pending events in the current thread
	Bool isPendingEvent() const;

	//! Return TRUE if there are pending events in the provided thread
	Bool isPendingEvent(Thread *) const;

	//! Return TRUE if there are pending events concerning a specific object
	Bool isPendingEvent(EvtHandler *) const;

	//! Return TRUE if there are pending events concerning a specific object in the provided thread
	Bool isPendingEvent(EvtHandler *, Thread *) const;

	//! Enable PostMessage for the main thread
	void enableAutoWakeUp();
	//! Disable PostMessage for the main thread
	void disableAutoWakeUp();
	//! Get the state of PostMessage for the main thread
	Bool isAutoWakeUp() const;

    //! Set a callback to receive a notification that a new event has been posted since the last wakeup.
    //! @note This is not sent every posted event, because this will be counter performant, instead of,
    //! once you receive this callback, processing waiting events on the application thread will reset a flag,
    //! and so another callback will be called on futurs events.
    void setWakeUpCallback(Callback *callback);
};

class O3D_API EvtPool
{
private:

	// Members
	Thread * m_pThread;
	Bool m_acquired;

	// Restricted
	EvtPool(const EvtPool &);
	EvtPool & operator = (const EvtPool &);

public:

	// Constructors
	EvtPool();
	EvtPool(Thread *);
	~EvtPool();

	//! @brief Change or initialize the event pool
	//! If the pool event was already initialized. The thread will be unregistered first.
	//! The posted events waiting for processing will be destroyed. So use this function carefully.
	//! This function should only be used to postpone the initialization of the pool event.
	void init(Thread *);

	//! @brief Process all posted events in the thread loop
	void processEvent();

	//! @brief Return the state of the pool
	//! Return TRUE if the pool successfully registered the thread, FALSE otherwise.
	//! Maybe the thread was already registered.
	//! @return the state of the pool
	Bool acquired() const { return m_acquired; }
};

} // namespace o3d

#endif // _O3D_EVTMANAGER_H

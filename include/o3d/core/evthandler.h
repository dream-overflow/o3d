/**
 * @file evthandler.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-11-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_EVTHANDLER_H
#define _O3D_EVTHANDLER_H

#include "classinfo.h"
#include "mutex.h"
#include <map>
#include <vector>

namespace o3d {

class Thread;
class EvtConnection;
class EvtLinkBase;
class EvtFunctionBase;
class EvtFunctionAsyncBase;

/**
 * @brief Catch and thrown events to or from other EvtHandler objects.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-11-13
 *
 * This class gives objects the ability to catch and thrown events to or from other
 * EvtHandler objects. It doesn't inherit from any class, but it handles multiple
 * inheritance, so you can safely use it with your own objects.
 *
 * All functions of this class are thread safe. Moreover recursive events are allowed,
 * but infinite recursive events are allowed too ! so be careful.
 */
class O3D_API EvtHandler
{
	O3D_NONCOPYABLE(EvtHandler)

public:

	O3D_DECLARE_ABSTRACT_CLASS(EvtHandler)

	typedef void (EvtHandler::*T_EvtHandlerFunction)(void);

    //! @brief An enum.
    //! This enum contains the different types of connection you can use. */
    enum ConnectionType
    {
        CONNECTION_AUTO = 0,  //!< Automatic mode.
        CONNECTION_SYNCH,     //!< Synchronous call to the callback
        CONNECTION_ASYNCH     //!< Asynchronous call to the callback
    };

	//! @brief Default constructor
	EvtHandler();
	//! @brief Destructor
	virtual ~EvtHandler();

	//! @brief Define the owner thread.
	//! This function is used to define the thread in which the object will be used.
	//! The owner thread is used to deduce the connection type to use when you connect
	//! an object to an event with the option CONNECTION_AUTO. If the threads are
	//! different, the connection will be asynchronous, otherwise, the synchronous
	//! mode will be used.
	void setThread(Thread *);
	//! @brief Return the owner thread
	Thread * getThread() const;

	//! @brief Disconnect the object from all events.
	//! Every connections will be destroyed by this function but remaining pending
	//! events will still be delivered. If you want to destroy pending events too,
	//! you must explicitly call the event manager.
    //! @see EvtManager::deletePendingEvents(EvtHandler*)
	void disconnect();

	//! @brief Disconnect an object
	//! By this function, all connections between this object and the provided object
	//! will be destroyed. So that if you currently bound to two events of the distant
	//! object, the twice connections will be destroyed.
	//! As EvtHandler::disconnect(), remaining pending events
	//! thrown by the object will still be delivered.
	//! @param _pHandler the event handler from which you want to be disconnected.
    //! @see EvtManager::deletePendingEvents(EvtHandler*)
	void disconnect(EvtHandler * _pHandler);

	//! @brief Disconnect an event. Internal usage.
	//! This function is used to disconnect a specific event by its link.
	//! @param _pLink the linkage associated to the connection.
	void disconnect(EvtLinkBase * _pLink);

	//! @brief Connect an event.
	//! This function allows you to connect this object to an event thrown by a distant
	//! event handler.
    //! @param _pLink a link instance. Can be created by O3D_LINK_EVT(...)
	//! @param _type the type of connection.
    //! @see Signal
    //! @see O3D_LINK_EVT(CLASS, SENDER, EVENT, RECEIVER, FUNC)
    //! @see EvtHandler::ConnectionType
    void connect(EvtLinkBase * _pLink, ConnectionType _type = CONNECTION_AUTO);

	//! @brief Connect an event.
	//! This function allows you to connect this object to an event thrown by a distant
	//! event handler but precise explicitly in which thread you want to catch the
	//! event. For standard use, you should not use this function.
    //! @param _pLink a link to the distant object. Can be created by O3D_LINK_EVT(...)
	//! @param _pThread the thread instance in which you want to catch the events.
	//!                 The connection will be asynchronous.
    //! @see Signal
    //! @see O3D_LINK_EVT(CLASS, SENDER, EVENT, RECEIVER, FUNC)
	void connect(EvtLinkBase * _pLink, Thread * _pThread);

	//! @brief Return the sender
	//! When you catch an event, it means, when you are in a callback, this function
	//! return the handler which throw the event.
	EvtHandler * getSender();

protected:

	friend class EvtFunctionBase;
    friend class EvtFunctionAsyncBase;

	//! @brief Internal function
	//! This function is used to specify who is the sender when you catch a event.
    //! This function is called by EvtFunctionBase just before to throw the event
	//! to this object.
	void setSender(EvtHandler *);

	//! @brief Internal function
	//! This function is used to remove the sender from a sender list after an event
	//! throw.
	void resetSender();

private:

	typedef std::list<EvtConnection>           T_ConnectionArray;
	typedef T_ConnectionArray::iterator        IT_ConnectionArray;
	typedef T_ConnectionArray::const_iterator  CIT_ConnectionArray;

	typedef std::map<UInt32, std::vector<EvtHandler*> > T_SenderMap;
    typedef T_SenderMap::iterator                       IT_SenderMap;
    typedef T_SenderMap::const_iterator                 CIT_SenderMap;

	// Members
	T_ConnectionArray m_connections;	//! Contains the connections to all events.

	Thread * m_pAttachedThread;		//! Owner thread. NULL means the default thread.

	T_SenderMap m_senders;			//! Contains the stack of sender for each thread.

	static RecursiveMutex m_mutex;	//! Mutex shared by all objects.
};


/**
 * @brief Manages connections between events handlers.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-11-13
 *
 * Internal class used to disconnect automatically all events when an event handler is
 * destroyed.
 */
class O3D_API EvtConnection
{
private:

	// Members
	EvtLinkBase * m_pLink;
	Int32 * m_pCounter;

	// Restricted
	void destroy();

public:

	// Constructors
	EvtConnection();
	explicit EvtConnection(EvtLinkBase *);
	EvtConnection(const EvtConnection &);
	~EvtConnection();

	EvtConnection & operator = (const EvtConnection &);

	EvtLinkBase * operator ->() const { return m_pLink; }
};

} // namespace o3d

#endif // _O3D_EVTHANDLER_H


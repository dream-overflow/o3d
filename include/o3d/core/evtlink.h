/**
 * @file evtlink.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-11-13
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_EVTLINK_H
#define _O3D_EVTLINK_H

#include "string.h"
#include "evthandler.h"
#include "evtfunction.h"
#include "evtcontainer.h"

namespace o3d {

/**
 * @brief Base event link. The link between the sender containing the signal and
 * the receiver containing the slot. But can be also a std::function with more a complex
 * context.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-11-13
 */
class O3D_API EvtLinkBase
{
private:

    typedef void (EvtHandler::*T_CallBack)(void);

	// Members
    EvtHandler * m_sender;
    EvtHandler * m_receiver;

    //void * m_baseThis;

	String m_eventName;

    EvtFunctionBase * m_function;

    Bool m_sync;

protected:

	// Restricted
	EvtLinkBase & operator = (const EvtLinkBase &);
	EvtLinkBase(const EvtLinkBase &);

public:

	// Constructors
    EvtLinkBase(EvtHandler *sender,
            EvtHandler *receiver,
            //void *baseThis,
            const String &name,
            EvtFunctionBase *func);

	virtual ~EvtLinkBase();

	virtual EvtLinkBase * clone() const = 0;
	virtual void init() = 0;

    EvtHandler * getSender() const { return m_sender; }
    EvtHandler * getReceiver() const { return m_receiver; }
    //void * getBaseThis() const { return m_baseThis; }

    EvtFunctionBase * getFunction() const { return m_function; }

    inline void setSync(Bool sync) { m_sync = sync; }
    inline Bool isSync() const { return m_sync; }
};

template <class T>
class EvtLink : public EvtLinkBase
{
private:

	typedef T T_FunctionType;
    typedef EvtContainer<T_FunctionType> T_Container;

    T_Container* m_signalContainer;

public:

	// Constructors
    EvtLink(
            EvtHandler *sender,
            EvtHandler *receiver,
            //void * baseThis,  // could be template but actually useless with static_cast
            T_Container *container,
            const String &name,
            T_FunctionType *func):
        EvtLinkBase(
            sender,
            receiver,
            //(void*)baseThis,
            name,
            (EvtFunctionBase*)func),
        m_signalContainer(container)
    {
    }

	EvtLink(const EvtLink & _which):
        EvtLinkBase(_which),
        m_signalContainer(_which.m_signalContainer)
    {
    }

	virtual ~EvtLink()
	{
        O3D_ASSERT(getSender() != nullptr);

        if (getSender() != nullptr)
            m_signalContainer->removeHandler((T_FunctionType*)getFunction());
	}

	virtual EvtLinkBase * clone() const
	{
		return new EvtLink(*this);
	}

	virtual void init()
	{
		getFunction()->setLink(this);
		getFunction()->setSender(getSender());
		getFunction()->setReceiver(getReceiver());
        //getFunction()->setBaseThis(getBaseThis());
        m_signalContainer->addHandler((T_FunctionType*)getFunction());
	}
};

} // namespace o3d

#endif // _O3D_EVTLINK_H


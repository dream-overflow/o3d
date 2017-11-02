/**
 * @file evtlink.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/evtlink.h"

using namespace o3d;

EvtLinkBase::EvtLinkBase(
        EvtHandler *sender,
        EvtHandler *receiver,
        //void *baseThis,
        const String &name,
        EvtFunctionBase *func):
    m_sender(sender),
    m_receiver(receiver),
    //m_baseThis(baseThis),
    m_eventName(name),
    m_function(func),
    m_sync(True)
{
}

EvtLinkBase::EvtLinkBase(const EvtLinkBase & _which):
    m_sender(_which.m_sender),
    m_receiver(_which.m_receiver),
    //m_baseThis(_which.m_baseThis),
	m_eventName(_which.m_eventName),
    m_function(_which.m_function),
    m_sync(_which.m_sync)
{
}

EvtLinkBase::~EvtLinkBase()
{
    deletePtr(m_function);
}

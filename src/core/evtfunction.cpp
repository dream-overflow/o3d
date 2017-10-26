/**
 * @file evtfunction.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/evtfunction.h"

#include "o3d/core/evtmanager.h"
#include "o3d/core/evtlink.h"

using namespace o3d;

EvtFunctionBase::EvtFunctionBase() :
    m_link(nullptr),
    m_sender(nullptr),
    m_receiver(nullptr),
    //m_baseThis(nullptr),
    m_thread(nullptr)
{
}

EvtFunctionBase::EvtFunctionBase(const EvtFunctionBase & _which) :
    m_link(_which.m_link),
    m_sender(_which.m_sender),
    m_receiver(_which.m_receiver),
    //m_baseThis(_which.m_baseThis),
    m_thread(_which.m_thread)
{
}

EvtFunctionBase::~EvtFunctionBase()
{
}

Bool EvtFunctionBase::isSync() const
{
    return m_link->isSync();
}

EvtFunctionAsyncBase::EvtFunctionAsyncBase() :
    m_sender(nullptr),
    m_receiver(nullptr),
    //m_baseThis(nullptr),
    m_thread(nullptr)
{
}

EvtFunctionAsyncBase::~EvtFunctionAsyncBase()
{
}

EvtFunction0Param::EvtFunction0Param(T_FunctionParamPtr _func):
    EvtFunctionBase(),
    m_funcPtr(_func)
{
}

EvtFunction0Param::EvtFunction0Param(const EvtFunction0Param & _which):
    EvtFunctionBase(_which),
    m_funcPtr(_which.m_funcPtr)
{
}

void EvtFunction0Param::call()
{
    O3D_ASSERT(m_link != nullptr);

    if (m_link->isSync())
    {
        setReceiverSender(m_sender);
        (static_cast<EvtHandler*>(getReceiver())->*(m_funcPtr))();
        //(((EvtHandler*)getBaseThis())->*(m_funcPtr))();
        resetReceiverSender();
    }
    else
    {
        EvtFunctionAsync0Param *event = new EvtFunctionAsync0Param(*this, m_funcPtr);
        EvtManager::instance()->postEvent(event);
    }
}

void EvtFunctionAsync0Param::process()
{
    O3D_ASSERT(m_receiver != nullptr);

    setReceiverSender(m_sender);
    (static_cast<EvtHandler*>(getReceiver())->*(m_funcPtr))();
    //(((EvtHandler*)getBaseThis())->*(m_funcPtr))();
    resetReceiverSender();
}

EvtStdFunction0Param::EvtStdFunction0Param(T_StdFunction _func):
    EvtFunctionBase(),
    m_func(_func)
{
}

EvtStdFunction0Param::EvtStdFunction0Param(const EvtStdFunction0Param & _which):
    EvtFunctionBase(_which),
    m_func(_which.m_func)
{
}

EvtStdFunction0Param::~EvtStdFunction0Param()
{
}

void EvtStdFunction0Param::call()
{
    O3D_ASSERT(m_link != nullptr);

    if (m_link->isSync())
    {
        setReceiverSender(m_sender);
        m_func();
        resetReceiverSender();
    }
    else
    {
        EvtStdFunctionAsync0Param *event = new EvtStdFunctionAsync0Param(*this, m_func);
        EvtManager::instance()->postEvent(event);
    }
}

void EvtStdFunctionAsync0Param::process()
{
    O3D_ASSERT(m_receiver != nullptr);

    setReceiverSender(m_sender);
    m_func();
    resetReceiverSender();
}


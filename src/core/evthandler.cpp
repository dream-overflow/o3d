/**
 * @file evthandler.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/thread.h"
#include "o3d/core/evthandler.h"

#include "o3d/core/evtmanager.h"
#include "o3d/core/evtlink.h"
#include "o3d/core/evtfunction.h"
#include "o3d/core/debug.h"

using namespace o3d;

#define O3D_EVT_HANDLER_DEBUG_STACK_SIZE_TRESHOLD 10

RecursiveMutex EvtHandler::m_mutex;

O3D_IMPLEMENT_ABSTRACT_CLASS(EvtHandler, CORE_EVT_HANDLER)

EvtHandler::EvtHandler():
	m_connections(),
    m_pAttachedThread(nullptr)
{
}

EvtHandler::~EvtHandler()
{
	disconnect();

    EvtManager::instance()->deletePendingEvents(this);
}

void EvtHandler::setThread(Thread * _pThread)
{
	RecurMutexLocker lLocker(m_mutex);

	m_pAttachedThread = _pThread;
}

Thread * EvtHandler::getThread() const
{
	RecurMutexLocker lLocker(m_mutex);

	return m_pAttachedThread;
}

void EvtHandler::disconnect()
{
	RecurMutexLocker lLocker(m_mutex);

	m_connections.clear();
}

void EvtHandler::disconnect(EvtHandler * _pHandler)
{
	RecurMutexLocker lLocker(m_mutex);

	IT_ConnectionArray it = m_connections.begin();

	while (it != m_connections.end())
	{
		if ((*it)->getSender() == _pHandler)
			it = m_connections.erase(it);
		else
			it++;
	}
}

void EvtHandler::disconnect(EvtLinkBase * _pLink)
{
	RecurMutexLocker lLocker(m_mutex);

	IT_ConnectionArray it = m_connections.begin();

	while (it != m_connections.end())
	{
		if ((*it).operator ->() == _pLink)
			it = m_connections.erase(it);
		else
			it++;
	}
}

void EvtHandler::connect(EvtLinkBase * _pLink, ConnectionType _type)
{
	RecurMutexLocker lLocker(m_mutex);

	_pLink->getFunction()->setThread(m_pAttachedThread);
    _pLink->setSync(
			(_type == CONNECTION_SYNCH) ||
			((_type == CONNECTION_AUTO) && (_pLink->getReceiver()->getThread() == _pLink->getSender()->getThread())));
	_pLink->init();

	m_connections.push_back(EvtConnection(_pLink));
}

void EvtHandler::connect(EvtLinkBase * _pLink, Thread * _pThread)
{
	RecurMutexLocker lLocker(m_mutex);

	_pLink->getFunction()->setThread(_pThread);
    _pLink->setSync(False);
	_pLink->init();

	m_connections.push_back(EvtConnection(_pLink));
}

void EvtHandler::setSender(EvtHandler * _pHandler)
{
	RecurMutexLocker lLocker(m_mutex);

	#ifdef _DEBUG
		std::vector<EvtHandler *> & lStack = m_senders[ThreadManager::getThreadId()];
		lStack.push_back(_pHandler);

		if (lStack.size() > O3D_EVT_HANDLER_DEBUG_STACK_SIZE_TRESHOLD)
			O3D_ERROR(E_InvalidOperation("Infinite recursive event call detected"));
	#else
		m_senders[ThreadManager::getThreadId()].push_back(_pHandler);
	#endif
}

void EvtHandler::resetSender()
{
	RecurMutexLocker lLocker(m_mutex);

	IT_SenderMap it = m_senders.find(ThreadManager::getThreadId());

	O3D_ASSERT((it != m_senders.end()) && !it->second.empty());

	if (it != m_senders.end())
	{
		if (!it->second.empty())
			it->second.pop_back();

		if (it->second.empty())
			m_senders.erase(it);
	}
}

EvtHandler * EvtHandler::getSender()
{
	RecurMutexLocker lLocker(m_mutex);

	CIT_SenderMap it = m_senders.find(ThreadManager::getThreadId());

	O3D_ASSERT(it != m_senders.end());

	if (it != m_senders.end())
		return it->second.back();
	else
        return nullptr;
}

EvtConnection::EvtConnection():
    m_pLink(nullptr),
    m_pCounter(nullptr)
{
}

EvtConnection::EvtConnection(EvtLinkBase * _pLink):
	m_pLink(_pLink),
	m_pCounter(new Int32(1))
{
    O3D_ASSERT(_pLink != nullptr);
}

EvtConnection::EvtConnection(const EvtConnection & _which):
	m_pLink(_which.m_pLink),
	m_pCounter(_which.m_pCounter)
{
    if (m_pCounter != nullptr)
		(*m_pCounter)++;
}

EvtConnection::~EvtConnection()
{
	destroy();
}

EvtConnection & EvtConnection::operator = (const EvtConnection & _which)
{
	if (this == &_which)
		return *this;

	destroy();

	m_pLink = _which.m_pLink;
	m_pCounter = _which.m_pCounter;

    if (m_pCounter != nullptr)
		(*m_pCounter)++;

	return *this;
}

void EvtConnection::destroy()
{
    if (m_pCounter != nullptr)
	{
		O3D_ASSERT(*m_pCounter > 0);

		if (--(*m_pCounter) == 0)
		{
			deletePtr(m_pCounter);
			deletePtr(m_pLink);
		}
	}
}


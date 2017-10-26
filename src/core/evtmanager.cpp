/**
 * @file evtmanager.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/thread.h"
#include "o3d/core/evtmanager.h"

#include "o3d/core/evt.h"
#include "o3d/core/debug.h"

#include "o3d/core/application.h"

using namespace o3d;

EvtManager * EvtManager::m_pInstance = nullptr;

EvtManager::EvtManager():
	m_poolMap(),
	m_mutex(),
    m_mainMessage(False),
    m_isAutoWakeUp(True)
{
}

EvtManager::~EvtManager()
{
}

EvtManager * EvtManager::instance()
{
    if (m_pInstance == nullptr)
		m_pInstance = new EvtManager();

	return m_pInstance;
}

void EvtManager::destroy()
{
	if (m_pInstance)
	{
		delete m_pInstance;
        m_pInstance = nullptr;
	}
}

void EvtManager::postEvent(EvtFunctionAsyncBase * _pFunction)
{
    O3D_ASSERT(_pFunction != nullptr);

	// On détermine dans quel thread il faut placer l'événement
	Thread * lThread = _pFunction->getThread();

	FastMutexLocker lLocker(m_mutex);

	IT_PoolMap it = m_poolMap.find(lThread);
	
	if (it != m_poolMap.end())
	{
		it->second.push_back(_pFunction);

		// Event for the main thread
        if ((lThread == nullptr) && m_isAutoWakeUp && !m_mainMessage)
		{
			Application::pushEvent(0xffff, 0, 0);
            m_mainMessage = True;
		}
	}
	else
	{
		deletePtr(_pFunction);
		O3D_ERROR(E_InvalidOperation("Thread not registered in the EvtManager"));
	}
}

void EvtManager::registerThread(Thread * _pThread)
{
	FastMutexLocker lLocker(m_mutex);

	IT_PoolMap it = m_poolMap.find(_pThread);

	if (it == m_poolMap.end())
		m_poolMap[_pThread] = T_EventList();
	else
		O3D_ERROR(E_InvalidOperation("Attempt to register twice the same thread in the EvtManager"));
}

void EvtManager::unRegisterThread(Thread * _pThread)
{
	FastMutexLocker lLocker(m_mutex);

	IT_PoolMap it = m_poolMap.find(_pThread);
	if (it != m_poolMap.end())
	{
		T_EventList & lList = it->second;
		for (IT_EventList itList = lList.begin() ; itList != lList.end() ; itList++)
			deletePtr(*itList);

		m_poolMap.erase(it);
	}
	else
		O3D_ERROR(E_InvalidOperation("This thread can't be unregistered"));
}

Bool EvtManager::isThreadRegistered(Thread * _pThread) const
{
	FastMutexLocker lLocker(m_mutex);

	CIT_PoolMap it = m_poolMap.find(_pThread);
	if (it != m_poolMap.end())
		return True;
	else
		return False;
}

UInt32 EvtManager::processEvent()
{
	const UInt32 lCurrentThreadId = ThreadManager::getThreadId();

	T_EventList lList;

	{
		FastMutexLocker lLocker(m_mutex);

		IT_PoolMap itMap = getPoolMap(lCurrentThreadId);

		lList = itMap->second;
		itMap->second.clear();

		// reset main message state if necessary
		if ((lCurrentThreadId == ThreadManager::getMainThreadId()) &&
            m_isAutoWakeUp &&
            m_mainMessage)
		{
            m_mainMessage = False;
		}
	}

	for (IT_EventList itList = lList.begin() ; itList != lList.end() ; itList++)
	{
        O3D_ASSERT((ThreadManager::getMainThreadId() == lCurrentThreadId) ||
                   (((*itList)->getThread() != nullptr) && ((*itList)->getThread()->getThreadID() == lCurrentThreadId)));

		(*itList)->process();
		deletePtr(*itList);
	}

	return UInt32(lList.size());
}

UInt32 EvtManager::processEvent(Thread * _pThread)
{
	T_EventList lList;

	{
		FastMutexLocker lLocker(m_mutex);

		IT_PoolMap itMap = m_poolMap.find(_pThread);

		if (itMap == m_poolMap.end())
			O3D_ERROR(E_InvalidOperation("The thread is not registered"));

		lList = itMap->second;
		itMap->second.clear();

		// reset main message state if necessary
        if ((_pThread == nullptr) &&
            m_isAutoWakeUp &&
            m_mainMessage)
		{
            m_mainMessage = False;
		}
	}

	for (IT_EventList itList = lList.begin() ; itList != lList.end() ; itList++)
	{
		(*itList)->process();
		deletePtr(*itList);
	}

	return UInt32(lList.size());
}

UInt32 EvtManager::processEvent(EvtHandler * _pHandler)
{
    O3D_ASSERT(_pHandler != nullptr);

	const UInt32 lCurrentThreadId = ThreadManager::getThreadId();

	T_EventList lList;
	{
		FastMutexLocker lLocker(m_mutex);

		T_EventList & lListCpy = getPoolMap(lCurrentThreadId)->second;
		IT_EventList it = lListCpy.begin();

		while (it != lListCpy.end())
		{
			if ((*it)->getReceiver() == _pHandler)
			{
				lList.push_back(*it);
				it = lListCpy.erase(it);
			}
			else
				it++;
		}

		// reset main message state if necessary
		if ((lCurrentThreadId == ThreadManager::getMainThreadId()) &&
            m_isAutoWakeUp &&
            m_mainMessage)
		{
            m_mainMessage = False;
		}
	}

	for(IT_EventList itList = lList.begin() ; itList != lList.end() ; itList++)
	{
		O3D_ASSERT((ThreadManager::getMainThreadId() == lCurrentThreadId) || (((*itList)->getThread() != NULL) && ((*itList)->getThread()->getThreadID() == lCurrentThreadId)));

		(*itList)->process();
		deletePtr(*itList);
	}

	return UInt32(lList.size());
}


UInt32 EvtManager::processEvent(EvtHandler * _pHandler, Thread * _pThread)
{
    O3D_ASSERT(_pHandler != nullptr);

	T_EventList lList;
	{
		FastMutexLocker lLocker(m_mutex);

		IT_PoolMap itMap = m_poolMap.find(_pThread);

		if (itMap == m_poolMap.end())
			O3D_ERROR(E_InvalidOperation("The thread is not registered"));

		T_EventList & lListCpy = itMap->second;
		IT_EventList it = lListCpy.begin();

		while (it != lListCpy.end())
		{
			if ((*it)->getReceiver() == _pHandler)
			{
				lList.push_back(*it);
				it = lListCpy.erase(it);
			}
			else
				it++;
		}

		// reset main message state if necessary
        if ((_pThread == nullptr) &&
            m_isAutoWakeUp &&
            m_mainMessage)
		{
            m_mainMessage = False;
		}
	}

	for(IT_EventList itList = lList.begin() ; itList != lList.end() ; itList++)
	{
		(*itList)->process();
		deletePtr(*itList);
	}

	return UInt32(lList.size());
}

void EvtManager::deletePendingEvents()
{
	FastMutexLocker lLocker(m_mutex);

	T_EventList & lList = getPoolMap(ThreadManager::getThreadId())->second;

	for (IT_EventList it = lList.begin() ; it != lList.end() ; it++)
		deletePtr(*it);

	lList.clear();
}

void EvtManager::deletePendingEvents(EvtHandler* _pHandler)
{
    O3D_ASSERT(_pHandler != nullptr);

	FastMutexLocker lLocker(m_mutex);

	for (IT_PoolMap itMap = m_poolMap.begin() ; itMap != m_poolMap.end() ; itMap++)
	{
		T_EventList & lList = itMap->second;

		IT_EventList it = lList.begin();

		while (it != lList.end())
		{
			if ((*it)->getReceiver() == _pHandler)
			{
				deletePtr(*it);
				it = lList.erase(it);
			}
			else
				it++;
		}
	}
}

Bool EvtManager::isPendingEvent() const
{
	FastMutexLocker lLocker(m_mutex);

	return !(getPoolMap(ThreadManager::getThreadId())->second.empty());
}

Bool EvtManager::isPendingEvent(Thread * _pThread) const
{
	if (isThreadRegistered(_pThread))
	{
		FastMutexLocker lLocker(m_mutex);
        const UInt32 lThreadId = (_pThread == nullptr ? ThreadManager::getMainThreadId() : _pThread->getThreadID());

		return !(getPoolMap(lThreadId)->second.empty());
	}
	else
		return False;
}

Bool EvtManager::isPendingEvent(EvtHandler * _pHandler) const
{
    O3D_ASSERT(_pHandler != nullptr);

	FastMutexLocker lLocker(m_mutex);

	const T_EventList & lListCpy = getPoolMap(ThreadManager::getThreadId())->second;

	for (CIT_EventList it = lListCpy.begin() ; it != lListCpy.end() ; it++)
		if ((*it)->getReceiver() == _pHandler)
			return True;

	return False;
}

Bool EvtManager::isPendingEvent(EvtHandler * _pHandler, Thread * _pThread) const
{
    O3D_ASSERT(_pHandler != nullptr);

	if (isThreadRegistered(_pThread))
	{
		FastMutexLocker lLocker(m_mutex);

        const UInt32 lThreadId = (_pThread == nullptr ? ThreadManager::getMainThreadId() : _pThread->getThreadID());
		const T_EventList & lListCpy = getPoolMap(lThreadId)->second;

		for (CIT_EventList it = lListCpy.begin() ; it != lListCpy.end() ; it++)
			if ((*it)->getReceiver() == _pHandler)
				return True;
	}

	return False;
}

EvtManager::IT_PoolMap EvtManager::getPoolMap(UInt32 _threadId)
{
	if (_threadId == ThreadManager::getMainThreadId())
	{
        IT_PoolMap itMap = m_poolMap.find((Thread*)nullptr);

		if (itMap == m_poolMap.end())
			O3D_ERROR(E_InvalidOperation("The main thread is not registered in the EvtManager"));
		return itMap;
	}
	else
	{
		IT_PoolMap itMap = m_poolMap.begin();

		for (; itMap != m_poolMap.end() ; itMap++)
		{
            if ((itMap->first != nullptr) && (itMap->first->getThreadID() == _threadId))
			{
				break;
			}
		}

		if (itMap == m_poolMap.end())
			O3D_ERROR(E_InvalidOperation(String("The thread <") << _threadId << "> is not registered in the EvtManager"));
		else
			return itMap;
	}

	return m_poolMap.end();
}

EvtManager::CIT_PoolMap EvtManager::getPoolMap(UInt32 _threadId) const
{
	return const_cast<EvtManager*>(this)->getPoolMap(_threadId);
}

// Enable PostMessage for the main thread
void EvtManager::enableAutoWakeUp()
{
	FastMutexLocker lLocker(m_mutex);
    m_isAutoWakeUp = True;
}

// Disable PostMessage for the main thread
void EvtManager::disableAutoWakeUp()
{
	FastMutexLocker lLocker(m_mutex);
    m_isAutoWakeUp = False;
}

// Get the state of PostMessage for the main thread
Bool EvtManager::isAutoWakeUp() const
{
	Bool ret = False;
	{
		FastMutexLocker lLocker(m_mutex);
        ret = m_isAutoWakeUp;
	}
	return ret;
}

EvtPool::EvtPool():
    m_pThread(nullptr),
	m_acquired(False)
{
}

EvtPool::EvtPool(Thread * _pThread):
	m_pThread(_pThread),
	m_acquired(!EvtManager::instance()->isThreadRegistered(_pThread))
{
	if (m_acquired)
		EvtManager::instance()->registerThread(m_pThread);
}

EvtPool::~EvtPool()
{
	if (m_acquired)
	{
		while (EvtManager::instance()->processEvent() > 0) {}
		EvtManager::instance()->unRegisterThread(m_pThread);
	}
}

void EvtPool::init(Thread * _pThread)
{
	if (m_acquired)
		EvtManager::instance()->unRegisterThread(m_pThread);

	m_pThread = _pThread;
	m_acquired = EvtManager::instance()->isThreadRegistered(_pThread);
	EvtManager::instance()->registerThread(m_pThread);
}

void EvtPool::processEvent()
{
	EvtManager::instance()->processEvent();
}


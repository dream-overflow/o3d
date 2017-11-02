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

#include "o3d/core/debug.h"

using namespace o3d;

Bool Timer::m_enabled = True;

// Destructor
Timer::~Timer()
{
	destroy();
}

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
		m_instance->destroy();
    }

	deletePtr(m_instance);
}

//---------------------------------------------------------------------------------------
// class O3DTimerManager
//---------------------------------------------------------------------------------------

// constructor
TimerManager::TimerManager() :
    TemplateManager<Timer>(nullptr)
{
	m_instance = (TimerManager*)this;
}

// add a new timer callback and return it's new Id
Int32 TimerManager::addTimer(
	UInt32 timeout,
	Timer::TimerMode mode,
	Callback *callback,
	void *data)
{
	FastMutexLocker locker(m_mutex);

	Timer* pTimer = new Timer(timeout, mode, callback, data);
    if (pTimer) {
		return addElement(pTimer);
    }

	return -1;
}

// remove a timer by it's Id
void TimerManager::deleteTimer(Int32 Id)
{
	FastMutexLocker locker(m_mutex);
	deleteElement(Id);
}

// Get a timer by it's Id
Timer* TimerManager::getTimer(Int32 Id)
{
	FastMutexLocker locker(m_mutex);
	return get(Id);
}

// Get timer by its system handle
Timer* TimerManager::getTimerInternal(_Timer handle)
{
	FastMutexLocker locker(m_mutex);

	stdext::hash_map<_Timer,Timer*>::iterator it = m_handlesMap.find(handle);
    if (it != m_handlesMap.end()) {
		return it->second;
    } else {
        return nullptr;
    }
}

/**
 * @file taskmanager.cpp
 * @brief Implementation of TaskManager.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2009-10-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/taskmanager.h"
#include "o3d/core/debug.h"

#include <algorithm>

using namespace o3d;

TaskManager* TaskManager::m_instance = NULL;

// Singleton instantiation
TaskManager* TaskManager::instance()
{
	if (!m_instance)
		m_instance = new TaskManager();
	return m_instance;
}

// Singleton destruction
void TaskManager::destroy()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

// Default constructor
TaskManager::TaskManager() :
	m_maxTask(5)
{
	m_instance = (TaskManager*)this; // Used to avoid recursive call when the ctor call himself...

	// connect himself to the delete task signal
    onDeleteTask.connect(this, &TaskManager::deleteTask, EvtHandler::CONNECTION_ASYNCH);
    onFinalizeTask.connect(this, &TaskManager::finalizeTask, EvtHandler::CONNECTION_ASYNCH);
}

// Destructor
TaskManager::~TaskManager()
{
	m_mutex.lock();

	// don't wait for any events, so disconnect from all
	disconnect();

	// delete pending task (we don't want they occur)
	for (IT_TaskList it = m_pendingTasksList.begin(); it != m_pendingTasksList.end(); ++it)
	{
		deletePtr(*it);
	}

	m_pendingTasksList.clear();

	// wait for the end of any running task
	for (IT_TaskList it = m_runningTasksList.begin(); it != m_runningTasksList.end(); ++it)
	{
		(*it)->waitFinish();

		// and finally delete it directly
		deletePtr(*it);
	}

	m_runningTasksList.clear();

	m_mutex.unlock();
}

// Add a task to process. A new task is started if tasks slots are available
void TaskManager::addTask(Task *task)
{
	O3D_ASSERT(task);
	if (!task)
		return;

	RecurMutexLocker locker(m_mutex);

	// to start the next pending task at finish, for finalize and next for auto deletion
    task->onTaskFinished.connect(this, &TaskManager::taskFinished, EvtHandler::CONNECTION_ASYNCH);

	// or when the task execution failed, call for its auto deletion without finalize
    task->onTaskFailed.connect(this, &TaskManager::taskFailed, EvtHandler::CONNECTION_ASYNCH);

	// process immediately by this thread
	if (m_maxTask == 0)
	{
		m_runningTasksList.push_back(task);

		// process the task
		task->start(False);
	}
	else
	{
		// empty slot ?
		if (m_runningTasksList.size() < m_maxTask)
		{
			m_runningTasksList.push_back(task);

			// and start it
			task->start(True);
		}
		else
		{
			// pending queue
			m_pendingTasksList.push_back(task);
		}
	}
}

// Define the number of maximum simultaneous running tasks.
void TaskManager::setNumMaxTasks(UInt32 max)
{
	RecurMutexLocker locker(m_mutex);

	m_maxTask = max;

	// start many others tasks as possible
	while ((m_runningTasksList.size() < m_maxTask) && !m_pendingTasksList.empty())
	{
		Task *task = m_pendingTasksList.front();
		m_pendingTasksList.pop_front();

		m_runningTasksList.push_back(task);

		// and start it
		task->start(True);
	}
}

// Get the current number of running tasks.
UInt32 TaskManager::getNumRunningTasks() const
{
	RecurMutexLocker locker(const_cast<RecursiveMutex&>(m_mutex));

	Int32 size = m_runningTasksList.size();
	return size;
}

// Kills any running tasks.
void TaskManager::kill()
{
	RecurMutexLocker locker(m_mutex);

	for (IT_TaskList it = m_runningTasksList.begin(); it != m_runningTasksList.end(); ++it)
	{
		(*it)->kill();
		deletePtr(*it);
	}

	m_runningTasksList.clear();

	for (IT_TaskList it = m_pendingTasksList.begin(); it != m_pendingTasksList.end(); ++it)
	{
		deletePtr(*it);
	}

	m_pendingTasksList.clear();
}

// When a task execution is not a success it call for its deletion.
void TaskManager::taskFailed(Task* task)
{
	O3D_ASSERT(task);

	if (task)
	{
		RecurMutexLocker locker(m_mutex);

		IT_TaskList it = std::find(m_runningTasksList.begin(), m_runningTasksList.end(), task);
		if (it != m_runningTasksList.end())
		{
			m_runningTasksList.erase(it);
			onDeleteTask(task);
		}
		else
			O3D_ERROR(E_InvalidParameter("Unknown running task"));
	}

	// start a new task if necessary
	if (m_runningTasksList.size() < m_maxTask)
	{
		if (!m_pendingTasksList.empty())
		{
			Task *task = m_pendingTasksList.front();
			m_pendingTasksList.pop_front();

			m_runningTasksList.push_back(task);

			// and start it
			task->start(True);
		}
	}
}

// When a task is finished to process it call for its finalize.
void TaskManager::taskFinished(Task* task)
{
	O3D_ASSERT(task);

	if (task)
		onFinalizeTask(task);
}

// When a task is finalized it call for its deletion.
void TaskManager::finalizeTask(Task* task)
{
	O3D_ASSERT(task);

	if (task)
	{
		// finalize
		task->synchronize();

		RecurMutexLocker locker(m_mutex);

		IT_TaskList it = std::find(m_runningTasksList.begin(), m_runningTasksList.end(), task);
		if (it != m_runningTasksList.end())
		{
			m_runningTasksList.erase(it);
			onDeleteTask(task);
		}
		else
			O3D_ERROR(E_InvalidParameter("Unknown running task"));
	}

	// start a new task if necessary
	if (m_runningTasksList.size() < m_maxTask)
	{
		if (!m_pendingTasksList.empty())
		{
			Task *task = m_pendingTasksList.front();
			m_pendingTasksList.pop_front();

			m_runningTasksList.push_back(task);

			// and start it
			task->start(True);
		}
	}
}

// When delete a task.
void TaskManager::deleteTask(Task* task)
{
	O3D_ASSERT(task);

	if (task)
		o3d::deletePtr(task);
}


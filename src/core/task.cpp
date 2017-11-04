/**
 * @file task.cpp
 * @brief Implementation of Task.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2009-10-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/task.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// Task
//---------------------------------------------------------------------------------------

Task::Task() :
	m_priority(Thread::PRIORITY_NORMAL),
	m_cpuAffinity(-1),
	m_thread(this),
	m_executed(False)
{
}

Task::~Task()
{
	// wait if running
    if (m_thread.isThread()) {
		m_thread.waitFinish();
    }
}

// Start the task.
void Task::start(Bool asynchronous)
{
    if (asynchronous) {
        if (!m_thread.isThread()) {
			m_thread.start();

			// thread priority
            if (m_priority != Thread::PRIORITY_NORMAL) {
				m_thread.setPriority(m_priority);
            }

			// thread CPU affinity
            if (m_cpuAffinity != -1) {
				std::list<UInt32> affinity;
				affinity.push_back(m_cpuAffinity);

				m_thread.setCPUAffinity(affinity);
			}
		}
    } else {
        run(nullptr);
	}
}

// Force to kill the task
void Task::kill()
{
    if (m_thread.isThread()) {
		m_thread.kill();
    }
}

// Is the task is currently running.
void Task::waitFinish()
{
	m_thread.waitFinish();
}

Int32 Task::run(void *param)
{
	// perform the task
	m_executed = execute();

	if (m_executed)
		onTaskFinished(this);
	else
		onTaskFailed(this);

	return 0;
}

// Finalize the task.
void Task::synchronize()
{
	if (m_executed)
	{
		finalize();
		onTaskFinalized(this);
	}
}


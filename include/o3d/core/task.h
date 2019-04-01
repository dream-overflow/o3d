/**
 * @file task.h
 * @brief Task object. A specialized task must inherit from this object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2009-10-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TASK_H
#define _O3D_TASK_H

#include "runnable.h"
#include "evt.h"
#include "evthandler.h"
#include "thread.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class Task
//-------------------------------------------------------------------------------------
//! Task object. A specialized task must inherit from this object.
//---------------------------------------------------------------------------------------
class O3D_API Task : public EvtHandler, public Runnable
{
	friend class TaskManager;

public:

	//! Default constructor.
	Task();

	//! Virtual destructor.
	virtual ~Task();

	//! Force to kill the task.
	void kill();

	//! Wait the task to finish.
	void waitFinish();

	//! Is the task was perform.
	inline Bool isExecuted() const { return m_executed; }

	//! Define the task thread priority. Default is NORMAL.
	void setPriority(Thread::ThreadPriority priority) { m_priority = priority; }

	//! Get the task thread priority.
	Thread::ThreadPriority getPriority() const { return m_priority; }

	//! Define the task thread CPU affinity. Default is not assigned.
	void setCPUAffinity(Int32 cpuAffinity) { m_cpuAffinity = cpuAffinity; }

	//! Get the task thread CPU affinity. -1 mean not assigned.
	Int32 getCPUAffinity() const { return m_cpuAffinity; }

	//-----------------------------------------------------------------------------------
	// User methods to be override
	//-----------------------------------------------------------------------------------

	//! Override the task processing (can be asynchronous).
	//! @return TRUE if successfully performed.
	virtual Bool execute() = 0;

	//! Override the task finalize (synchronized with the main thread).
	//! @return TRUE if successfully finalized.
	virtual Bool finalize() = 0;

public:

	//! Called when the task is successfully executed.
    Signal<Task*> onTaskFinished{this};

	//! Called when an error occurs during the task execution.
    Signal<Task*> onTaskFailed{this};

	//! Called when task was finalize.
    Signal<Task*> onTaskFinalized{this};

protected:

	//! Start the task. The task manager start the task by this way.
	//! @param asynchronous If TRUE the task is executed asynchronously.
	void start(Bool asynchronous);

	//! Main.
    virtual Int32 run(void *param);

	//! Finalize the task.
	void synchronize();

	Thread::ThreadPriority m_priority;  //!< Thread priority.
	Int32 m_cpuAffinity;            //!< CPU affinity.

	Thread m_thread;      //!< Thread of the task if threaded.
	Bool m_executed;  //!< TRUE when execution was perform.
};

} // namespace o3d

#endif // _O3D_TASK_H

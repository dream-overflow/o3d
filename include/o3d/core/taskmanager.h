/**
 * @file taskmanager.h
 * @brief Manage asynchronous task.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2009-10-05
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TASKMANAGER_H
#define _O3D_TASKMANAGER_H

#include "task.h"

#include <list>

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class TaskManager
//-------------------------------------------------------------------------------------
//! Manage asynchronous task.
//---------------------------------------------------------------------------------------
class O3D_API TaskManager : public EvtHandler
{
public:

	//! Get the singleton instance.
    static TaskManager* instance();
	//! Delete the singleton instance.
    static void destroy();

	//! Add a task to process. The task is started if tasks slots are available,
	//! otherwise the task is put in the pending list, waiting for a slot.
	//! @param task The task to process.
	void addTask(Task *task);

	//! Define the number of maximum simultaneous running tasks.
	//! @param max If 0 task are processed immediately. If there is one or many pending task,
	//! so they would be processed now.
	void setNumMaxTasks(UInt32 max);

	//! Get the current number of running tasks.
	UInt32 getNumRunningTasks() const;

	//! Kills any running tasks and abort any pending tasks.
	void kill();

public:

	//! Emitted on task is finished and finalized to delete it on the next loop.
	//! The param contain the processed task.
    Signal<Task*> onDeleteTask{this};

	//! Emitted on task is finished for synchronize.
	//! The param contain the processed task.
    Signal<Task*> onFinalizeTask{this};

private:

	UInt32 m_maxTask;         //!< Number of maximum simultaneous running tasks.
	RecursiveMutex m_mutex;

	typedef std::list<Task*> T_TaskList;
	typedef T_TaskList::iterator IT_TaskList;
	typedef T_TaskList::const_iterator CIT_TaskList;

	T_TaskList m_runningTasksList;
	T_TaskList m_pendingTasksList;

	//! When a task execution failed it call for its deletion.
    void taskFailed(Task* task);

	//! When a task is finished to process it call for its finalize.
    void taskFinished(Task* task);

	//! When a task is finalized it call for its deletion.
    void finalizeTask(Task* task);

	//! When delete a task.
    void deleteTask(Task* task);

	//-----------------------------------------------------------------------------------
	// Singleton
	//-----------------------------------------------------------------------------------

    static TaskManager* m_instance;

	//! Restricted default constructor.
    TaskManager();
	//! Restricted destructor.
    virtual ~TaskManager();
	//! Non copyable.
    TaskManager(const TaskManager &dup);
	//! Non assignable.
    TaskManager& operator=(const TaskManager &dup);
};

} // namespace o3d

#endif // _O3D_TASKMANAGER_H


/**
 * @file scheduledthreadpool.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCHEDULEDTHREADPOOL_H
#define _O3D_SCHEDULEDTHREADPOOL_H

#include "threadfactory.h"

namespace o3d {

/**
 * @brief ScheduledRunnable
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-11-28
 */
class O3D_API ScheduledRunnable : public Runnable
{
public:

    ScheduledRunnable();

    virtual ~ScheduledRunnable();

    void setTimestamp(Int64 timestamp);

    void setRunnable(Runnable *runnable);

    /**
     * @brief setDelay
     * @param delay Delay before next run, in microseconds.
     */
    void setDelay(Int64 delay);

    /**
     * @brief getTimestamp
     * @return Scheduled timestamp in microseconds.
     */
    Int64 getTimestamp() const;

    /**
     * @brief getDelay
     * @return Delay before next run, in microseconds.
     */
    Int64 getDelay() const;

    /**
     * @brief cancel Cancel.
     */
    void cancel();

    /**
     * @brief isCanceled
     * @return True if previously canceled.
     */
    Bool isCanceled() const;

    /**
     * @brief isDone
     * @return True once
     */
    Bool isDone() const;

    virtual Int32 run(void *p);

private:

    FastMutex m_mutex;

    Int64 m_delay;

    Bool m_running;
    Bool m_cancel;
    Bool m_done;

    Runnable *m_runnable;

    Int64 m_timestamp; //!< When the task must be performed
};

/**
 * @brief ScheduledThreadPool
 * Manage a pool of a limited number of thread, and a list of Runnable object
 * to process at regular interval of time.
 * @note Works with a microseconds precision.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-11-27
 */
class O3D_API ScheduledThreadPool
{
public:

    /**
     * @brief ScheduledThreadPool
     * @param numThread Parallels running threads
     * @param threadFactory A specifi thread factory, or use a default in null.
     */
    ScheduledThreadPool(UInt32 numThread, ThreadFactory *threadFactory = nullptr);

    virtual ~ScheduledThreadPool();

    /**
     * @brief schedule Schedule a new task.
     * @param runnable If run returns <0 the task is canceled
     * @param initialDelay Delay before the first execution
     * @param delay Delay between each execution
     */
    void schedule(Runnable *runnable,
            Int32 initialDelay,
            Int32 delay,
            TimeUnit unit = TIME_MILLISECOND);

    //! Set as daemon. If true, thread are not terminated at destructor.
    void setDaemon(Bool daemon);

    //! Is set as daemon
    Bool isDaemon() const;

    //! Kills any running tasks withou waiting the end of every thread.
    void kill();

    //! Terminate. Send a cancel signal to every thread, and wait them to finish normaly.
    void terminate();

    ScheduledRunnable *getNextTask();

    //! Wait for new task signal.
    void waitForNewTask();

    //! Poll for the next running tasks.
    void poll();

    //! Reinject a task for a further execution.
    void put(ScheduledRunnable *task);

    //! Is threads running
    Bool isRunning() const;

    //! Is terminate signal.
    Bool isTerminate() const;

private:

    class Scheduler : public Runnable
    {
    public:

        Scheduler(ScheduledThreadPool *pool);

        virtual Int32 run(void *p);

    private:

        FastMutex m_mutex;

        ScheduledThreadPool *m_pool;
        ScheduledRunnable *m_task;
    };

    RecursiveMutex m_mutex;
    Semaphore m_waitingTask;

    Bool m_daemon;
    Bool m_running;
    Bool m_terminate;

    typedef std::list<ScheduledRunnable*> T_ScheduledRunnableList;
    typedef T_ScheduledRunnableList::iterator IT_ScheduledRunnableList;
    typedef T_ScheduledRunnableList::const_iterator CIT_ScheduledRunnableList;

    T_ScheduledRunnableList m_pendingTasks;
    T_ScheduledRunnableList m_pickedTasks;

    typedef std::list<Thread*> T_ThreadList;
    typedef T_ThreadList::iterator IT_ThreadList;
    typedef T_ThreadList::const_iterator CIT_ThreadList;

    T_ThreadList m_threads;
};

} // namespace o3d

#endif // _O3D_SCHEDULEDTHREADPOOL_H


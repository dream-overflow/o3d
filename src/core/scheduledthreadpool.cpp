/**
 * @file scheduledthreadpool.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/scheduledthreadpool.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/time.h"

using namespace o3d;

ScheduledThreadPool::ScheduledThreadPool(UInt32 numThread, ThreadFactory *threadFactory) :
    m_daemon(False),
    m_running(False),
    m_terminate(False)
{
    AutoPtr<DefaultThreadFactory> tf;

    if (threadFactory == nullptr)
    {
        tf = new DefaultThreadFactory;
        threadFactory = (ThreadFactory*)tf.get();
    }

    // create
    for (UInt32 i = 0; i < numThread; ++i)
    {
        Thread *thread = threadFactory->createThread(new Scheduler(this));
        m_threads.push_back(thread);
    }

    // start
    m_running = True;
    for (Thread *thread : m_threads)
    {
        thread->start(nullptr);
    }
}

ScheduledThreadPool::~ScheduledThreadPool()
{
    RecurMutexLocker locker(m_mutex);
    if (m_daemon)
        return;

    locker.unlock();
    terminate();
    locker.relock();

    // stop signal...
    m_running = False;

    // wake up
    m_waitingTask.postSignal();

    // join...
    while (!m_threads.empty())
    {
        locker.unlock();
        m_threads.front()->waitFinish();
        // cleanup scheduler
        deletePtr(m_threads.front()->getRunnable());
        // and thread
        deletePtr(m_threads.front());
        m_threads.pop_front();
        locker.relock();
    }
}

void ScheduledThreadPool::schedule(Runnable *runnable,
        Int32 initialDelay,
        Int32 delay,
        TimeUnit unit)
{
    RecurMutexLocker locker(m_mutex);

    ScheduledRunnable *newTask = new ScheduledRunnable;

    newTask->setTimestamp(System::getTime(TIME_MICROSECOND) + Time::toMicros<Int64>(initialDelay, unit));
    newTask->setRunnable(runnable);
    newTask->setDelay(Time::toMicros<Int64>(delay, unit));

    if (m_pendingTasks.empty())
    {
        m_pendingTasks.push_back(newTask);

        locker.unlock();

        // send a signal to consumers
        m_waitingTask.postSignal();

        return;
    }

    for (IT_ScheduledRunnableList it = m_pendingTasks.begin(); it != m_pendingTasks.end(); ++it)
    {
        ScheduledRunnable *task = *it;
        if (task->getTimestamp() > newTask->getTimestamp())
        {
            // insert before a task with a later timestamp
            m_pendingTasks.insert(it, newTask);

            locker.unlock();

            // send a signal to consumers
            m_waitingTask.postSignal();

            return;
        }
    }

    m_pendingTasks.push_back(newTask);

    locker.unlock();

    // send a signal to consumers
    m_waitingTask.postSignal();
}

void ScheduledThreadPool::setDaemon(Bool daemon)
{
    RecurMutexLocker locker(m_mutex);
    m_daemon = daemon;
}

Bool ScheduledThreadPool::isDaemon() const
{
    RecurMutexLocker locker(m_mutex);
    return m_daemon;
}

void ScheduledThreadPool::kill()
{
    for (Thread *thread : m_threads)
    {
        // wake up
        m_waitingTask.postSignal();
        // and kill
        thread->kill();
    }

    for (ScheduledRunnable *scheduled : m_pendingTasks)
    {
        deletePtr(scheduled);
    }

    for (ScheduledRunnable *scheduled : m_pickedTasks)
    {
        deletePtr(scheduled);
    }

    m_pendingTasks.clear();
    m_pickedTasks.clear();
}

void ScheduledThreadPool::terminate()
{
    RecurMutexLocker locker(m_mutex);
    if (m_daemon)
        return;

    // terminate signal
    m_terminate = True;

    for (ScheduledRunnable *scheduled : m_pendingTasks)
    {
        m_mutex.unlock();
        scheduled->cancel();
        m_mutex.lock();
    }

    for (ScheduledRunnable *scheduled : m_pickedTasks)
    {
        m_mutex.unlock();
        scheduled->cancel();
        m_mutex.lock();
    }
/*  if a task is running, it can finish until the destructor or kill
    while (!m_pendingTasks.empty() || !m_pickedTasks.empty())
    {
        m_mutex.unlock();
        System::waitMs(10);
        m_mutex.lock();
    }*/

    for (ScheduledRunnable *scheduled : m_pendingTasks)
    {
        deletePtr(scheduled);
    }

    for (ScheduledRunnable *scheduled : m_pickedTasks)
    {
        deletePtr(scheduled);
    }

    m_pendingTasks.clear();
    m_pickedTasks.clear();

    m_terminate = False;
}

ScheduledRunnable *ScheduledThreadPool::getNextTask()
{
    RecurMutexLocker locker(m_mutex);
    if (m_pickedTasks.empty())
        return nullptr;

    ScheduledRunnable *task = m_pickedTasks.front();
    m_pickedTasks.pop_front();

    return task;
}

void ScheduledThreadPool::waitForNewTask()
{
    m_waitingTask.waitSignal(100);
}

void ScheduledThreadPool::poll()
{
    RecurMutexLocker locker(m_mutex);

    // no task to perform
    if (m_pendingTasks.empty())
        return;

    ScheduledRunnable *task = m_pendingTasks.front();
    Int64 time = System::getTime(TIME_MICROSECOND);

    while (task->getTimestamp() <= time)
    {
        m_pendingTasks.pop_front();
        m_pickedTasks.push_back(task);

        if (!m_pendingTasks.empty())
            task = m_pendingTasks.front();
        else
            break;
    }

    if (m_pickedTasks.empty())
        return;

    locker.unlock();

    // send a signal to consumers
    m_waitingTask.postSignal();
}

void ScheduledThreadPool::put(ScheduledRunnable *putTask)
{
    Int64 timestamp = System::getTime(TIME_MICROSECOND) + putTask->getDelay();
    putTask->setTimestamp(timestamp);

    {
        RecurMutexLocker locker(m_mutex);

        if (!m_running) {
            return;
        }

        if (putTask->isCanceled()) {
            return;
        }

        if (m_pendingTasks.empty()) {
            m_pendingTasks.push_back(putTask);

            // send a signal to consumers
            m_waitingTask.postSignal();

            return;
        }

        for (IT_ScheduledRunnableList it = m_pendingTasks.begin(); it != m_pendingTasks.end(); ++it) {
            ScheduledRunnable *task = *it;
            if (task->getTimestamp() > putTask->getTimestamp()) {
                // insert before a task with a later timestamp
                m_pendingTasks.insert(it, putTask);

                // send a signal to consumers
                m_waitingTask.postSignal();

                return;
            }
        }

        m_pendingTasks.push_back(putTask);

        // send a signal to consumers
        m_waitingTask.postSignal();
    }
}

Bool ScheduledThreadPool::isRunning() const
{
    RecurMutexLocker locker(m_mutex);
    return m_running;
}

Bool ScheduledThreadPool::isTerminate() const
{
    RecurMutexLocker locker(m_mutex);
    return m_terminate;
}

ScheduledRunnable::ScheduledRunnable() :
    m_delay(0),
    m_running(False),
    m_cancel(False),
    m_done(False),
    m_runnable(nullptr),
    m_timestamp(0)
{
}

ScheduledRunnable::~ScheduledRunnable()
{
    deletePtr(m_runnable);
}

void ScheduledRunnable::setTimestamp(Int64 timestamp)
{
    FastMutexLocker locker(m_mutex);
    m_timestamp = timestamp;
}

void ScheduledRunnable::setRunnable(Runnable *runnable)
{
    FastMutexLocker locker(m_mutex);
    m_runnable = runnable;
}

void ScheduledRunnable::setDelay(Int64 delay)
{
    FastMutexLocker locker(m_mutex);
    m_delay = delay;
}

Int64 ScheduledRunnable::getTimestamp() const
{
    FastMutexLocker locker(m_mutex);
    return m_timestamp;
}

void ScheduledRunnable::cancel()
{
    FastMutexLocker locker(m_mutex);
    m_cancel = True;
}

Bool ScheduledRunnable::isCanceled() const
{
    FastMutexLocker locker(m_mutex);
    return m_cancel;
}

Bool ScheduledRunnable::isDone() const
{
    FastMutexLocker locker(m_mutex);
    return m_done;
}

Int64 ScheduledRunnable::getDelay() const
{
    FastMutexLocker locker(m_mutex);
    return m_delay;
}

Int32 ScheduledRunnable::run(void *p)
{
    m_done = False;

    if (m_cancel)
        return 0;

    m_running = True;

    Int32 res = m_runnable->run(p);

    if (res < 0)
        m_cancel = True;

    m_done = True;
    m_running = False;

    return res;
}

ScheduledThreadPool::Scheduler::Scheduler(ScheduledThreadPool *pool) :
    m_pool(pool),
    m_task(nullptr)
{
}

Int32 ScheduledThreadPool::Scheduler::run(void *p)
{
    Bool run = False;
    Bool terminate = False;

    m_mutex.lock();
    run = m_pool->isRunning();

    while (run) {
        m_mutex.unlock();

        m_pool->poll();
        m_task = m_pool->getNextTask();

        // running
        if (m_task) {
            // run
            Int32 r = m_task->run(p);

            // if not canceled do it again
            if ((r >= 0) && !m_task->isCanceled() && !terminate) {
                m_pool->put(m_task);
            } else {
                deletePtr(m_task);
            }

            m_task = nullptr;
        } else {
            // wait for a task during 100 ms
            m_pool->waitForNewTask();
        }

        m_mutex.lock();
        run = m_pool->isRunning();
        terminate = m_pool->isTerminate();
    }
    m_mutex.unlock();

    return 0;
}

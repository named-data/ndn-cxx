/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "scheduler.h"
#include "one-time-task.h"
#include "periodic-task.h"
#include "logging.h"

#include <utility>
#include <boost/make_shared.hpp>

INIT_LOGGER ("Scheduler");

using namespace std;
using namespace boost;

#define EVLOOP_NO_EXIT_ON_EMPTY 0x04

static void
dummyCallback(evutil_socket_t fd, short what, void *arg)
{
  // 1 year later, that was a long run for the app
  // let's wait for another year
  timeval tv;
  tv.tv_sec = 365 * 24 * 3600;
  tv.tv_usec = 0;
  event *ev = *(event **)arg;
  int res = evtimer_add(ev, &tv);
}

// IntervalGeneratorPtr
// IntervalGenerator:: Null;

void errorCallback(int err)
{
  _LOG_ERROR ("Fatal error: " << err);
}

Scheduler::Scheduler()
  : m_running(false)
  , m_executor(1)
{
  event_set_fatal_callback(errorCallback);
  evthread_use_pthreads();
  m_base = event_base_new();

  // This is a hack to prevent event_base_loop from exiting;
  // the flag EVLOOP_NO_EXIT_ON_EMPTY is somehow ignored, at least on Mac OS X
  // it's going to be scheduled to 10 years later
  timeval tv;
  tv.tv_sec = 365 * 24 * 3600;
  tv.tv_usec = 0;
  m_ev = evtimer_new(m_base, dummyCallback, &m_ev);
  int res = evtimer_add(m_ev, &tv);
  if (res < 0)
  {
    _LOG_ERROR("heck");
  }
}

Scheduler::~Scheduler()
{
  shutdown ();
  evtimer_del(m_ev);
  event_free(m_ev);
  event_base_free(m_base);
}

void
Scheduler::eventLoop()
{
  while(true)
  {
    if (event_base_loop(m_base, EVLOOP_NO_EXIT_ON_EMPTY) < 0)
    {
      _LOG_DEBUG ("scheduler loop break error");
    }
    else
    {
      _LOG_DEBUG ("scheduler loop break normal");
    }

    {
      ScopedLock lock(m_mutex);
      if (!m_running)
        {
          _LOG_DEBUG ("scheduler loop break normal");
          break;
        }
    }

    // just to prevent craziness in CPU usage which supposedly should not happen
    // after adding the dummy event
    usleep(1000);
  }
}

void
Scheduler::execute(Executor::Job job)
{
  m_executor.execute(job);
}

void
Scheduler::start()
{
  ScopedLock lock(m_mutex);
  if (!m_running)
  {
    m_thread = boost::thread(&Scheduler::eventLoop, this);
    m_executor.start();
    m_running = true;
  }
}

void
Scheduler::shutdown()
{
  bool breakAndWait = false;
  {
    ScopedLock lock (m_mutex);
    if (m_running)
      {
        m_running = false;
        breakAndWait = true;
      }
  }

  if (breakAndWait)
    {
      event_base_loopbreak(m_base);
      m_executor.shutdown();
      m_thread.join();
    }
}

TaskPtr
Scheduler::scheduleOneTimeTask (SchedulerPtr scheduler, double delay,
                                const Task::Callback &callback, const Task::Tag &tag)
{
  TaskPtr task = make_shared<OneTimeTask> (callback, tag, scheduler, delay);
  if (scheduler->addTask (task))
    return task;
  else
    return TaskPtr ();
}

TaskPtr
Scheduler::schedulePeriodicTask (SchedulerPtr scheduler, IntervalGeneratorPtr delayGenerator,
                                 const Task::Callback &callback, const Task::Tag &tag)
{
  TaskPtr task = make_shared<PeriodicTask> (callback, tag, scheduler, delayGenerator);

  if (scheduler->addTask (task))
    return task;
  else
    return TaskPtr ();
}

bool
Scheduler::addTask(TaskPtr newTask, bool reset/* = true*/)
{
  if (addToMap(newTask))
  {
    if (reset)
      {
        newTask->reset();
      }
    int res = evtimer_add(newTask->ev(), newTask->tv());
    if (res < 0)
    {
      _LOG_ERROR ("evtimer_add failed for " << newTask->tag());
    }
    return true;
  }
  else
  {
    _LOG_ERROR ("fail to add task: " << newTask->tag());
  }

  return false;
}

void
Scheduler::deleteTask(TaskPtr task)
{
  deleteTask (task->tag ());
}

void
Scheduler::rescheduleTask(TaskPtr task)
{
  ScopedLock lock(m_mutex);
  TaskMapIt it = m_taskMap.find(task->tag());
  if (it != m_taskMap.end())
  {
    TaskPtr task = it->second;
    task->reset();
    int res = evtimer_add(task->ev(), task->tv());
    if (res < 0)
    {
      _LOG_ERROR ("evtimer_add failed for " << task->tag());
    }
  }
  else
  {
    addTask(task);
  }
}

void
Scheduler::rescheduleTask(const Task::Tag &tag)
{
  ScopedLock lock(m_mutex);
  TaskMapIt it = m_taskMap.find(tag);
  if (it != m_taskMap.end())
  {
    TaskPtr task = it->second;
    task->reset();
    int res = evtimer_add(task->ev(), task->tv());
    if (res < 0)
    {
      cout << "evtimer_add failed for " << task->tag() << endl;
    }
  }
}

void
Scheduler::rescheduleTaskAt (const Task::Tag &tag, double time)
{
  ScopedLock lock(m_mutex);
  TaskMapIt it = m_taskMap.find (tag);
  if (it != m_taskMap.end())
  {
    TaskPtr task = it->second;
    task->reset();
    task->setTv (time);

    int res = evtimer_add(task->ev(), task->tv());
    if (res < 0)
    {
      _LOG_ERROR ("evtimer_add failed for " << task->tag());
    }
  }
  else
    {
      _LOG_ERROR ("Task for tag " << tag << " not found");
    }
}

void
Scheduler::rescheduleTaskAt (TaskPtr task, double time)
{
  ScopedLock lock(m_mutex);
  TaskMapIt it = m_taskMap.find(task->tag());
  if (it != m_taskMap.end())
  {
    TaskPtr task = it->second;
    task->reset();
    task->setTv (time);

    int res = evtimer_add(task->ev(), task->tv());
    if (res < 0)
    {
      _LOG_ERROR ("evtimer_add failed for " << task->tag());
    }
  }
  else
  {
    task->setTv (time); // force different time
    addTask (task, false);
  }
}


bool
Scheduler::addToMap(TaskPtr task)
{
  ScopedLock lock(m_mutex);
  if (m_taskMap.find(task->tag()) == m_taskMap.end())
  {
    m_taskMap.insert(make_pair(task->tag(), task));
    return true;
  }
  return false;
}

void
Scheduler::deleteTask(const Task::Tag &tag)
{
  ScopedLock lock(m_mutex);
  TaskMapIt it = m_taskMap.find(tag);
  if (it != m_taskMap.end())
  {
    TaskPtr task = it->second;
    evtimer_del(task->ev());
    m_taskMap.erase(it);
  }
}

void
Scheduler::deleteTask(const Task::TaskMatcher &matcher)
{
  ScopedLock lock(m_mutex);
  TaskMapIt it = m_taskMap.begin();
  while(it != m_taskMap.end())
  {
    TaskPtr task = it->second;
    if (matcher(task))
    {
      evtimer_del(task->ev());
      // Use post increment; map.erase invalidate the iterator that is beening erased,
      // but does not invalidate other iterators. This seems to be the convention to
      // erase something from C++ STL map while traversing.
      m_taskMap.erase(it++);
    }
    else
    {
      ++it;
    }
  }
}

int
Scheduler::size()
{
  ScopedLock lock(m_mutex);
  return m_taskMap.size();
}

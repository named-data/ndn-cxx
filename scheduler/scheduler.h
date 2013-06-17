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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <event2/event.h>
#include <event2/thread.h>
#include <event2/event-config.h>
#include <event2/util.h>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/exception/all.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <math.h>
#include <map>
#include <sys/time.h>

#include "scheduler/task.h"
#include "scheduler/interval-generator.h"
#include "executor/executor.h"

class Scheduler;
typedef boost::shared_ptr<Scheduler> SchedulerPtr;

/**
 * @brief Scheduler class
 */
class Scheduler
{
public:
  Scheduler();
  virtual ~Scheduler();

  // start event scheduling
  virtual void
  start();

  // stop event scheduling
  virtual void
  shutdown();

  // helper method to schedule one-time task
  static TaskPtr
  scheduleOneTimeTask (SchedulerPtr scheduler, double delay, const Task::Callback &callback, const Task::Tag &tag);

  // helper method to schedule periodic task
  static TaskPtr
  schedulePeriodicTask (SchedulerPtr scheduler, IntervalGeneratorPtr delayGenerator,
                        const Task::Callback &callback, const Task::Tag &tag);

  // if task with the same tag exists, the task is not added and return false
  virtual bool
  addTask(TaskPtr task, bool reset = true);

  // delete task by task->tag, regardless of whether it's invoked or not
  virtual void
  deleteTask(TaskPtr task);

  // delete task by tag, regardless of whether it's invoked or not
  // if no task is found, no effect
  virtual void
  deleteTask(const Task::Tag &tag);

  // delete tasks by matcher, regardless of whether it's invoked or not
  // this is flexiable in that you can use any form of criteria in finding tasks to delete
  // but keep in mind this is a linear scan

  // if no task is found, no effect
  virtual void
  deleteTask(const Task::TaskMatcher &matcher);

  // task must already have been added to the scheduler, otherwise this method has no effect
  // this is usually used by PeriodicTask
  virtual void
  rescheduleTask(const Task::Tag &tag);

  // if the task is not pending, it will be added to the schedule queue
  // if the task is pending, the delay is changed to the passed in delay
  // e.g. if at second 0 task A with delay 5 is originally going to run at second 5 and
  // rescheduleTask(A) is called at second 4, A will be reschedule to run
  // at second 9
  virtual void
  rescheduleTask(TaskPtr task);

  virtual void
  rescheduleTaskAt (const Task::Tag &tag, double time);

  virtual void
  rescheduleTaskAt (TaskPtr task, double time);

  void
  execute(Executor::Job);

  void
  eventLoop();

  event_base *
  base() { return m_base; }

  // used in test
  int
  size();

protected:
  bool
  addToMap(TaskPtr task);

protected:
  typedef std::map<Task::Tag, TaskPtr> TaskMap;
  typedef std::map<Task::Tag, TaskPtr>::iterator TaskMapIt;
  typedef boost::recursive_mutex Mutex;
  typedef boost::unique_lock<Mutex> ScopedLock;

  TaskMap m_taskMap;
  Mutex m_mutex;
  volatile bool m_running;
  event_base *m_base;
  event *m_ev;
  boost::thread m_thread;
  Executor m_executor;
};

struct SchedulerException : virtual boost::exception, virtual std::exception { };

#endif // SCHEDULER_H

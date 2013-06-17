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

#include "task.h"
#include "scheduler.h"

static void
eventCallback(evutil_socket_t fd, short what, void *arg)
{
  Task *task = static_cast<Task *>(arg);
  task->execute();
  task = NULL;
}

Task::Task(const Callback &callback, const Tag &tag, const SchedulerPtr &scheduler)
     : m_callback(callback)
     , m_tag(tag)
     , m_scheduler(scheduler)
     , m_invoked(false)
     , m_event(NULL)
     , m_tv(NULL)
{
  m_event = evtimer_new(scheduler->base(), eventCallback, this);
  m_tv = new timeval;
}

Task::~Task()
{
  if (m_event != NULL)
  {
    event_free(m_event);
    m_event = NULL;
  }

  if (m_tv != NULL)
  {
    delete m_tv;
    m_tv = NULL;
  }
}

void
Task::setTv(double delay)
{
  // Alex: when using abs function, i would recommend use it with std:: prefix, otherwise
  // the standard one may be used, which converts everything to INT, making a lot of problems
  double intPart, fraction;
  fraction = modf(std::abs(delay), &intPart);

  m_tv->tv_sec = static_cast<int>(intPart);
  m_tv->tv_usec = static_cast<int>((fraction * 1000000));
}

void
Task::execute()
{
  // m_scheduler->execute(boost::bind(&Task::run, this));

  // using a shared_ptr of this to ensure that when invoked from executor
  // the task object still exists
  // otherwise, it could be the case that the run() is to be executed, but before it
  // could finish, the TaskPtr gets deleted from scheduler and the task object
  // gets destroyed, causing crash
  m_scheduler->execute(boost::bind(&Task::run, shared_from_this()));
}

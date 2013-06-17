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

#include "periodic-task.h"
#include "logging.h"
#include <utility>

INIT_LOGGER ("Scheduler.PeriodicTask");

PeriodicTask::PeriodicTask(const Callback &callback, const Tag &tag, const SchedulerPtr &scheduler,
                           IntervalGeneratorPtr generator)
  : Task(callback, tag, scheduler)
  , m_generator(generator)
{
}

void
PeriodicTask::run()
{
  if (!m_invoked)
  {
    m_invoked = true;
    m_callback();

    if (m_invoked)
      {
        // m_invoked getting back if it is rescheduled inside the callback
        m_scheduler->rescheduleTask(m_tag);
      }
  }
}

void
PeriodicTask::reset()
{
  m_invoked = false;
  double interval = m_generator->nextInterval();
  setTv(interval);
}

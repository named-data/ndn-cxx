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

#include "one-time-task.h"
#include "scheduler.h"

OneTimeTask::OneTimeTask(const Callback &callback, const Tag &tag, const SchedulerPtr &scheduler, double delay)
            : Task(callback, tag, scheduler)
{
  setTv(delay);
}

void
OneTimeTask::run()
{
  if (!m_invoked)
  {
    m_callback();
    m_invoked = true;
    deregisterSelf();
  }
}

void
OneTimeTask::deregisterSelf()
{
  m_scheduler->deleteTask(m_tag);
}

void
OneTimeTask::reset()
{
  m_invoked = false;
}

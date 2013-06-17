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

#ifndef ONE_TIME_TASK_H
#define ONE_TIME_TASK_H

#include "task.h"

class OneTimeTask : public Task
{
public:
  OneTimeTask(const Callback &callback, const Tag &tag, const SchedulerPtr &scheduler, double delay);
  virtual ~OneTimeTask(){}

  // invoke callback and mark self as invoked and deregister self from scheduler
  virtual void
  run() _OVERRIDE;

  // after reset, the task is marked as un-invoked and can be add to scheduler again, with same delay
  // if not invoked yet, no effect
  virtual void
  reset() _OVERRIDE;

private:
  // this is to deregister itself from scheduler automatically after invoke
  void
  deregisterSelf();
};


#endif // EVENT_SCHEDULER_H

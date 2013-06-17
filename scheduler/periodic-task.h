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

#ifndef PERIODIC_TASK_H
#define PERIODIC_TASK_H

#include "task.h"
#include "scheduler.h"
#include "interval-generator.h"

class PeriodicTask : public Task
{
public:
  // generator is needed only when this is a periodic task
  // two simple generators implementation (SimpleIntervalGenerator and RandomIntervalGenerator) are provided;
  // if user needs more complex pattern in the intervals between calls, extend class IntervalGenerator
  PeriodicTask(const Callback &callback, const Tag &tag, const SchedulerPtr &scheduler, IntervalGeneratorPtr generator);
  virtual ~PeriodicTask(){}

  // invoke callback, reset self and ask scheduler to schedule self with the next delay interval
  virtual void
  run() _OVERRIDE;

  // set the next delay and mark as un-invoke
  virtual void
  reset() _OVERRIDE;

private:
  IntervalGeneratorPtr m_generator;
};

#endif // PERIODIC_TASK_H

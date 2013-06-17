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

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/thread.hpp>
#include <deque>

/* A very simple executor to execute submitted tasks immediately or
 * in the future (depending on whether there is idle thread)
 * A fixed number of threads are created for executing tasks;
 * The policy is FIFO
 * No cancellation of submitted tasks
 */

class Executor
{
public:
  typedef boost::function<void ()> Job;

  Executor(int poolSize);
  ~Executor();

  // execute the job immediately or sometime in the future
  void
  execute(const Job &job);

  int
  poolSize();

// only for test
  int
  jobQueueSize();

  void
  start ();

  void
  shutdown ();

private:
  void
  run();

  Job
  waitForJob();

private:
  typedef std::deque<Job> JobQueue;
  typedef boost::mutex Mutex;
  typedef boost::unique_lock<Mutex> Lock;
  typedef boost::condition_variable Cond;
  typedef boost::thread Thread;
  typedef boost::thread_group ThreadGroup;
  JobQueue m_queue;
  Mutex m_mutex;
  Cond m_cond;
  ThreadGroup m_group;

  volatile bool m_needStop;
  int m_poolSize;
};

typedef boost::shared_ptr<Executor> ExecutorPtr;
#endif // EXECUTOR_H

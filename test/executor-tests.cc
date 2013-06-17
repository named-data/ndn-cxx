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

#include <boost/test/unit_test.hpp>
#include "executor/executor.h"

#include "logging.h"

INIT_LOGGER ("Test.Executor");

using namespace boost;
using namespace std;

void timeConsumingJob ()
{
  _LOG_DEBUG ("Start sleep");
  sleep(1);
  _LOG_DEBUG ("Finish sleep");
}

BOOST_AUTO_TEST_CASE(TestExecutor)
{
  INIT_LOGGERS ();

  {
    Executor executor (3);
    executor.start ();
    Executor::Job job = bind(timeConsumingJob);

    executor.execute(job);
    executor.execute(job);

    usleep(2000);
    // both jobs should have been taken care of
    BOOST_CHECK_EQUAL(executor.jobQueueSize(), 0);

    usleep(500000);

    // add four jobs while only one thread is idle
    executor.execute(job);
    executor.execute(job);
    executor.execute(job);
    executor.execute(job);

    usleep(1000);
    // three jobs should remain in queue
    BOOST_CHECK_EQUAL(executor.jobQueueSize(), 3);

    usleep(500000);
    // two threads should have finished and
    // take care of two queued jobs
    BOOST_CHECK_EQUAL(executor.jobQueueSize(), 1);

    // all jobs should have been fetched
    usleep(501000);
    BOOST_CHECK_EQUAL(executor.jobQueueSize(), 0);

    executor.shutdown ();
  } //separate scope to ensure that destructor is called


  sleep(1);
}

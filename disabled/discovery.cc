/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "discovery.h"

#include "scheduler/scheduler.h"
#include "scheduler/simple-interval-generator.h"
#include "scheduler/task.h"
#include "scheduler/periodic-task.h"

#include <sstream>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

using namespace std;

namespace ndn
{

namespace discovery
{

const string
TaggedFunction::CHAR_SET = string("abcdefghijklmnopqtrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

TaggedFunction::TaggedFunction(const Callback &callback, const string &tag)
                   : m_callback(callback)
                   , m_tag(tag)
{
}

string
TaggedFunction::GetRandomTag()
{
  //boost::random::random_device rng;
  boost::random::uniform_int_distribution<> dist(0, CHAR_SET.size() - 1);
  ostringstream oss;
  for (int i = 0; i < DEFAULT_TAG_SIZE; i++)
  {
    //oss << CHAR_SET[dist(rng)];
  }
  return oss.str();
}

void
TaggedFunction::operator()(const Name &name)
{
  if (!m_callback.empty())
  {
    m_callback(name);
  }
}

} // namespace discovery

const double
Discovery::INTERVAL = 15.0;

Discovery *
Discovery::instance = NULL;

boost::mutex
Discovery::mutex;

Discovery::Discovery()
              : m_scheduler(new Scheduler())
              , m_localPrefix("/")
{
  m_scheduler->start();

  Scheduler::scheduleOneTimeTask (m_scheduler, 0,
                                  boost::bind(&Discovery::poll, this), "Initial-Local-Prefix-Check");
  Scheduler::schedulePeriodicTask (m_scheduler,
                                   boost::make_shared<SimpleIntervalGenerator>(INTERVAL),
                                   boost::bind(&Discovery::poll, this), "Local-Prefix-Check");
}

Discovery::~Discovery()
{
  m_scheduler->shutdown();
}

void
Discovery::addCallback(const discovery::TaggedFunction &callback)
{
  m_callbacks.push_back(callback);
}

int
Discovery::deleteCallback(const discovery::TaggedFunction &callback)
{
  List::iterator it = m_callbacks.begin();
  while (it != m_callbacks.end())
  {
    if ((*it) == callback)
    {
      it = m_callbacks.erase(it);
    }
    else
    {
      ++it;
    }
  }
  return m_callbacks.size();
}

void
Discovery::poll()
{
  Name localPrefix = Wrapper::getLocalPrefix();
  if (localPrefix != m_localPrefix)
  {
    Lock lock(mutex);
    for (List::iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
    {
      (*it)(localPrefix);
    }
    m_localPrefix = localPrefix;
  }
}

void
Discovery::registerCallback(const discovery::TaggedFunction &callback)
{
  Lock lock(mutex);
  if (instance == NULL)
  {
    instance = new Discovery();
  }

  instance->addCallback(callback);
}

void
Discovery::deregisterCallback(const discovery::TaggedFunction &callback)
{
  Lock lock(mutex);
  if (instance == NULL)
  {
    cerr << "Discovery::deregisterCallback called without instance" << endl;
  }
  else
  {
    int size = instance->deleteCallback(callback);
    if (size == 0)
    {
      delete instance;
      instance = NULL;
    }
  }
}

}


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

#ifndef NDN_CLOSURE_H
#define NDN_CLOSURE_H

#include "ndn.cxx/common.h"
#include "ndn.cxx/fields/name.h"
#include "ndn.cxx/interest.h"

namespace ndn {

class ParsedContentObject;
typedef boost::shared_ptr<ParsedContentObject> PcoPtr;

class Closure
{
public:
  typedef boost::function<void (Name, PcoPtr pco)> DataCallback;

  typedef boost::function<void (Name, const Closure &, InterestPtr)> TimeoutCallback;

  Closure(const DataCallback &dataCallback, const TimeoutCallback &timeoutCallback = TimeoutCallback());
  virtual ~Closure();

  virtual void
  runDataCallback(Name name, ndn::PcoPtr pco);

  virtual void
  runTimeoutCallback(Name interest, const Closure &closure, InterestPtr originalInterest);

  virtual Closure *
  dup () const { return new Closure (*this); }

public:
  TimeoutCallback m_timeoutCallback;
  DataCallback m_dataCallback;
};

} // ndn

#endif

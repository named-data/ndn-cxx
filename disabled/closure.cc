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

#include "closure.h"

namespace ndn {

Closure::Closure(const DataCallback &dataCallback, const TimeoutCallback &timeoutCallback)
  : m_timeoutCallback (timeoutCallback)
  , m_dataCallback (dataCallback)
{
}

Closure::~Closure ()
{
}

void
Closure::runTimeoutCallback(Name interest, const Closure &closure, InterestPtr origInterest)
{
  if (!m_timeoutCallback.empty ())
    {
      m_timeoutCallback (interest, closure, origInterest);
    }
}


void
Closure::runDataCallback(Name name, PcoPtr content)
{
  if (!m_dataCallback.empty ())
    {
      m_dataCallback (name, content);
    }
}

} // ndn

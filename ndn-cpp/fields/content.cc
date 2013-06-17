/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "content.h"
#include "ndn-cpp/error.h"

namespace ndn
{

const name::Component Content::noFinalBlock = name::Component ();
const TimeInterval Content::noFreshness;
const TimeInterval Content::maxFreshness = time::Seconds (2147);

Content::Content ()
{
}

Content::Content (const void *buffer, size_t size,
                  const Time &timestamp,
                  Type type/* = DATA*/,
                  const TimeInterval &freshness/* = maxFreshness*/,
                  const name::Component &finalBlock/* = noFinalBlock*/)
  : m_timestamp (timestamp)
  , m_type (type)
  , m_freshness (freshness)
  , m_finalBlockId (finalBlock)

  , m_content (buffer, size)
{
}

Content::Content (const void *buffer, size_t size,
                  Type type/* = DATA*/,
                  const TimeInterval &freshness/* = maxFreshness*/,
                  const name::Component &finalBlock/* = noFinalBlock*/)
  : m_timestamp (time::Now ())
  , m_type (type)
  , m_freshness (freshness)
  , m_finalBlockId (finalBlock)

  , m_content (buffer, size)
{
}


} // ndn

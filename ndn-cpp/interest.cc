/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *         Zhenkai Zhu <zhenkai@cs.ucla.edu>
 */

#include "interest.h"
#include <boost/lexical_cast.hpp>

using namespace std;

namespace ndn {

Interest::Interest ()
  // m_name
  : m_maxSuffixComponents (Interest::ncomps)
  , m_minSuffixComponents (Interest::ncomps)
  , m_answerOriginKind (AOK_DEFAULT)
  , m_interestLifetime (time::Seconds (-1.0))
  , m_scope (NO_SCOPE)
  , m_childSelector (CHILD_DEFAULT)
  // m_publisherKeyDigest
{
}

Interest::Interest (const Name &name)
  : m_name (name)
  , m_maxSuffixComponents (Interest::ncomps)
  , m_minSuffixComponents (Interest::ncomps)
  , m_answerOriginKind(AOK_DEFAULT)
  , m_interestLifetime (time::Seconds (-1.0))
  , m_scope (NO_SCOPE)
  , m_childSelector (CHILD_DEFAULT)
  // m_publisherKeyDigest
{
}

Interest::Interest (const Interest &other)
{
  m_name = other.m_name;
  m_maxSuffixComponents = other.m_maxSuffixComponents;
  m_minSuffixComponents = other.m_minSuffixComponents;
  m_answerOriginKind = other.m_answerOriginKind;
  m_interestLifetime = other.m_interestLifetime;
  m_scope = other.m_scope;
  m_childSelector = other.m_childSelector;
  m_publisherPublicKeyDigest = other.m_publisherPublicKeyDigest;
}

bool
Interest::operator == (const Interest &other)
{
  return
       m_name == other.m_name
    && m_maxSuffixComponents == other.m_maxSuffixComponents
    && m_minSuffixComponents == other.m_minSuffixComponents
    && m_answerOriginKind == other.m_answerOriginKind
    && m_interestLifetime == other.m_interestLifetime
    && m_scope == other.m_scope
    && m_childSelector == other.m_childSelector;
}

} // ndn

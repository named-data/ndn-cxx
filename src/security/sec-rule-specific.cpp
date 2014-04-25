/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#include "common.hpp"

#include "sec-rule-specific.hpp"
#include "signature-sha256-with-rsa.hpp"

using namespace std;

namespace ndn {

SecRuleSpecific::SecRuleSpecific(shared_ptr<Regex> dataRegex,
                                 shared_ptr<Regex> signerRegex)
  : SecRule(true)
  , m_dataRegex(dataRegex)
  , m_signerRegex(signerRegex)
{
}

SecRuleSpecific::SecRuleSpecific(const SecRuleSpecific& rule)
  : SecRule(true)
  , m_dataRegex(rule.m_dataRegex)
  , m_signerRegex(rule.m_signerRegex)
{
}

bool
SecRuleSpecific::matchDataName(const Data& data)
{
  return m_dataRegex->match(data.getName());
}

bool
SecRuleSpecific::matchSignerName(const Data& data)
{
  try
    {
      SignatureSha256WithRsa sig(data.getSignature());
      Name signerName = sig.getKeyLocator().getName();
      return m_signerRegex->match(signerName);
    }
  catch (std::runtime_error& e)
    {
      return false;
    }
}

bool
SecRuleSpecific::satisfy(const Data& data)
{
  return (matchDataName(data) && matchSignerName(data)) ? true : false;
}

bool
SecRuleSpecific::satisfy(const Name& dataName, const Name& signerName)
{
  return (m_dataRegex->match(dataName) && m_signerRegex->match(signerName));
}

} // namespace ndn

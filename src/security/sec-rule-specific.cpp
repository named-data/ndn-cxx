/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
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
{}

SecRuleSpecific::SecRuleSpecific(const SecRuleSpecific& rule)
  : SecRule(true)
  , m_dataRegex(rule.m_dataRegex)
  , m_signerRegex(rule.m_signerRegex)
{}

bool
SecRuleSpecific::matchDataName(const Data& data)
{ return m_dataRegex->match(data.getName()); }

bool
SecRuleSpecific::matchSignerName(const Data& data)
{
  try {
    SignatureSha256WithRsa sig(data.getSignature());
    Name signerName = sig.getKeyLocator().getName ();
    return m_signerRegex->match(signerName);
  }
  catch (SignatureSha256WithRsa::Error& e) {
    return false;
  }
  catch (KeyLocator::Error& e) {
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

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SEC_RULE_SPECIFIC_H
#define NDN_SEC_RULE_SPECIFIC_H

#include "../common.hpp"
#include "sec-rule.hpp"
#include "../util/regex.hpp"

namespace ndn{

class SecRuleSpecific : public ndn::SecRule
{
  
public:
  SecRuleSpecific(ndn::shared_ptr<ndn::Regex> dataRegex,
                  ndn::shared_ptr<ndn::Regex> signerRegex);

  SecRuleSpecific(const SecRuleSpecific& rule);

  virtual
  ~SecRuleSpecific() {};

  bool 
  matchDataName(const ndn::Data& data);

  bool 
  matchSignerName(const ndn::Data& data);

  bool
  satisfy(const ndn::Data& data);

  bool
  satisfy(const ndn::Name& dataName, const ndn::Name& signerName);
  
private:
  ndn::shared_ptr<ndn::Regex> m_dataRegex;
  ndn::shared_ptr<ndn::Regex> m_signerRegex;
};

}

#endif

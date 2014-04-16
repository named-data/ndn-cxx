/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_SEC_RULE_SPECIFIC_H
#define NDN_SECURITY_SEC_RULE_SPECIFIC_H

#include "../common.hpp"
#include "sec-rule.hpp"
#include "../util/regex.hpp"

namespace ndn {

class SecRuleSpecific : public SecRule
{

public:
  SecRuleSpecific(shared_ptr<Regex> dataRegex,
                  shared_ptr<Regex> signerRegex);

  SecRuleSpecific(const SecRuleSpecific& rule);

  virtual
  ~SecRuleSpecific() {};

  bool
  matchDataName(const Data& data);

  bool
  matchSignerName(const Data& data);

  bool
  satisfy(const Data& data);

  bool
  satisfy(const Name& dataName, const Name& signerName);

private:
  shared_ptr<Regex> m_dataRegex;
  shared_ptr<Regex> m_signerRegex;
};

} // namespace ndn

#endif //NDN_SECURITY_SEC_RULE_SPECIFIC_H

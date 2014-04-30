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

  SecRuleSpecific(shared_ptr<Regex> dataRegex);

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

  bool
  isExempted() const
  {
    return m_isExempted;
  }

private:
  shared_ptr<Regex> m_dataRegex;
  shared_ptr<Regex> m_signerRegex;
  bool m_isExempted;
};

} // namespace ndn

#endif //NDN_SECURITY_SEC_RULE_SPECIFIC_H

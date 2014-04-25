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

#ifndef NDN_SECURITY_SEC_RULE_RELATIVE_HPP
#define NDN_SECURITY_SEC_RULE_RELATIVE_HPP

#include "sec-rule.hpp"
#include "../util/regex.hpp"

namespace ndn {

class SecRuleRelative : public SecRule
{
public:
  class Error : public SecRule::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : SecRule::Error(what)
    {
    }
  };

  SecRuleRelative(const std::string& dataRegex, const std::string& signerRegex,
                  const std::string& op,
                  const std::string& dataExpand, const std::string& signerExpand,
                  bool isPositive);

  virtual
  ~SecRuleRelative();

  virtual bool
  matchDataName(const Data& data);

  virtual bool
  matchSignerName(const Data& data);

  virtual bool
  satisfy(const Data& data);

  virtual bool
  satisfy(const Name& dataName, const Name& signerName);

private:
  bool
  compare(const Name& dataName, const Name& signerName);

private:
  const std::string m_dataRegex;
  const std::string m_signerRegex;
  const std::string m_op;
  const std::string m_dataExpand;
  const std::string m_signerExpand;

  Regex m_dataNameRegex;
  Regex m_signerNameRegex;
};

} // namespace ndn

#endif //NDN_SECURITY_SEC_RULE_RELATIVE_HPP

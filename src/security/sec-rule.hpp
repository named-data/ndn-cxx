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

#ifndef NDN_SECURITY_SEC_RULE_HPP
#define NDN_SECURITY_SEC_RULE_HPP

#include "../common.hpp"
#include "../data.hpp"

namespace ndn {

class SecRule
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  SecRule(bool isPositive)
    : m_isPositive(isPositive)
  {
  }

  virtual
  ~SecRule()
  {
  }

  virtual bool
  matchDataName(const Data& data) = 0;

  virtual bool
  matchSignerName(const Data& data) = 0;

  virtual bool
  satisfy(const Data& data) = 0;

  virtual bool
  satisfy(const Name& dataName, const Name& signerName) = 0;

  inline bool
  isPositive();

protected:
  bool m_isPositive;
};

bool
SecRule::isPositive()
{
  return m_isPositive;
}

} // namespace ndn

#endif //NDN_SECURITY_SEC_RULE_HPP

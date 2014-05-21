/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
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

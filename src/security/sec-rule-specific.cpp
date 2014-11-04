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

#include "common.hpp"

#include "sec-rule-specific.hpp"
#include "signature-sha256-with-rsa.hpp"

namespace ndn {

SecRuleSpecific::SecRuleSpecific(shared_ptr<Regex> dataRegex,
                                 shared_ptr<Regex> signerRegex)
  : SecRule(true)
  , m_dataRegex(dataRegex)
  , m_signerRegex(signerRegex)
  , m_isExempted(false)
{
}

SecRuleSpecific::SecRuleSpecific(shared_ptr<Regex> dataRegex)
  : SecRule(true)
  , m_dataRegex(dataRegex)
  , m_isExempted(true)
{
}

SecRuleSpecific::SecRuleSpecific(const SecRuleSpecific& rule)
  : SecRule(true)
  , m_dataRegex(rule.m_dataRegex)
  , m_signerRegex(rule.m_signerRegex)
  , m_isExempted(rule.m_isExempted)
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
  if (m_isExempted)
    return true;

  try
    {
      if (!data.getSignature().hasKeyLocator())
        return false;

      const KeyLocator& keyLocator = data.getSignature().getKeyLocator();
      if (keyLocator.getType() != KeyLocator::KeyLocator_Name)
        return false;

      const Name& signerName = keyLocator.getName();
      return m_signerRegex->match(signerName);
    }
  catch (tlv::Error& e)
    {
      return false;
    }
  catch (RegexMatcher::Error& e)
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
  bool isSignerMatched = m_isExempted || m_signerRegex->match(signerName);
  return m_dataRegex->match(dataName) && isSignerMatched;
}

} // namespace ndn

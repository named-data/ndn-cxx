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

#include "sec-rule-relative.hpp"

#include "signature-sha256-with-rsa.hpp"
#include "security-common.hpp"

namespace ndn {

using std::string;

SecRuleRelative::SecRuleRelative(const string& dataRegex, const string& signerRegex,
                                 const string& op,
                                 const string& dataExpand, const string& signerExpand,
                                 bool isPositive)
  : SecRule(isPositive),
    m_dataRegex(dataRegex),
    m_signerRegex(signerRegex),
    m_op(op),
    m_dataExpand(dataExpand),
    m_signerExpand(signerExpand),
    m_dataNameRegex(dataRegex, dataExpand),
    m_signerNameRegex(signerRegex, signerExpand)
{
  if (op != ">" && op != ">=" && op != "==")
    BOOST_THROW_EXCEPTION(Error("op is wrong"));
}

SecRuleRelative::~SecRuleRelative()
{
}

bool
SecRuleRelative::satisfy(const Data& data)
{
  Name dataName = data.getName();
  try
    {
      if (!data.getSignature().hasKeyLocator())
        return false;

      const KeyLocator& keyLocator = data.getSignature().getKeyLocator();
      if (keyLocator.getType() != KeyLocator::KeyLocator_Name)
        return false;

      const Name& signerName = keyLocator.getName();
      return satisfy(dataName, signerName);
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
SecRuleRelative::satisfy(const Name& dataName, const Name& signerName)
{
  if (!m_dataNameRegex.match(dataName))
    return false;
  Name expandDataName = m_dataNameRegex.expand();

  if (!m_signerNameRegex.match(signerName))
    return false;
  Name expandSignerName =  m_signerNameRegex.expand();

  bool matched = compare(expandDataName, expandSignerName);

  return matched;
}

bool
SecRuleRelative::matchDataName(const Data& data)
{
  return m_dataNameRegex.match(data.getName());
}

bool
SecRuleRelative::matchSignerName(const Data& data)
{
  try
    {
      if (!data.getSignature().hasKeyLocator())
        return false;

      const KeyLocator& keyLocator = data.getSignature().getKeyLocator();
      if (keyLocator.getType() != KeyLocator::KeyLocator_Name)
        return false;

      const Name& signerName = keyLocator.getName();
      return m_signerNameRegex.match(signerName);
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
SecRuleRelative::compare(const Name& dataName, const Name& signerName)
{
  if ((dataName == signerName) && ("==" == m_op || ">=" == m_op))
    return true;

  Name::const_iterator i = dataName.begin();
  Name::const_iterator j = signerName.begin();

  for (; i != dataName.end() && j != signerName.end(); i++, j++)
    {
      if (i->compare(*j) == 0)
        continue;
      else
        return false;
    }

  if (i == dataName.end())
    return false;
  else
    return true;
}

} // namespace ndn

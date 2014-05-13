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

#include "sec-rule-relative.hpp"

#include "signature-sha256-with-rsa.hpp"
#include "security-common.hpp"

using namespace std;

namespace ndn {

SecRuleRelative::SecRuleRelative (const string& dataRegex, const string& signerRegex,
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
    throw Error("op is wrong!");
}

SecRuleRelative::~SecRuleRelative()
{
}

bool
SecRuleRelative::satisfy (const Data& data)
{
  Name dataName = data.getName();
  try
    {
      SignatureSha256WithRsa sig(data.getSignature());
      Name signerName = sig.getKeyLocator().getName ();
      return satisfy (dataName, signerName);
    }
  catch (std::runtime_error& e)
    {
      return false;
    }
}

bool
SecRuleRelative::satisfy (const Name& dataName, const Name& signerName)
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
SecRuleRelative::matchDataName (const Data& data)
{
  return m_dataNameRegex.match(data.getName());
}

bool
SecRuleRelative::matchSignerName (const Data& data)
{
  try
    {
      SignatureSha256WithRsa sig(data.getSignature());
      Name signerName = sig.getKeyLocator().getName ();
      return m_signerNameRegex.match(signerName);
    }
  catch (std::runtime_error& e)
    {
      return false;
    }
}

bool
SecRuleRelative::compare(const Name& dataName, const Name& signerName)
{
  if ((dataName == signerName) && ("==" == m_op || ">=" == m_op))
    return true;

  Name::const_iterator i = dataName.begin ();
  Name::const_iterator j = signerName.begin ();

  for (; i != dataName.end () && j != signerName.end (); i++, j++)
    {
      if ((i->compare(*j)) == 0)
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

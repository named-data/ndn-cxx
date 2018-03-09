/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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
 */

#include "rule.hpp"
#include "util/logger.hpp"

#include <boost/algorithm/string/predicate.hpp>

NDN_LOG_INIT(ndn.security.validator_config.Rule);

namespace ndn {
namespace security {
namespace v2 {
namespace validator_config {

Rule::Rule(const std::string& id, uint32_t pktType)
  : m_id(id)
  , m_pktType(pktType)
{
}

void
Rule::addFilter(unique_ptr<Filter> filter)
{
  m_filters.push_back(std::move(filter));
}

void
Rule::addChecker(unique_ptr<Checker> checker)
{
  m_checkers.push_back(std::move(checker));
}

bool
Rule::match(uint32_t pktType, const Name& pktName) const
{
  NDN_LOG_TRACE("Trying to match " << pktName);
  if (pktType != m_pktType) {
    BOOST_THROW_EXCEPTION(Error("Invalid packet type supplied (" +
                                to_string(pktType) + " != " + to_string(m_pktType) + ")"));
  }

  if (m_filters.empty()) {
    return true;
  }

  bool retval = false;
  for (const auto& filter : m_filters) {
    retval |= filter->match(pktType, pktName);
    if (retval) {
      break;
    }
  }
  return retval;
}

bool
Rule::check(uint32_t pktType, const Name& pktName, const Name& klName,
            const shared_ptr<ValidationState>& state) const
{
  NDN_LOG_TRACE("Trying to check " << pktName << " with keyLocator " << klName);

  if (pktType != m_pktType) {
    BOOST_THROW_EXCEPTION(Error("Invalid packet type supplied (" +
                                to_string(pktType) + " != " + to_string(m_pktType) + ")"));
  }

  bool hasPendingResult = false;
  for (const auto& checker : m_checkers) {
    bool result = checker->check(pktType, pktName, klName, state);
    if (!result) {
      return result;
    }
    hasPendingResult = true;
  }

  return hasPendingResult;
}

unique_ptr<Rule>
Rule::create(const ConfigSection& configSection, const std::string& configFilename)
{
  auto propertyIt = configSection.begin();

  // Get rule.id
  if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "id")) {
    BOOST_THROW_EXCEPTION(Error("Expecting <rule.id>"));
  }

  std::string ruleId = propertyIt->second.data();
  propertyIt++;

  // Get rule.for
  if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "for")) {
    BOOST_THROW_EXCEPTION(Error("Expecting <rule.for> in rule: " + ruleId));
  }

  std::string usage = propertyIt->second.data();
  propertyIt++;

  bool isForData = false;
  if (boost::iequals(usage, "data")) {
    isForData = true;
  }
  else if (boost::iequals(usage, "interest")) {
    isForData = false;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("Unrecognized <rule.for>: " + usage + " in rule: " + ruleId));
  }

  auto rule = make_unique<Rule>(ruleId, isForData ? tlv::Data : tlv::Interest);

  // Get rule.filter(s)
  for (; propertyIt != configSection.end(); propertyIt++) {
    if (!boost::iequals(propertyIt->first, "filter")) {
      if (boost::iequals(propertyIt->first, "checker")) {
        break;
      }
      BOOST_THROW_EXCEPTION(Error("Expecting <rule.filter> in rule: " + ruleId));
    }

    rule->addFilter(Filter::create(propertyIt->second, configFilename));
  }

  // Get rule.checker(s)
  bool hasCheckers = false;
  for (; propertyIt != configSection.end(); propertyIt++) {
    if (!boost::iequals(propertyIt->first, "checker")) {
      BOOST_THROW_EXCEPTION(Error("Expecting <rule.checker> in rule: " + ruleId));
    }

    rule->addChecker(Checker::create(propertyIt->second, configFilename));
    hasCheckers = true;
  }

  if (propertyIt != configSection.end()) {
    BOOST_THROW_EXCEPTION(Error("Expecting end of <rule>: " + ruleId));
  }

  if (!hasCheckers) {
    BOOST_THROW_EXCEPTION(Error("No <rule.checker> is specified in rule: " + ruleId));
  }

  return rule;
}

} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn

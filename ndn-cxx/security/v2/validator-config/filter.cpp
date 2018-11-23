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

#include "filter.hpp"

#include "data.hpp"
#include "interest.hpp"
#include "security/security-common.hpp"
#include "util/regex.hpp"

#include <boost/algorithm/string/predicate.hpp>

namespace ndn {
namespace security {
namespace v2 {
namespace validator_config {

bool
Filter::match(uint32_t pktType, const Name& pktName)
{
  BOOST_ASSERT(pktType == tlv::Interest || pktType == tlv::Data);

  if (pktType == tlv::Interest) {
    if (pktName.size() < signed_interest::MIN_SIZE)
      return false;

    return matchName(pktName.getPrefix(-signed_interest::MIN_SIZE));
  }
  else {
    return matchName(pktName);
  }
}

RelationNameFilter::RelationNameFilter(const Name& name, NameRelation relation)
  : m_name(name)
  , m_relation(relation)
{
}

bool
RelationNameFilter::matchName(const Name& name)
{
  return checkNameRelation(m_relation, m_name, name);
}

RegexNameFilter::RegexNameFilter(const Regex& regex)
  : m_regex(regex)
{
}

bool
RegexNameFilter::matchName(const Name& name)
{
  return m_regex.match(name);
}

unique_ptr<Filter>
Filter::create(const ConfigSection& configSection, const std::string& configFilename)
{
  auto propertyIt = configSection.begin();

  if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "type")) {
    BOOST_THROW_EXCEPTION(Error("Expecting <filter.type>"));
  }

  std::string type = propertyIt->second.data();
  if (boost::iequals(type, "name"))
    return createNameFilter(configSection, configFilename);
  else
    BOOST_THROW_EXCEPTION(Error("Unrecognized <filter.type>: " + type));
}

unique_ptr<Filter>
Filter::createNameFilter(const ConfigSection& configSection, const std::string& configFilename)
{
  auto propertyIt = configSection.begin();
  propertyIt++;

  if (propertyIt == configSection.end())
    BOOST_THROW_EXCEPTION(Error("Unexpected end of <filter>"));

  if (boost::iequals(propertyIt->first, "name")) {
    // Get filter.name
    Name name;
    try {
      name = Name(propertyIt->second.data());
    }
    catch (const Name::Error&) {
      BOOST_THROW_EXCEPTION(Error("Invalid <filter.name>: " + propertyIt->second.data()));
    }

    propertyIt++;

    // Get filter.relation
    if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "relation")) {
      BOOST_THROW_EXCEPTION(Error("Expecting <filter.relation>"));
    }

    NameRelation relation = getNameRelationFromString(propertyIt->second.data());
    propertyIt++;

    if (propertyIt != configSection.end())
      BOOST_THROW_EXCEPTION(Error("Expecting end of <filter>"));

    return make_unique<RelationNameFilter>(name, relation);
  }
  else if (boost::iequals(propertyIt->first, "regex")) {
    std::string regexString = propertyIt->second.data();
    propertyIt++;

    if (propertyIt != configSection.end())
      BOOST_THROW_EXCEPTION(Error("Expecting end of <filter>"));

    try {
      return make_unique<RegexNameFilter>(Regex(regexString));
    }
    catch (const Regex::Error&) {
      BOOST_THROW_EXCEPTION(Error("Invalid <filter.regex>: " + regexString));
    }
  }
  else {
    BOOST_THROW_EXCEPTION(Error("Unrecognized <filter> property: " + propertyIt->first));
  }
}

} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn

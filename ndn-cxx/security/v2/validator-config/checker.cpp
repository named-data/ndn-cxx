/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/security/v2/validator-config/checker.hpp"
#include "ndn-cxx/security/v2/validation-state.hpp"
#include "ndn-cxx/security/verification-helpers.hpp"
#include "ndn-cxx/security/pib/key.hpp"

#include <boost/algorithm/string/predicate.hpp>

namespace ndn {
namespace security {
namespace v2 {
namespace validator_config {

bool
Checker::check(uint32_t pktType, const Name& pktName, const Name& klName,
               const shared_ptr<ValidationState>& state)
{
  BOOST_ASSERT(pktType == tlv::Interest || pktType == tlv::Data);

  if (pktType == tlv::Interest) {
    if (pktName.size() < signed_interest::MIN_SIZE)
      return false;

    return checkNames(pktName.getPrefix(-signed_interest::MIN_SIZE), klName, state);
  }
  else {
    return checkNames(pktName, klName, state);
  }
}

NameRelationChecker::NameRelationChecker(const Name& name, const NameRelation& relation)
  : m_name(name)
  , m_relation(relation)
{
}

bool
NameRelationChecker::checkNames(const Name& pktName, const Name& klName,
                                const shared_ptr<ValidationState>& state)
{
  // pktName not used in this check
  Name identity = extractIdentityFromKeyName(klName);
  bool result = checkNameRelation(m_relation, m_name, identity);
  if (!result) {
    std::ostringstream os;
    os << "KeyLocator check failed: name relation " << m_name << " " << m_relation
       << " for packet " << pktName << " is invalid"
       << " (KeyLocator=" << klName << ", identity=" << identity << ")";
    state->fail({ValidationError::POLICY_ERROR, os.str()});
  }
  return result;
}

RegexChecker::RegexChecker(const Regex& regex)
  : m_regex(regex)
{
}

bool
RegexChecker::checkNames(const Name& pktName, const Name& klName, const shared_ptr<ValidationState>& state)
{
  bool result = m_regex.match(klName);
  if (!result) {
    std::ostringstream os;
    os << "KeyLocator check failed: regex " << m_regex << " for packet " << pktName << " is invalid"
       << " (KeyLocator=" << klName << ")";
    state->fail({ValidationError::POLICY_ERROR, os.str()});
  }
  return result;
}

HyperRelationChecker::HyperRelationChecker(const std::string& pktNameExpr, const std::string pktNameExpand,
                                           const std::string& klNameExpr, const std::string klNameExpand,
                                           const NameRelation& hyperRelation)
  : m_hyperPRegex(pktNameExpr, pktNameExpand)
  , m_hyperKRegex(klNameExpr, klNameExpand)
  , m_hyperRelation(hyperRelation)
{
}

bool
HyperRelationChecker::checkNames(const Name& pktName, const Name& klName,
                                 const shared_ptr<ValidationState>& state)
{
  if (!m_hyperPRegex.match(pktName) || !m_hyperKRegex.match(klName)) {
    std::ostringstream os;
    os << "Packet " << pktName << " (" << "KeyLocator=" << klName << ") does not match "
       << "the hyper relation rule pkt=" << m_hyperPRegex << ", key=" << m_hyperKRegex;
    state->fail({ValidationError::POLICY_ERROR, os.str()});
    return false;
  }

  bool result = checkNameRelation(m_hyperRelation, m_hyperKRegex.expand(), m_hyperPRegex.expand());
  if (!result) {
    std::ostringstream os;
    os << "KeyLocator check failed: hyper relation " << m_hyperRelation
       << " pkt=" << m_hyperPRegex << ", key=" << m_hyperKRegex
       << " of packet " << pktName << " (KeyLocator=" << klName << ") is invalid";
    state->fail({ValidationError::POLICY_ERROR, os.str()});
  }
  return result;
}

unique_ptr<Checker>
Checker::create(const ConfigSection& configSection, const std::string& configFilename)
{
  auto propertyIt = configSection.begin();

  // Get checker.type
  if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "type")) {
    NDN_THROW(Error("Expecting <checker.type>"));
  }

  std::string type = propertyIt->second.data();
  if (boost::iequals(type, "customized")) {
    return createCustomizedChecker(configSection, configFilename);
  }
  else if (boost::iequals(type, "hierarchical")) {
    return createHierarchicalChecker(configSection, configFilename);
  }
  else {
    NDN_THROW(Error("Unrecognized <checker.type>: " + type));
  }
}

unique_ptr<Checker>
Checker::createCustomizedChecker(const ConfigSection& configSection,
                                 const std::string& configFilename)
{
  auto propertyIt = configSection.begin();
  propertyIt++;

  // TODO implement restrictions based on signature type (outside this checker)

  if (propertyIt != configSection.end() && boost::iequals(propertyIt->first, "sig-type")) {
    // ignore sig-type
    propertyIt++;
  }

  // Get checker.key-locator
  if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "key-locator")) {
    NDN_THROW(Error("Expecting <checker.key-locator>"));
  }

  auto checker = createKeyLocatorChecker(propertyIt->second, configFilename);
  propertyIt++;

  if (propertyIt != configSection.end()) {
    NDN_THROW(Error("Expecting end of <checker>"));
  }
  return checker;
}

unique_ptr<Checker>
Checker::createHierarchicalChecker(const ConfigSection& configSection,
                                   const std::string& configFilename)
{
  auto propertyIt = configSection.begin();
  propertyIt++;

  // TODO implement restrictions based on signature type (outside this checker)

  if (propertyIt != configSection.end() && boost::iequals(propertyIt->first, "sig-type")) {
    // ignore sig-type
    propertyIt++;
  }

  if (propertyIt != configSection.end()) {
    NDN_THROW(Error("Expecting end of <checker>"));
  }
  return make_unique<HyperRelationChecker>("^(<>*)$",        "\\1",
                                           "^(<>*)<KEY><>$", "\\1",
                                           NameRelation::IS_PREFIX_OF);
}

unique_ptr<Checker>
Checker::createKeyLocatorChecker(const ConfigSection& configSection,
                                 const std::string& configFilename)
{
  auto propertyIt = configSection.begin();

  // Get checker.key-locator.type
  if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "type"))
    NDN_THROW(Error("Expecting <checker.key-locator.type>"));

  std::string type = propertyIt->second.data();
  if (boost::iequals(type, "name"))
    return createKeyLocatorNameChecker(configSection, configFilename);
  else
    NDN_THROW(Error("Unrecognized <checker.key-locator.type>: " + type));
}

unique_ptr<Checker>
Checker::createKeyLocatorNameChecker(const ConfigSection& configSection,
                                     const std::string& configFilename)
{
  auto propertyIt = configSection.begin();
  propertyIt++;

  if (propertyIt == configSection.end())
    NDN_THROW(Error("Unexpected end of <checker.key-locator>"));

  if (boost::iequals(propertyIt->first, "name")) {
    Name name;
    try {
      name = Name(propertyIt->second.data());
    }
    catch (const Name::Error&) {
      NDN_THROW_NESTED(Error("Invalid <checker.key-locator.name>: " + propertyIt->second.data()));
    }
    propertyIt++;

    if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "relation")) {
      NDN_THROW(Error("Expecting <checker.key-locator.relation>"));
    }

    std::string relationString = propertyIt->second.data();
    propertyIt++;

    NameRelation relation = getNameRelationFromString(relationString);

    if (propertyIt != configSection.end()) {
      NDN_THROW(Error("Expecting end of <checker.key-locator>"));
    }
    return make_unique<NameRelationChecker>(name, relation);
  }
  else if (boost::iequals(propertyIt->first, "regex")) {
    std::string regexString = propertyIt->second.data();
    propertyIt++;

    if (propertyIt != configSection.end()) {
      NDN_THROW(Error("Expecting end of <checker.key-locator>"));
    }

    try {
      return make_unique<RegexChecker>(Regex(regexString));
    }
    catch (const Regex::Error&) {
      NDN_THROW_NESTED(Error("Invalid <checker.key-locator.regex>: " + regexString));
    }
  }
  else if (boost::iequals(propertyIt->first, "hyper-relation")) {
    const ConfigSection& hSection = propertyIt->second;
    auto hPropertyIt = hSection.begin();

    // Get k-regex
    if (hPropertyIt == hSection.end() || !boost::iequals(hPropertyIt->first, "k-regex")) {
      NDN_THROW(Error("Expecting <checker.key-locator.hyper-relation.k-regex>"));
    }

    std::string kRegex = hPropertyIt->second.data();
    hPropertyIt++;

    // Get k-expand
    if (hPropertyIt == hSection.end() || !boost::iequals(hPropertyIt->first, "k-expand")) {
      NDN_THROW(Error("Expecting <checker.key-locator.hyper-relation.k-expand>"));
    }

    std::string kExpand = hPropertyIt->second.data();
    hPropertyIt++;

    // Get h-relation
    if (hPropertyIt == hSection.end() || !boost::iequals(hPropertyIt->first, "h-relation")) {
      NDN_THROW(Error("Expecting <checker.key-locator.hyper-relation.h-relation>"));
    }

    std::string hRelation = hPropertyIt->second.data();
    hPropertyIt++;

    // Get p-regex
    if (hPropertyIt == hSection.end() || !boost::iequals(hPropertyIt->first, "p-regex")) {
      NDN_THROW(Error("Expecting <checker.key-locator.hyper-relation.p-regex>"));
    }

    std::string pRegex = hPropertyIt->second.data();
    hPropertyIt++;

    // Get p-expand
    if (hPropertyIt == hSection.end() || !boost::iequals(hPropertyIt->first, "p-expand")) {
      NDN_THROW(Error("Expecting <checker.key-locator.hyper-relation.p-expand>"));
    }

    std::string pExpand = hPropertyIt->second.data();
    hPropertyIt++;

    if (hPropertyIt != hSection.end()) {
      NDN_THROW(Error("Expecting end of <checker.key-locator.hyper-relation>"));
    }

    NameRelation relation = getNameRelationFromString(hRelation);
    try {
      return make_unique<HyperRelationChecker>(pRegex, pExpand, kRegex, kExpand, relation);
    }
    catch (const Regex::Error&) {
      NDN_THROW_NESTED(Error("Invalid regex for <key-locator.hyper-relation>"));
    }
  }
  else {
    NDN_THROW(Error("Unrecognized <checker.key-locator>: " + propertyIt->first));
  }
}

} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#include "ndn-cxx/security/validator-config/checker.hpp"
#include "ndn-cxx/security/validation-policy.hpp"
#include "ndn-cxx/security/validation-state.hpp"
#include "ndn-cxx/security/verification-helpers.hpp"
#include "ndn-cxx/security/pib/key.hpp"

#include <boost/algorithm/string/predicate.hpp>

namespace ndn {
namespace security {
inline namespace v2 {
namespace validator_config {

Checker::Checker(tlv::SignatureTypeValue sigType)
  : m_sigType(sigType)
{
}

Checker::Result::Result(std::string error)
  : m_error(std::move(error))
{
}

class Checker::NegativeResultBuilder
{
public:
  template<typename T>
  NegativeResultBuilder&
  operator<<(const T& value)
  {
    m_ss << value;
    return *this;
  }

  operator Checker::Result() const
  {
    auto error = m_ss.str();
    return Checker::Result(error.empty() ? "checker failed" : std::move(error));
  }

private:
  std::ostringstream m_ss;
};

Checker::NegativeResultBuilder
Checker::reject()
{
  return NegativeResultBuilder();
}

Checker::Result
Checker::check(uint32_t pktType, tlv::SignatureTypeValue sigType, const Name& pktName, const Name& klName,
               const ValidationState& state)
{
  BOOST_ASSERT(pktType == tlv::Interest || pktType == tlv::Data);

  if (sigType != m_sigType) {
    return reject() << "signature type does not match the checker "
                    << sigType << " != " << m_sigType;
  }

  if (pktType == tlv::Interest) {
    auto fmt = state.getTag<SignedInterestFormatTag>();
    BOOST_ASSERT(fmt);

    if (*fmt == SignedInterestFormat::V03) {
      // This check is redundant if parameter digest checking is enabled. However, the parameter
      // digest checking can be disabled in API.
      if (pktName.size() == 0 || pktName[-1].type() != tlv::ParametersSha256DigestComponent) {
        return reject() << "ParametersSha256DigestComponent missing";
      }
      return checkNames(pktName.getPrefix(-1), klName);
    }
    else {
      if (pktName.size() < signed_interest::MIN_SIZE)
        return reject() << "name too short";

      return checkNames(pktName.getPrefix(-signed_interest::MIN_SIZE), klName);
    }
  }
  else {
    return checkNames(pktName, klName);
  }
}

Checker::Result
Checker::checkNames(const Name& pktName, const Name& klName)
{
  return accept();
}

NameRelationChecker::NameRelationChecker(tlv::SignatureTypeValue sigType, const Name& name, const NameRelation& relation)
  : Checker(sigType)
  , m_name(name)
  , m_relation(relation)
{
}

Checker::Result
NameRelationChecker::checkNames(const Name& pktName, const Name& klName)
{
  // pktName not used in this check
  Name identity = extractIdentityNameFromKeyLocator(klName);
  if (checkNameRelation(m_relation, m_name, identity)) {
    return accept();
  }

  return reject() << "identity " << identity << " and packet name do not satisfy "
                  << m_relation << " relation";
}

RegexChecker::RegexChecker(tlv::SignatureTypeValue sigType, const Regex& regex)
  : Checker(sigType)
  , m_regex(regex)
{
}

Checker::Result
RegexChecker::checkNames(const Name& pktName, const Name& klName)
{
  if (m_regex.match(klName)) {
    return accept();
  }

  return reject() << "KeyLocator does not match regex " << m_regex;
}

HyperRelationChecker::HyperRelationChecker(tlv::SignatureTypeValue sigType,
                                           const std::string& pktNameExpr, const std::string pktNameExpand,
                                           const std::string& klNameExpr, const std::string klNameExpand,
                                           const NameRelation& hyperRelation)
  : Checker(sigType)
  , m_hyperPRegex(pktNameExpr, pktNameExpand)
  , m_hyperKRegex(klNameExpr, klNameExpand)
  , m_hyperRelation(hyperRelation)
{
}

Checker::Result
HyperRelationChecker::checkNames(const Name& pktName, const Name& klName)
{
  if (!m_hyperPRegex.match(pktName)) {
    return reject() << "packet name does not match p-regex " << m_hyperPRegex;
  }

  if (!m_hyperKRegex.match(klName)) {
    return reject() << "KeyLocator does not match k-regex " << m_hyperKRegex;
  }

  auto kExpand = m_hyperKRegex.expand();
  auto pExpand = m_hyperPRegex.expand();
  if (checkNameRelation(m_hyperRelation, kExpand, pExpand)) {
    return accept();
  }

  return reject() << "expanded names " << kExpand << " and " << pExpand
                  << " do not satisfy " << m_hyperRelation << " relation";
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

static tlv::SignatureTypeValue
parseSigType(const std::string& value)
{
  if (boost::iequals(value, "rsa-sha256")) {
    return tlv::SignatureSha256WithRsa;
  }
  else if (boost::iequals(value, "ecdsa-sha256")) {
    return tlv::SignatureSha256WithEcdsa;
  }
  // TODO: uncomment when HMAC logic is defined/implemented
  // else if (boost::iequals(value, "hmac-sha256")) {
  //   return tlv::SignatureHmacWithSha256;
  // }
  else if (boost::iequals(value, "sha256")) {
    return tlv::DigestSha256;
  }
  else {
    NDN_THROW(Error("Unrecognized value of <checker.sig-type>: " + value));
  }
}

unique_ptr<Checker>
Checker::createCustomizedChecker(const ConfigSection& configSection,
                                 const std::string& configFilename)
{
  auto propertyIt = configSection.begin();
  propertyIt++;

  // assume that checker by default is for ecdsa-sha256, unless explicitly specified
  auto sigType = tlv::SignatureSha256WithEcdsa;

  if (propertyIt != configSection.end() && boost::iequals(propertyIt->first, "sig-type")) {
    sigType = parseSigType(propertyIt->second.data());
    propertyIt++;
  }

  if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "key-locator")) {
    if (sigType == tlv::DigestSha256) {
      // for sha256, key-locator is optional
      return make_unique<Checker>(sigType);
    }
    NDN_THROW(Error("Expecting <checker.key-locator>"));
  }

  auto checker = createKeyLocatorChecker(sigType, propertyIt->second, configFilename);
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

  // assume that checker by default is for ecdsa-sha256, unless explicitly specificied
  auto sigType = tlv::SignatureSha256WithEcdsa;

  if (propertyIt != configSection.end() && boost::iequals(propertyIt->first, "sig-type")) {
    sigType = parseSigType(propertyIt->second.data());
    propertyIt++;
  }

  if (propertyIt != configSection.end()) {
    NDN_THROW(Error("Expecting end of <checker>"));
  }
  return make_unique<HyperRelationChecker>(sigType,
                                           "^(<>*)$",             "\\1",
                                           "^(<>*)<KEY><>{1,3}$", "\\1",
                                           NameRelation::IS_PREFIX_OF);
}

unique_ptr<Checker>
Checker::createKeyLocatorChecker(tlv::SignatureTypeValue sigType,
                                 const ConfigSection& configSection, const std::string& configFilename)
{
  auto propertyIt = configSection.begin();

  // Get checker.key-locator.type
  if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "type"))
    NDN_THROW(Error("Expecting <checker.key-locator.type>"));

  std::string type = propertyIt->second.data();
  if (boost::iequals(type, "name"))
    return createKeyLocatorNameChecker(sigType, configSection, configFilename);
  else
    NDN_THROW(Error("Unrecognized <checker.key-locator.type>: " + type));
}

unique_ptr<Checker>
Checker::createKeyLocatorNameChecker(tlv::SignatureTypeValue sigType,
                                     const ConfigSection& configSection, const std::string& configFilename)
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
    return make_unique<NameRelationChecker>(sigType, name, relation);
  }
  else if (boost::iequals(propertyIt->first, "regex")) {
    std::string regexString = propertyIt->second.data();
    propertyIt++;

    if (propertyIt != configSection.end()) {
      NDN_THROW(Error("Expecting end of <checker.key-locator>"));
    }

    try {
      return make_unique<RegexChecker>(sigType, Regex(regexString));
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
      return make_unique<HyperRelationChecker>(sigType, pRegex, pExpand, kRegex, kExpand, relation);
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
} // inline namespace v2
} // namespace security
} // namespace ndn

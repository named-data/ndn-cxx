/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_SECURITY_V2_VALIDATOR_CONFIG_CHECKER_HPP
#define NDN_SECURITY_V2_VALIDATOR_CONFIG_CHECKER_HPP

#include "common.hpp"
#include "name-relation.hpp"
#include "../../../name.hpp"
#include "../../../util/regex.hpp"

namespace ndn {
namespace security {
namespace v2 {

class ValidationState;

namespace validator_config {

class Checker : noncopyable
{
public:
  virtual
  ~Checker() = default;

  /**
   * @brief Check if packet name ane KeyLocator satisfy the checker's conditions
   *
   * @param pktType tlv::Interest or tlv::Data
   * @param pktName packet's name
   * @param klName  KeyLocator's name
   * @param state Validation state
   *
   * @retval false data is immediately invalid. Will call state::fail() with proper code and message.
   * @retval true  further signature verification is needed.
   */
  bool
  check(uint32_t pktType, const Name& pktName, const Name& klName, const shared_ptr<ValidationState>& state);

  /**
   * @brief create a checker from configuration section
   *
   * @param configSection The section containing the definition of checker.
   * @param configFilename The configuration file name.
   * @return a checker created from configuration
   */
  static unique_ptr<Checker>
  create(const ConfigSection& configSection, const std::string& configFilename);

private:
  static unique_ptr<Checker>
  createCustomizedChecker(const ConfigSection& configSection, const std::string& configFilename);

  static unique_ptr<Checker>
  createHierarchicalChecker(const ConfigSection& configSection, const std::string& configFilename);

  static unique_ptr<Checker>
  createKeyLocatorChecker(const ConfigSection& configSection, const std::string& configFilename);

  static unique_ptr<Checker>
  createKeyLocatorNameChecker(const ConfigSection& configSection, const std::string& configFilename);

protected:
  virtual bool
  checkNames(const Name& pktName, const Name& klName, const shared_ptr<ValidationState>& state) = 0;
};

class NameRelationChecker : public Checker
{
public:
  NameRelationChecker(const Name& name, const NameRelation& relation);

protected:
  bool
  checkNames(const Name& pktName, const Name& klName, const shared_ptr<ValidationState>& state) override;

private:
  Name m_name;
  NameRelation m_relation;
};

class RegexChecker : public Checker
{
public:
  explicit
  RegexChecker(const Regex& regex);

protected:
  bool
  checkNames(const Name& pktName, const Name& klName, const shared_ptr<ValidationState>& state) override;

private:
  Regex m_regex;
};

class HyperRelationChecker : public Checker
{
public:
  HyperRelationChecker(const std::string& pktNameExpr, const std::string pktNameExpand,
                       const std::string& klNameExpr, const std::string klNameExpand,
                       const NameRelation& hyperRelation);

protected:
  bool
  checkNames(const Name& pktName, const Name& klName, const shared_ptr<ValidationState>& state) override;

private:
  Regex m_hyperPRegex;
  Regex m_hyperKRegex;
  NameRelation m_hyperRelation;
};

} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_VALIDATOR_CONFIG_CHECKER_HPP

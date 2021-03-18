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

#ifndef NDN_SECURITY_VALIDATOR_CONFIG_CHECKER_HPP
#define NDN_SECURITY_VALIDATOR_CONFIG_CHECKER_HPP

#include "ndn-cxx/name.hpp"
#include "ndn-cxx/security/validator-config/common.hpp"
#include "ndn-cxx/security/validator-config/name-relation.hpp"
#include "ndn-cxx/util/regex.hpp"

namespace ndn {
namespace security {
inline namespace v2 {

class ValidationState;

namespace validator_config {

class Checker : noncopyable
{
public:
  class Result
  {
  public:
    /**
     * @brief Return true if the check matches, false otherwise.
     * @retval false packet does not pass the checker.
     * @retval true  packet passes the checker. Further verification may be needed.
     */
    explicit operator bool() const
    {
      return m_error.empty();
    }

    /**
     * @brief Return checker error message.
     * @pre !bool(*this)
     */
    const std::string&
    getErrorMessage() const
    {
      return m_error;
    }

  private:
    explicit
    Result(std::string error);

  private:
    std::string m_error;

    friend Checker;
  };

  virtual
  ~Checker() = default;

  /**
   * @brief Check if packet name and KeyLocator satisfy the checker's conditions
   *
   * @param pktType tlv::Interest or tlv::Data
   * @param pktName packet's name
   * @param klName  KeyLocator's name
   * @param state Validation state
   */
  Result
  check(uint32_t pktType, const Name& pktName, const Name& klName, const ValidationState& state);

  /**
   * @brief create a checker from configuration section
   *
   * @param configSection The section containing the definition of checker.
   * @param configFilename The configuration file name.
   * @return a checker created from configuration
   */
  static unique_ptr<Checker>
  create(const ConfigSection& configSection, const std::string& configFilename);

protected:
  virtual Result
  checkNames(const Name& pktName, const Name& klName) = 0;

  static Result
  accept()
  {
    return Result("");
  }

  class NegativeResultBuilder;

  static NegativeResultBuilder
  reject();

private:
  static unique_ptr<Checker>
  createCustomizedChecker(const ConfigSection& configSection, const std::string& configFilename);

  static unique_ptr<Checker>
  createHierarchicalChecker(const ConfigSection& configSection, const std::string& configFilename);

  static unique_ptr<Checker>
  createKeyLocatorChecker(const ConfigSection& configSection, const std::string& configFilename);

  static unique_ptr<Checker>
  createKeyLocatorNameChecker(const ConfigSection& configSection, const std::string& configFilename);
};

class NameRelationChecker : public Checker
{
public:
  NameRelationChecker(const Name& name, const NameRelation& relation);

protected:
  Result
  checkNames(const Name& pktName, const Name& klName) override;

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
  Result
  checkNames(const Name& pktName, const Name& klName) override;

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
  Result
  checkNames(const Name& pktName, const Name& klName) override;

private:
  Regex m_hyperPRegex;
  Regex m_hyperKRegex;
  NameRelation m_hyperRelation;
};

} // namespace validator_config
} // inline namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_VALIDATOR_CONFIG_CHECKER_HPP

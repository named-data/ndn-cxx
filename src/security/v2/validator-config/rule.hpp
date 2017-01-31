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

#ifndef NDN_SECURITY_V2_VALIDATOR_CONFIG_RULE_HPP
#define NDN_SECURITY_V2_VALIDATOR_CONFIG_RULE_HPP

#include "filter.hpp"
#include "checker.hpp"

namespace ndn {
namespace security {
namespace v2 {

class ValidationState;

namespace validator_config {

class Rule : noncopyable
{
public:
  Rule(const std::string& id, uint32_t pktType);

  const std::string&
  getId() const
  {
    return m_id;
  }

  uint32_t
  getPktType() const
  {
    return m_pktType;
  }

  void
  addFilter(unique_ptr<Filter> filter);

  void
  addChecker(unique_ptr<Checker> checker);

  /**
   * @brief check if the packet name matches rule's filter
   *
   * If no filters were added, the rule matches everything.
   *
   * @param pktType tlv::Interest or tlv::Data
   * @param pktName packet name, for signed Interests the last two components are not removed
   * @retval true  If at least one filter matches @p pktName
   * @retval false If none of the filters match @p pktName
   *
   * @throw Error the supplied pktType doesn't match one for which the rule is designed
   */
  bool
  match(uint32_t pktType, const Name& pktName) const;

  /**
   * @brief check if packet satisfies rule's condition
   *
   * @param pktType tlv::Interest or tlv::Data
   * @param pktName packet name, for signed Interests the last two components are not removed
   * @param klName KeyLocator name
   * @param state Validation state
   *
   * @retval false packet violates at least one checker. Will call state::fail() with proper code and message.
   * @retval true  packet satisfies all checkers, further validation is needed
   *
   * @throw Error the supplied pktType doesn't match one for which the rule is designed
   */
  bool
  check(uint32_t pktType, const Name& pktName, const Name& klName, const shared_ptr<ValidationState>& state) const;

public:
  /**
   * @brief create a rule from configuration section
   *
   * @param configSection The section containing the definition of checker.
   * @param configFilename The configuration file name.
   * @return a rule created from configuration
   */
  static unique_ptr<Rule>
  create(const ConfigSection& configSection, const std::string& configFilename);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  std::string m_id;
  uint32_t m_pktType;
  std::vector<unique_ptr<Filter>> m_filters;
  std::vector<unique_ptr<Checker>> m_checkers;
};

} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_VALIDATOR_CONFIG_RULE_HPP

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

#ifndef NDN_SECURITY_V2_VALIDATOR_CONFIG_FILTER_HPP
#define NDN_SECURITY_V2_VALIDATOR_CONFIG_FILTER_HPP

#include "common.hpp"
#include "name-relation.hpp"
#include "../../../interest.hpp"
#include "../../../data.hpp"
#include "../../../util/regex.hpp"

namespace ndn {
namespace security {
namespace v2 {
namespace validator_config {

/**
 * @brief Filter is one of the classes used by ValidatorConfig.
 *
 * The ValidatorConfig class consists of a set of rules.
 * The Filter class is a part of a rule and is used to match packet.
 * Matched packets will be checked against the checkers defined in the rule.
 */
class Filter : noncopyable
{
public:
  virtual
  ~Filter() = default;

  bool
  match(uint32_t pktType, const Name& pktName);

public:
  /**
   * @brief Create a filter from the configuration section
   *
   * @param configSection The section containing the definition of filter.
   * @param configFilename The configuration file name.
   * @return a filter created from configuration
   */
  static unique_ptr<Filter>
  create(const ConfigSection& configSection, const std::string& configFilename);

private:
  static unique_ptr<Filter>
  createNameFilter(const ConfigSection& configSection, const std::string& configFilename);

private:
  virtual bool
  matchName(const Name& pktName) = 0;
};

/**
 * @brief Check that name is in relation to the packet name
 *
 * The following configuration
 * @code
 * filter
 * {
 *   type name
 *   name /example
 *   relation is-prefix-of
 * }
 * @endcode
 *
 * creates
 * @code
 * RelationNameFilter("/example", RelationNameFilter::RELATION_IS_PREFIX_OF);
 * @endcode
 */
class RelationNameFilter : public Filter
{
public:
  RelationNameFilter(const Name& name, NameRelation relation);

private:
  bool
  matchName(const Name& pktName) override;

private:
  Name m_name;
  NameRelation m_relation;
};

/**
 * @brief Filter to check that packet name matches the specified regular expression
 *
 * The following configuration
 * @code
 * filter
 * {
 *   type name
 *   regex ^[^<KEY>]*<KEY><>*<ksk-.*>$
 * }
 * @endcode
 *
 * creates
 * @code
 * RegexNameFilter("^[^<KEY>]*<KEY><>*<ksk-.*>$");
 * @endcode
 *
 * @sa Regex
 */
class RegexNameFilter : public Filter
{
public:
  explicit
  RegexNameFilter(const Regex& regex);

private:
  bool
  matchName(const Name& pktName) override;

private:
  Regex m_regex;
};

} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_VALIDATOR_CONFIG_FILTER_HPP

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

#ifndef NDN_SECURITY_V2_VALIDATOR_CONFIG_NAME_RELATION_HPP
#define NDN_SECURITY_V2_VALIDATOR_CONFIG_NAME_RELATION_HPP

#include "common.hpp"
#include "../../../name.hpp"

namespace ndn {
namespace security {
namespace v2 {
namespace validator_config {

enum class NameRelation {
  EQUAL,
  IS_PREFIX_OF,
  IS_STRICT_PREFIX_OF
};

std::ostream&
operator<<(std::ostream& os, NameRelation relation);

/**
 * @brief check whether @p name1 and @p name2 satisfies @p relation
 */
bool
checkNameRelation(NameRelation relation, const Name& name1, const Name& name2);

/**
 * @brief convert @p relationString to NameRelation
 * @throw Error if @p relationString cannot be converted
 */
NameRelation
getNameRelationFromString(const std::string& relationString);

} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_VALIDATOR_CONFIG_NAME_RELATION_HPP

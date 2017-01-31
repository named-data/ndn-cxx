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

#include "name-relation.hpp"

#include <boost/algorithm/string.hpp>

namespace ndn {
namespace security {
namespace v2 {
namespace validator_config {

std::ostream&
operator<<(std::ostream& os, NameRelation relation)
{
  switch (relation)  {
    case NameRelation::EQUAL:
      return os << "equal";
    case NameRelation::IS_PREFIX_OF:
      return os << "is-prefix-of";
    case NameRelation::IS_STRICT_PREFIX_OF:
      return os << "is-strict-prefix-of";
  }
  return os;
}

bool
checkNameRelation(NameRelation relation, const Name& name1, const Name& name2)
{
  switch (relation)  {
    case NameRelation::EQUAL:
      return name1 == name2;
    case NameRelation::IS_PREFIX_OF:
      return name1.isPrefixOf(name2);
    case NameRelation::IS_STRICT_PREFIX_OF:
      return name1.isPrefixOf(name2) && name1.size() < name2.size();
  }
  return false;
}

NameRelation
getNameRelationFromString(const std::string& relationString)
{
  if (boost::iequals(relationString, "equal")) {
    return NameRelation::EQUAL;
  }
  else if (boost::iequals(relationString, "is-prefix-of")) {
    return NameRelation::IS_PREFIX_OF;
  }
  else if (boost::iequals(relationString, "is-strict-prefix-of")) {
    return NameRelation::IS_STRICT_PREFIX_OF;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("Unsupported relation: " + relationString));
  }
}

} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn

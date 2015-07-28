/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#ifndef NDN_SECURITY_CONF_FILTER_HPP
#define NDN_SECURITY_CONF_FILTER_HPP

#include "../../common.hpp"
#include "../../data.hpp"
#include "../../interest.hpp"
#include "../../util/regex.hpp"
#include "../security-common.hpp"
#include <boost/algorithm/string.hpp>

#include "common.hpp"

namespace ndn {
namespace security {
namespace conf {

/**
 * @brief Filter is one of the classes used by ValidatorConfig.
 *
 * The ValidatorConfig class consists of a set of rules.
 * The Filter class is a part of a rule and is used to match packet.
 * Matched packets will be checked against the checkers defined in the rule.
 */

class Filter
{
public:

  virtual
  ~Filter()
  {
  }

  bool
  match(const Data& data)
  {
    return matchName(data.getName());
  }

  bool
  match(const Interest& interest)
  {
    if (interest.getName().size() < signed_interest::MIN_LENGTH)
      return false;

    Name unsignedName = interest.getName().getPrefix(-signed_interest::MIN_LENGTH);
    return matchName(unsignedName);
  }

protected:
  virtual bool
  matchName(const Name& name) = 0;
};

class RelationNameFilter : public Filter
{
public:
  enum Relation
    {
      RELATION_EQUAL,
      RELATION_IS_PREFIX_OF,
      RELATION_IS_STRICT_PREFIX_OF
    };

  RelationNameFilter(const Name& name, Relation relation)
    : m_name(name)
    , m_relation(relation)
  {
  }

  virtual
  ~RelationNameFilter()
  {
  }

protected:
  virtual bool
  matchName(const Name& name)
  {
    switch (m_relation)
      {
      case RELATION_EQUAL:
        return (name == m_name);
      case RELATION_IS_PREFIX_OF:
        return m_name.isPrefixOf(name);
      case RELATION_IS_STRICT_PREFIX_OF:
        return (m_name.isPrefixOf(name) && m_name.size() < name.size());
      default:
        return false;
      }
  }

private:
  Name m_name;
  Relation m_relation;
};

class RegexNameFilter : public Filter
{
public:
  explicit
  RegexNameFilter(const Regex& regex)
    : m_regex(regex)
  {
  }

  virtual
  ~RegexNameFilter()
  {
  }

protected:
  virtual bool
  matchName(const Name& name)
  {
    return m_regex.match(name);
  }

private:
  Regex m_regex;
};

class FilterFactory
{
public:
  static shared_ptr<Filter>
  create(const ConfigSection& configSection)
  {
    ConfigSection::const_iterator propertyIt = configSection.begin();

    if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "type"))
      BOOST_THROW_EXCEPTION(Error("Expect <filter.type>!"));

    std::string type = propertyIt->second.data();

    if (boost::iequals(type, "name"))
      return createNameFilter(configSection);
    else
      BOOST_THROW_EXCEPTION(Error("Unsupported filter.type: " + type));
  }
private:
  static shared_ptr<Filter>
  createNameFilter(const ConfigSection& configSection)
  {
    ConfigSection::const_iterator propertyIt = configSection.begin();
    propertyIt++;

    if (propertyIt == configSection.end())
      BOOST_THROW_EXCEPTION(Error("Expect more properties for filter(name)"));

    if (boost::iequals(propertyIt->first, "name"))
      {
        // Get filter.name
        Name name;
        try
          {
            name = Name(propertyIt->second.data());
          }
        catch (Name::Error& e)
          {
            BOOST_THROW_EXCEPTION(Error("Wrong filter.name: " + propertyIt->second.data()));
          }

        propertyIt++;

        // Get filter.relation
        if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "relation"))
          BOOST_THROW_EXCEPTION(Error("Expect <filter.relation>!"));

        std::string relationString = propertyIt->second.data();
        propertyIt++;

        RelationNameFilter::Relation relation;
        if (boost::iequals(relationString, "equal"))
          relation = RelationNameFilter::RELATION_EQUAL;
        else if (boost::iequals(relationString, "is-prefix-of"))
          relation = RelationNameFilter::RELATION_IS_PREFIX_OF;
        else if (boost::iequals(relationString, "is-strict-prefix-of"))
          relation = RelationNameFilter::RELATION_IS_STRICT_PREFIX_OF;
        else
          BOOST_THROW_EXCEPTION(Error("Unsupported relation: " + relationString));


        if (propertyIt != configSection.end())
          BOOST_THROW_EXCEPTION(Error("Expect the end of filter!"));

        return make_shared<RelationNameFilter>(name, relation);
      }
    else if (boost::iequals(propertyIt->first, "regex"))
      {
        std::string regexString = propertyIt->second.data();
        propertyIt++;

        if (propertyIt != configSection.end())
          BOOST_THROW_EXCEPTION(Error("Expect the end of filter!"));

        try
          {
            return shared_ptr<RegexNameFilter>(new RegexNameFilter(regexString));
          }
        catch (Regex::Error& e)
          {
            BOOST_THROW_EXCEPTION(Error("Wrong filter.regex: " + regexString));
          }
      }
    else
      BOOST_THROW_EXCEPTION(Error("Wrong filter(name) properties"));
  }
};

} // namespace conf
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_CONF_FILTER_HPP

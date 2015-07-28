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

#ifndef NDN_SECURITY_CONF_KEY_LOCATOR_CHECKER_HPP
#define NDN_SECURITY_CONF_KEY_LOCATOR_CHECKER_HPP

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

class KeyLocatorCheckerFactory;

/**
 * @brief KeyLocatorChecker is one of the classes used by ValidatorConfig.
 *
 * The ValidatorConfig class consists of a set of rules.
 * The KeyLocatorChecker class is part of a rule and is used to check if the KeyLocator field of a
 * packet satisfy the requirements.
 */


class KeyLocatorChecker
{
public:
  enum Relation {
    RELATION_EQUAL,
    RELATION_IS_PREFIX_OF,
    RELATION_IS_STRICT_PREFIX_OF
  };

  virtual
  ~KeyLocatorChecker()
  {
  }

  bool
  check(const Data& data,
        const KeyLocator& keyLocator,
        std::string& failInfo)
  {
    return check(data.getName(), keyLocator, failInfo);
  }

  bool
  check(const Interest& interest,
        const KeyLocator& keyLocator,
        std::string& failInfo)
  {
    if (interest.getName().size() < signed_interest::MIN_LENGTH)
      {
        failInfo = "No Signature";
        return false;
      }

    Name signedName = interest.getName().getPrefix(-signed_interest::MIN_LENGTH);
    return check(signedName, keyLocator, failInfo);
  }

protected:

  virtual bool
  check(const Name& packetName,
        const KeyLocator& keyLocator,
        std::string& failInfo) = 0;

  bool
  checkRelation(const Relation& relation, const Name& name1, const Name& name2)
  {
    switch (relation)
      {
      case RELATION_EQUAL:
        return (name1 == name2);
      case RELATION_IS_PREFIX_OF:
        return name1.isPrefixOf(name2);
      case RELATION_IS_STRICT_PREFIX_OF:
        return (name1.isPrefixOf(name2) && name1 != name2);
      default:
        return false;
      }
  }
};

class RelationKeyLocatorNameChecker : public KeyLocatorChecker
{
public:
  RelationKeyLocatorNameChecker(const Name& name,
                                const KeyLocatorChecker::Relation& relation)
    : m_name(name)
    , m_relation(relation)
  {
  }

protected:
  virtual bool
  check(const Name& packetName,
        const KeyLocator& keyLocator,
        std::string& failInfo)
  {
    try
      {
        if (checkRelation(m_relation, m_name, keyLocator.getName()))
          return true;

        failInfo = "KeyLocatorChecker failed!";
        return false;
      }
    catch (KeyLocator::Error& e)
      {
        failInfo = "KeyLocator does not have name";
        return false;
      }
  }

private:
  Name m_name;
  KeyLocatorChecker::Relation m_relation;
};

class RegexKeyLocatorNameChecker : public KeyLocatorChecker
{
public:
  explicit
  RegexKeyLocatorNameChecker(const Regex& regex)
    : m_regex(regex)
  {
  }

protected:
  virtual bool
  check(const Name& packetName,
        const KeyLocator& keyLocator,
        std::string& failInfo)
  {
    try
      {
        if (m_regex.match(keyLocator.getName()))
          return true;

        failInfo = "KeyLocatorChecker failed!";
        return false;
      }
    catch (KeyLocator::Error& e)
      {
        failInfo = "KeyLocator does not have name";
        return false;
      }
  }

private:
  Regex m_regex;
};

class HyperKeyLocatorNameChecker : public KeyLocatorChecker
{
public:
  HyperKeyLocatorNameChecker(const std::string& pExpr, const std::string pExpand,
                             const std::string& kExpr, const std::string kExpand,
                             const Relation& hyperRelation)
    : m_hyperPRegex(new Regex(pExpr, pExpand))
    , m_hyperKRegex(new Regex(kExpr, kExpand))
    , m_hyperRelation(hyperRelation)
  {
  }

protected:
  virtual bool
  check(const Name& packetName,
        const KeyLocator& keyLocator,
        std::string& failInfo)
  {
    try
      {
        if (m_hyperPRegex->match(packetName) &&
            m_hyperKRegex->match(keyLocator.getName()) &&
            checkRelation(m_hyperRelation,
                          m_hyperKRegex->expand(),
                          m_hyperPRegex->expand()))
          return true;

        failInfo = "KeyLocatorChecker failed!";
        return false;
      }
    catch (KeyLocator::Error& e)
      {
        failInfo = "KeyLocator does not have name";
        return false;
      }

  }

private:
  shared_ptr<Regex> m_hyperPRegex;
  shared_ptr<Regex> m_hyperKRegex;
  Relation m_hyperRelation;
};


class KeyLocatorCheckerFactory
{
public:
  static shared_ptr<KeyLocatorChecker>
  create(const ConfigSection& configSection, const std::string& filename)
  {
    ConfigSection::const_iterator propertyIt = configSection.begin();

    // Get checker.key-locator.type
    if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "type"))
      BOOST_THROW_EXCEPTION(Error("Expect <checker.key-locator.type>!"));

    std::string type = propertyIt->second.data();

    if (boost::iequals(type, "name"))
      return createKeyLocatorNameChecker(configSection, filename);
    else
      BOOST_THROW_EXCEPTION(Error("Unsupported checker.key-locator.type: " + type));
  }

private:
  static shared_ptr<KeyLocatorChecker>
  createKeyLocatorNameChecker(const ConfigSection& configSection,
                              const std::string& filename)
  {
    ConfigSection::const_iterator propertyIt = configSection.begin();
    propertyIt++;

    if (propertyIt == configSection.end())
      BOOST_THROW_EXCEPTION(Error("Expect more checker.key-locator properties"));

    if (boost::iequals(propertyIt->first, "name"))
      {
        Name name;
        try
          {
            name = Name(propertyIt->second.data());
          }
        catch (Name::Error& e)
          {
            BOOST_THROW_EXCEPTION(Error("Invalid checker.key-locator.name: " +
                                        propertyIt->second.data()));
          }
        propertyIt++;

        if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "relation"))
          BOOST_THROW_EXCEPTION(Error("Expect <checker.key-locator.relation>!"));

        std::string relationString = propertyIt->second.data();
        propertyIt++;

        KeyLocatorChecker::Relation relation;
        if (boost::iequals(relationString, "equal"))
          relation = KeyLocatorChecker::RELATION_EQUAL;
        else if (boost::iequals(relationString, "is-prefix-of"))
          relation = KeyLocatorChecker::RELATION_IS_PREFIX_OF;
        else if (boost::iequals(relationString, "is-strict-prefix-of"))
          relation = KeyLocatorChecker::RELATION_IS_STRICT_PREFIX_OF;
        else
          BOOST_THROW_EXCEPTION(Error("Unsupported relation: " + relationString));

        if (propertyIt != configSection.end())
          BOOST_THROW_EXCEPTION(Error("Expect the end of checker.key-locator!"));

        return shared_ptr<RelationKeyLocatorNameChecker>
          (new RelationKeyLocatorNameChecker(name, relation));
      }
    else if (boost::iequals(propertyIt->first, "regex"))
      {
        std::string regexString = propertyIt->second.data();
        propertyIt++;

        if (propertyIt != configSection.end())
          BOOST_THROW_EXCEPTION(Error("Expect the end of checker.key-locator!"));

        try
          {
            return shared_ptr<RegexKeyLocatorNameChecker>
              (new RegexKeyLocatorNameChecker(regexString));
          }
        catch (Regex::Error& e)
          {
            BOOST_THROW_EXCEPTION(Error("Invalid checker.key-locator.regex: " + regexString));
          }
      }
    else if (boost::iequals(propertyIt->first, "hyper-relation"))
      {
        const ConfigSection& hSection = propertyIt->second;

        ConfigSection::const_iterator hPropertyIt = hSection.begin();

        // Get k-regex
        if (hPropertyIt == hSection.end() || !boost::iequals(hPropertyIt->first, "k-regex"))
          BOOST_THROW_EXCEPTION(Error("Expect <checker.key-locator.hyper-relation.k-regex>!"));

        std::string kRegex = hPropertyIt->second.data();
        hPropertyIt++;

        // Get k-expand
        if (hPropertyIt == hSection.end() || !boost::iequals(hPropertyIt->first, "k-expand"))
          BOOST_THROW_EXCEPTION(Error("Expect <checker.key-locator.hyper-relation.k-expand>!"));

        std::string kExpand = hPropertyIt->second.data();
        hPropertyIt++;

        // Get h-relation
        if (hPropertyIt == hSection.end() || !boost::iequals(hPropertyIt->first, "h-relation"))
          BOOST_THROW_EXCEPTION(Error("Expect <checker.key-locator.hyper-relation.h-relation>!"));

        std::string hRelation = hPropertyIt->second.data();
        hPropertyIt++;

        // Get p-regex
        if (hPropertyIt == hSection.end() || !boost::iequals(hPropertyIt->first, "p-regex"))
          BOOST_THROW_EXCEPTION(Error("Expect <checker.key-locator.hyper-relation.p-regex>!"));

        std::string pRegex = hPropertyIt->second.data();
        hPropertyIt++;

        // Get p-expand
        if (hPropertyIt == hSection.end() || !boost::iequals(hPropertyIt->first, "p-expand"))
          BOOST_THROW_EXCEPTION(Error("Expect <checker.key-locator.hyper-relation.p-expand>!"));

        std::string pExpand = hPropertyIt->second.data();
        hPropertyIt++;

        if (hPropertyIt != hSection.end())
          BOOST_THROW_EXCEPTION(Error("Expect the end of checker.key-locator.hyper-relation!"));

        KeyLocatorChecker::Relation relation;
        if (boost::iequals(hRelation, "equal"))
          relation = KeyLocatorChecker::RELATION_EQUAL;
        else if (boost::iequals(hRelation, "is-prefix-of"))
          relation = KeyLocatorChecker::RELATION_IS_PREFIX_OF;
        else if (boost::iequals(hRelation, "is-strict-prefix-of"))
          relation = KeyLocatorChecker::RELATION_IS_STRICT_PREFIX_OF;
        else
          BOOST_THROW_EXCEPTION(Error("Unsupported checker.key-locator.hyper-relation.h-relation: "
                                      + hRelation));

        try
          {
            return shared_ptr<HyperKeyLocatorNameChecker>
              (new HyperKeyLocatorNameChecker(pRegex, pExpand,
                                              kRegex, kExpand,
                                              relation));
          }
        catch (Regex::Error& e)
          {
            BOOST_THROW_EXCEPTION(Error("Invalid regex for key-locator.hyper-relation"));
          }
      }
    else
      BOOST_THROW_EXCEPTION(Error("Unsupported checker.key-locator"));
  }
};


} // namespace conf
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_CONF_KEY_LOCATOR_CHECKER_HPP

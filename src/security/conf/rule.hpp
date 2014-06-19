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

#ifndef NDN_SECURITY_CONF_RULE_HPP
#define NDN_SECURITY_CONF_RULE_HPP

#include "filter.hpp"
#include "checker.hpp"


namespace ndn {
namespace security {
namespace conf {

template<class Packet>
class Rule
{
public:
  explicit
  Rule(const std::string& id)
    : m_id(id)
  {
  }

  virtual
  ~Rule()
  {
  }

  const std::string&
  getId()
  {
    return m_id;
  }

  void
  addFilter(const shared_ptr<Filter>& filter)
  {
    m_filters.push_back(filter);
  }

  void
  addChecker(const shared_ptr<Checker>& checker)
  {
    m_checkers.push_back(checker);
  }

  bool
  match(const Packet& packet)
  {
    if (m_filters.empty())
      return true;

    for (FilterList::iterator it = m_filters.begin();
         it != m_filters.end(); it++)
      {
        if (!(*it)->match(packet))
          return false;
      }

    return true;
  }

  /**
   * @brief check if packet satisfies certain condition
   *
   * @param packet The packet
   * @param onValidated Callback function which is called when packet is immediately valid
   * @param onValidationFailed Call function which is called when packet is immediately invalid
   * @return -1 if packet is immediately invalid (onValidationFailed has been called)
   *          1 if packet is immediately valid (onValidated has been called)
   *          0 if further signature verification is needed.
   */
  template<class ValidatedCallback, class ValidationFailureCallback>
  int8_t
  check(const Packet& packet,
        const ValidatedCallback& onValidated,
        const ValidationFailureCallback& onValidationFailed)
  {
    for (CheckerList::iterator it = m_checkers.begin();
         it != m_checkers.end(); it++)
      {
        int8_t result = (*it)->check(packet, onValidated, onValidationFailed);
        if (result >= 0)
          return result;
      }
    return -1;
  }

private:
  typedef std::vector<shared_ptr<Filter> > FilterList;
  typedef std::vector<shared_ptr<Checker> > CheckerList;

  std::string m_id;
  FilterList m_filters;
  CheckerList m_checkers;
};

} // namespace conf
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_CONF_RULE_HPP

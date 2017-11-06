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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#include "regex-repeat-matcher.hpp"
#include "regex-backref-matcher.hpp"
#include "regex-component-set-matcher.hpp"

#include <boost/regex.hpp>
#include <cstdlib>

namespace ndn {

RegexRepeatMatcher::RegexRepeatMatcher(const std::string& expr,
                                       shared_ptr<RegexBackrefManager> backrefManager,
                                       size_t indicator)
  : RegexMatcher(expr, EXPR_REPEAT_PATTERN, std::move(backrefManager))
  , m_indicator(indicator)
{
  compile();
}

void
RegexRepeatMatcher::compile()
{
  if ('(' == m_expr[0]) {
    auto matcher = make_shared<RegexBackrefMatcher>(m_expr.substr(0, m_indicator), m_backrefManager);
    m_backrefManager->pushRef(matcher);
    matcher->lateCompile();
    m_matchers.push_back(std::move(matcher));
  }
  else {
    m_matchers.push_back(make_shared<RegexComponentSetMatcher>(m_expr.substr(0, m_indicator),
                                                               m_backrefManager));
  }

  parseRepetition();
}

bool
RegexRepeatMatcher::parseRepetition()
{
  size_t exprSize = m_expr.size();
  const size_t MAX_REPETITIONS = std::numeric_limits<size_t>::max();

  if (exprSize == m_indicator) {
    m_repeatMin = 1;
    m_repeatMax = 1;
    return true;
  }

  if (exprSize == (m_indicator + 1)) {
    if ('?' == m_expr[m_indicator]) {
      m_repeatMin = 0;
      m_repeatMax = 1;
      return true;
    }
    if ('+' == m_expr[m_indicator]) {
      m_repeatMin = 1;
      m_repeatMax = MAX_REPETITIONS;
      return true;
    }
    if ('*' == m_expr[m_indicator]) {
      m_repeatMin = 0;
      m_repeatMax = MAX_REPETITIONS;
      return true;
    }
  }
  else {
    std::string repeatStruct = m_expr.substr(m_indicator, exprSize - m_indicator);
    size_t rsSize = repeatStruct.size();
    size_t min = 0;
    size_t max = 0;

    if (boost::regex_match(repeatStruct, boost::regex("\\{[0-9]+,[0-9]+\\}"))) {
      size_t separator = repeatStruct.find_first_of(',', 0);
      min = std::atoi(repeatStruct.substr(1, separator - 1).data());
      max = std::atoi(repeatStruct.substr(separator + 1, rsSize - separator - 2).data());
    }
    else if (boost::regex_match(repeatStruct, boost::regex("\\{,[0-9]+\\}"))) {
      size_t separator = repeatStruct.find_first_of(',', 0);
      min = 0;
      max = std::atoi(repeatStruct.substr(separator + 1, rsSize - separator - 2).data());
    }
    else if (boost::regex_match(repeatStruct, boost::regex("\\{[0-9]+,\\}"))) {
      size_t separator = repeatStruct.find_first_of(',', 0);
      min = std::atoi(repeatStruct.substr(1, separator).data());
      max = MAX_REPETITIONS;
    }
    else if (boost::regex_match(repeatStruct, boost::regex("\\{[0-9]+\\}"))) {
      min = std::atoi(repeatStruct.substr(1, rsSize - 1).data());
      max = min;
    }
    else
      BOOST_THROW_EXCEPTION(Error(std::string("Error: RegexRepeatMatcher.ParseRepetition():")
                                  + " Unrecognized format " + m_expr));

    if (min > MAX_REPETITIONS || max > MAX_REPETITIONS || min > max)
      BOOST_THROW_EXCEPTION(Error(std::string("Error: RegexRepeatMatcher.ParseRepetition():")
                                  + " Wrong number " + m_expr));

    m_repeatMin = min;
    m_repeatMax = max;

    return true;
  }

  return false;
}

bool
RegexRepeatMatcher::match(const Name& name, size_t offset, size_t len)
{
  m_matchResult.clear();

  if (m_repeatMin == 0)
    if (len == 0)
      return true;

  if (recursiveMatch(0, name, offset, len)) {
    for (size_t i = offset; i < offset + len; i++)
      m_matchResult.push_back(name.get(i));
    return true;
  }

  return false;
}

bool
RegexRepeatMatcher::recursiveMatch(size_t repeat, const Name& name, size_t offset, size_t len)
{
  ssize_t tried = len;

  if (0 < len && repeat >= m_repeatMax) {
    return false;
  }

  if (0 == len && repeat < m_repeatMin) {
    return false;
  }

  if (0 == len && repeat >= m_repeatMin) {
    return true;
  }

  auto matcher = m_matchers[0];
  while (tried >= 0) {
    if (matcher->match(name, offset, tried) &&
        recursiveMatch(repeat + 1, name, offset + tried, len - tried))
      return true;
    tried--;
  }

  return false;
}

} // namespace ndn

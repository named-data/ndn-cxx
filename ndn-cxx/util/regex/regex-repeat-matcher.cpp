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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#include "ndn-cxx/util/regex/regex-repeat-matcher.hpp"
#include "ndn-cxx/util/regex/regex-backref-matcher.hpp"
#include "ndn-cxx/util/regex/regex-component-set-matcher.hpp"

#include <regex>

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
    auto matcher = make_shared<RegexBackrefMatcher>(m_expr.substr(0, m_indicator),
                                                    m_backrefManager);
    m_backrefManager->pushRef(matcher);
    matcher->compile();
    m_matchers.push_back(std::move(matcher));
  }
  else {
    m_matchers.push_back(make_shared<RegexComponentSetMatcher>(m_expr.substr(0, m_indicator),
                                                               m_backrefManager));
  }

  parseRepetition();
}

void
RegexRepeatMatcher::parseRepetition()
{
  constexpr size_t MAX_REPETITIONS = std::numeric_limits<size_t>::max();
  size_t exprSize = m_expr.size();

  if (exprSize == m_indicator) {
    m_repeatMin = 1;
    m_repeatMax = 1;
  }
  else if (exprSize == m_indicator + 1) {
    switch (m_expr[m_indicator]) {
    case '?':
      m_repeatMin = 0;
      m_repeatMax = 1;
      break;
    case '+':
      m_repeatMin = 1;
      m_repeatMax = MAX_REPETITIONS;
      break;
    case '*':
      m_repeatMin = 0;
      m_repeatMax = MAX_REPETITIONS;
      break;
    default:
      NDN_THROW(Error("Unrecognized quantifier '"s + m_expr[m_indicator] + "' in regex: " + m_expr));
    }
  }
  else {
    std::string repeatStruct = m_expr.substr(m_indicator, exprSize - m_indicator);
    size_t rsSize = repeatStruct.size();

    try {
      if (std::regex_match(repeatStruct, std::regex("\\{[0-9]+,[0-9]+\\}"))) {
        size_t separator = repeatStruct.find_first_of(',', 0);
        m_repeatMin = std::stoul(repeatStruct.substr(1, separator - 1));
        m_repeatMax = std::stoul(repeatStruct.substr(separator + 1, rsSize - separator - 2));
        if (m_repeatMin > m_repeatMax) {
          NDN_THROW(Error("Invalid number of repetitions '" + repeatStruct + "' in regex: " + m_expr));
        }
      }
      else if (std::regex_match(repeatStruct, std::regex("\\{,[0-9]+\\}"))) {
        size_t separator = repeatStruct.find_first_of(',', 0);
        m_repeatMin = 0;
        m_repeatMax = std::stoul(repeatStruct.substr(separator + 1, rsSize - separator - 2));
      }
      else if (std::regex_match(repeatStruct, std::regex("\\{[0-9]+,\\}"))) {
        size_t separator = repeatStruct.find_first_of(',', 0);
        m_repeatMin = std::stoul(repeatStruct.substr(1, separator));
        m_repeatMax = MAX_REPETITIONS;
      }
      else if (std::regex_match(repeatStruct, std::regex("\\{[0-9]+\\}"))) {
        m_repeatMin = std::stoul(repeatStruct.substr(1, rsSize - 1));
        m_repeatMax = m_repeatMin;
      }
      else {
        NDN_THROW(Error("Invalid quantifier '" + repeatStruct + "' in regex: " + m_expr));
      }
    }
    // std::stoul can throw invalid_argument or out_of_range, both are derived from logic_error
    catch (const std::logic_error&) {
      NDN_THROW_NESTED(Error("Invalid number of repetitions '" + repeatStruct + "' in regex: " + m_expr));
    }
  }
}

bool
RegexRepeatMatcher::match(const Name& name, size_t offset, size_t len)
{
  m_matchResult.clear();

  if (m_repeatMin == 0 && len == 0) {
    return true;
  }

  if (recursiveMatch(0, name, offset, len)) {
    for (size_t i = offset; i < offset + len; i++) {
      m_matchResult.push_back(name.get(i));
    }
    return true;
  }

  return false;
}

bool
RegexRepeatMatcher::recursiveMatch(size_t repeat, const Name& name, size_t offset, size_t len)
{
  if (0 < len && repeat >= m_repeatMax) {
    return false;
  }

  if (0 == len && repeat < m_repeatMin) {
    return false;
  }

  if (0 == len && repeat >= m_repeatMin) {
    return true;
  }

  const auto& matcher = m_matchers[0];
  ssize_t tried = static_cast<ssize_t>(len);
  while (tried >= 0) {
    if (matcher->match(name, offset, tried) &&
        recursiveMatch(repeat + 1, name, offset + tried, len - tried)) {
      return true;
    }
    tried--;
  }

  return false;
}

} // namespace ndn

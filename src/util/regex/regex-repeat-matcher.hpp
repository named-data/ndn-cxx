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

#ifndef NDN_UTIL_REGEX_REGEX_REPEAT_MATCHER_HPP
#define NDN_UTIL_REGEX_REGEX_REPEAT_MATCHER_HPP

#include "../../common.hpp"

#include <boost/regex.hpp>

#include "regex-matcher.hpp"

namespace ndn {

class RegexRepeatMatcher : public RegexMatcher
{
public:
  RegexRepeatMatcher(const std::string& expr,
                     shared_ptr<RegexBackrefManager> backrefManager,
                     size_t indicator);

  virtual
  ~RegexRepeatMatcher()
  {
  }

  virtual bool
  match(const Name& name, size_t offset, size_t len);

protected:
  /**
   * @brief Compile the regular expression to generate the more matchers when necessary
   * @returns true if compiling succeeds
   */
  virtual void
  compile();

private:
  bool
  parseRepetition();

  bool
  recursiveMatch(size_t repeat,
                 const Name& name,
                 size_t offset, size_t len);

private:
  size_t m_indicator;
  size_t m_repeatMin;
  size_t m_repeatMax;
};

} // namespace ndn

#include "regex-backref-matcher.hpp"
#include "regex-component-set-matcher.hpp"

namespace ndn {

inline
RegexRepeatMatcher::RegexRepeatMatcher(const std::string& expr,
                                       shared_ptr<RegexBackrefManager> backrefManager,
                                       size_t indicator)
  : RegexMatcher(expr, EXPR_REPEAT_PATTERN, backrefManager)
  , m_indicator(indicator)
{
  compile();
}

inline void
RegexRepeatMatcher::compile()
{
  shared_ptr<RegexMatcher> matcher;

  if ('(' == m_expr[0]) {
    matcher = make_shared<RegexBackrefMatcher>(m_expr.substr(0, m_indicator), m_backrefManager);
    m_backrefManager->pushRef(matcher);
    dynamic_pointer_cast<RegexBackrefMatcher>(matcher)->lateCompile();
  }
  else{
    matcher = make_shared<RegexComponentSetMatcher>(m_expr.substr(0, m_indicator),
                                                    m_backrefManager);
  }
  m_matchers.push_back(matcher);

  parseRepetition();
}

inline bool
RegexRepeatMatcher::parseRepetition()
{
  size_t exprSize = m_expr.size();
  const size_t MAX_REPETITIONS = std::numeric_limits<size_t>::max();

  if (exprSize == m_indicator) {
    m_repeatMin = 1;
    m_repeatMax = 1;

    return true;
  }
  else {
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
        min = atoi(repeatStruct.substr(1, separator - 1).c_str());
        max = atoi(repeatStruct.substr(separator + 1, rsSize - separator - 2).c_str());
      }
      else if (boost::regex_match(repeatStruct, boost::regex("\\{,[0-9]+\\}"))) {
        size_t separator = repeatStruct.find_first_of(',', 0);
        min = 0;
        max = atoi(repeatStruct.substr(separator + 1, rsSize - separator - 2).c_str());
      }
      else if (boost::regex_match(repeatStruct, boost::regex("\\{[0-9]+,\\}"))) {
        size_t separator = repeatStruct.find_first_of(',', 0);
        min = atoi(repeatStruct.substr(1, separator).c_str());
        max = MAX_REPETITIONS;
      }
      else if (boost::regex_match(repeatStruct, boost::regex("\\{[0-9]+\\}"))) {
        min = atoi(repeatStruct.substr(1, rsSize - 1).c_str());
        max = min;
      }
      else
        BOOST_THROW_EXCEPTION(RegexMatcher::Error(std::string("Error: RegexRepeatMatcher.ParseRepetition():")
                                                  + " Unrecognized format "+ m_expr));

      if (min > MAX_REPETITIONS || max > MAX_REPETITIONS || min > max)
        BOOST_THROW_EXCEPTION(RegexMatcher::Error(std::string("Error: RegexRepeatMatcher.ParseRepetition():")
                                                  + " Wrong number " + m_expr));

      m_repeatMin = min;
      m_repeatMax = max;

      return true;
    }
  }
  return false;
}

inline bool
RegexRepeatMatcher::match(const Name& name, size_t offset, size_t len)
{
  m_matchResult.clear();

  if (0 == m_repeatMin)
    if (0 == len)
      return true;

  if (recursiveMatch(0, name, offset, len))
    {
      for (size_t i = offset; i < offset + len; i++)
        m_matchResult.push_back(name.get(i));
      return true;
    }
  else
    return false;
}

inline bool
RegexRepeatMatcher::recursiveMatch(size_t repeat, const Name& name, size_t offset, size_t len)
{
  ssize_t tried = len;
  shared_ptr<RegexMatcher> matcher = m_matchers[0];

  if (0 < len && repeat >= m_repeatMax)
    {
      return false;
    }

  if (0 == len && repeat < m_repeatMin)
    {
      return false;
    }

  if (0 == len && repeat >= m_repeatMin)
    {
      return true;
    }

  while (tried >= 0)
    {
      if (matcher->match(name, offset, tried) &&
          recursiveMatch(repeat + 1, name, offset + tried, len - tried))
        return true;
      tried--;
    }

  return false;
}


} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_REPEAT_MATCHER_HPP

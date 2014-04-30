/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
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
                     shared_ptr<RegexBackrefManager> backRefManager,
                     int indicator);

  virtual
  ~RegexRepeatMatcher(){}

  virtual bool
  match(const Name& name, const int& offset, const int& len);

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
  recursiveMatch (int repeat,
                  const Name& name,
                  const int& offset,
                  const int&len);

private:
  int m_indicator;
  int m_repeatMin;
  int m_repeatMax;
};

} // namespace ndn

#include "regex-backref-matcher.hpp"
#include "regex-component-set-matcher.hpp"

namespace ndn {

inline
RegexRepeatMatcher::RegexRepeatMatcher(const std::string& expr,
                                       shared_ptr<RegexBackrefManager> backrefManager,
                                       int indicator)
  : RegexMatcher (expr, EXPR_REPEAT_PATTERN, backrefManager)
  , m_indicator(indicator)
{
  // _LOG_TRACE ("Enter RegexRepeatMatcher Constructor");
  compile();
  // _LOG_TRACE ("Exit RegexRepeatMatcher Constructor");
}

inline void
RegexRepeatMatcher::compile()
{
  // _LOG_TRACE ("Enter RegexRepeatMatcher::compile");

  shared_ptr<RegexMatcher> matcher;

  if ('(' == m_expr[0]){
    matcher = make_shared<RegexBackrefMatcher>(m_expr.substr(0, m_indicator), m_backrefManager);
    m_backrefManager->pushRef(matcher);
    dynamic_pointer_cast<RegexBackrefMatcher>(matcher)->lateCompile();
  }
  else{
    matcher = make_shared<RegexComponentSetMatcher>(m_expr.substr(0, m_indicator),
                                                    m_backrefManager);
  }
  m_matcherList.push_back(matcher);

  parseRepetition();

  // _LOG_TRACE ("Exit RegexRepeatMatcher::compile");

}

inline bool
RegexRepeatMatcher::parseRepetition()
{
  int exprSize = m_expr.size();
  int intMax = std::numeric_limits<int>::max();

  if (exprSize == m_indicator){
    m_repeatMin = 1;
    m_repeatMax = 1;

    return true;
  }
  else{
    if (exprSize == (m_indicator + 1)){
      if ('?' == m_expr[m_indicator]){
        m_repeatMin = 0;
        m_repeatMax = 1;
        return true;
      }
      if ('+' == m_expr[m_indicator]){
        m_repeatMin = 1;
        m_repeatMax = intMax;
        return true;
      }
      if ('*' == m_expr[m_indicator]){
        m_repeatMin = 0;
        m_repeatMax = intMax;
        return true;
      }
    }
    else{
      std::string repeatStruct = m_expr.substr(m_indicator, exprSize - m_indicator);
      int rsSize = repeatStruct.size();
      int min = 0;
      int max = 0;

      if (boost::regex_match(repeatStruct, boost::regex("\\{[0-9]+,[0-9]+\\}"))){
        int separator = repeatStruct.find_first_of(',', 0);
        min = atoi(repeatStruct.substr(1, separator - 1).c_str());
        max = atoi(repeatStruct.substr(separator + 1, rsSize - separator - 2).c_str());
      }
      else if (boost::regex_match(repeatStruct, boost::regex("\\{,[0-9]+\\}"))){
        int separator = repeatStruct.find_first_of(',', 0);
        min = 0;
        max = atoi(repeatStruct.substr(separator + 1, rsSize - separator - 2).c_str());
      }
      else if (boost::regex_match(repeatStruct, boost::regex("\\{[0-9]+,\\}"))){
        int separator = repeatStruct.find_first_of(',', 0);
        min = atoi(repeatStruct.substr(1, separator).c_str());
        max = intMax;
      }
      else if (boost::regex_match(repeatStruct, boost::regex("\\{[0-9]+\\}"))){
        min = atoi(repeatStruct.substr(1, rsSize - 1).c_str());
        max = min;
      }
      else
        throw RegexMatcher::Error(std::string("Error: RegexRepeatMatcher.ParseRepetition(): ")
                                  + "Unrecognized format "+ m_expr);

      if (min > intMax || max > intMax || min > max)
        throw RegexMatcher::Error(std::string("Error: RegexRepeatMatcher.ParseRepetition(): ")
                                  + "Wrong number " + m_expr);

      m_repeatMin = min;
      m_repeatMax = max;

      return true;
    }
  }
  return false;
}

inline bool
RegexRepeatMatcher::match(const Name& name, const int& offset, const int& len)
{
  // _LOG_TRACE ("Enter RegexRepeatMatcher::match");

  m_matchResult.clear();

  if (0 == m_repeatMin)
    if (0 == len)
      return true;

  if (recursiveMatch(0, name, offset, len))
    {
      for (int i = offset; i < offset + len; i++)
        m_matchResult.push_back(name.get(i));
      return true;
    }
  else
    return false;
}

inline bool
RegexRepeatMatcher::recursiveMatch(int repeat, const Name& name, const int& offset, const int& len)
{
  int tried = len;
  shared_ptr<RegexMatcher> matcher = m_matcherList[0];

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

  while(tried >= 0)
    {
      if (matcher->match(name, offset, tried) and recursiveMatch(repeat + 1, name,
                                                                 offset + tried, len - tried))
        return true;
      tried --;
    }

  return false;
}


} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_REPEAT_MATCHER_HPP

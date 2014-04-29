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

#ifndef NDN_UTIL_REGEX_REGEX_PATTERN_LIST_MATCHER_HPP
#define NDN_UTIL_REGEX_REGEX_PATTERN_LIST_MATCHER_HPP

#include "../../common.hpp"

#include "regex-matcher.hpp"

namespace ndn {

class RegexBackrefManager;

class RegexPatternListMatcher : public RegexMatcher
{
public:
  RegexPatternListMatcher(const std::string& expr, shared_ptr<RegexBackrefManager> backrefManager);

  virtual
  ~RegexPatternListMatcher()
  {
  };

protected:
  virtual void
  compile();

private:
  bool
  extractPattern(size_t index, size_t* next);

  int
  extractSubPattern(const char left, const char right, size_t index);

  int
  extractRepetition(size_t index);

private:

};

} // namespace ndn

#include "regex-repeat-matcher.hpp"
#include "regex-backref-matcher.hpp"

namespace ndn {

inline
RegexPatternListMatcher::RegexPatternListMatcher(const std::string& expr,
                                                 shared_ptr<RegexBackrefManager> backrefManager)
  : RegexMatcher(expr, EXPR_PATTERN_LIST, backrefManager)
{
  compile();
}

inline void
RegexPatternListMatcher::compile()
{
  size_t len = m_expr.size();
  size_t index = 0;
  size_t subHead = index;

  while (index < len) {
    subHead = index;

    if (!extractPattern(subHead, &index))
      throw RegexMatcher::Error("Compile error");
  }
}

inline bool
RegexPatternListMatcher::extractPattern(size_t index, size_t* next)
{
  size_t start = index;
  size_t end = index;
  size_t indicator = index;

  switch (m_expr[index]) {
  case '(':
    index++;
    index = extractSubPattern('(', ')', index);
    indicator = index;
    end = extractRepetition(index);
    if (indicator == end) {
      shared_ptr<RegexMatcher> matcher =
        make_shared<RegexBackrefMatcher>(m_expr.substr(start, end - start), m_backrefManager);
      m_backrefManager->pushRef(matcher);
      dynamic_pointer_cast<RegexBackrefMatcher>(matcher)->lateCompile();

      m_matchers.push_back(matcher);
    }
    else
      m_matchers.push_back(make_shared<RegexRepeatMatcher>(m_expr.substr(start, end - start),
                                                           m_backrefManager, indicator - start));
    break;

  case '<':
    index++;
    index = extractSubPattern('<', '>', index);
    indicator = index;
    end = extractRepetition(index);
    m_matchers.push_back(make_shared<RegexRepeatMatcher>(m_expr.substr(start, end - start),
                                                         m_backrefManager, indicator - start));
    break;

  case '[':
    index++;
    index = extractSubPattern('[', ']', index);
    indicator = index;
    end = extractRepetition(index);
    m_matchers.push_back(make_shared<RegexRepeatMatcher>(m_expr.substr(start, end - start),
                                                         m_backrefManager, indicator - start));
    break;

  default:
    throw RegexMatcher::Error("Unexpected syntax");
  }

  *next = end;

  return true;
}

inline int
RegexPatternListMatcher::extractSubPattern(const char left, const char right, size_t index)
{
  size_t lcount = 1;
  size_t rcount = 0;

  while (lcount > rcount) {

    if (index >= m_expr.size())
      throw RegexMatcher::Error("Parenthesis mismatch");

    if (left == m_expr[index])
      lcount++;

    if (right == m_expr[index])
      rcount++;

    index++;
  }
  return index;
}

inline int
RegexPatternListMatcher::extractRepetition(size_t index)
{
  size_t exprSize = m_expr.size();

  if (index == exprSize)
    return index;

  if (('+' == m_expr[index] || '?' == m_expr[index] || '*' == m_expr[index])) {
    return ++index;
  }

  if ('{' == m_expr[index]) {
    while ('}' != m_expr[index]) {
      index++;
      if (index == exprSize)
        break;
    }
    if (index == exprSize)
      throw RegexMatcher::Error("Missing right brace bracket");
    else
      return ++index;
  }
  else {
    return index;
  }
}


} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_PATTERN_LIST_MATCHER_HPP

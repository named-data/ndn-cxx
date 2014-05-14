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

#ifndef NDN_UTIL_REGEX_COMPONENT_SET_MATCHER_HPP
#define NDN_UTIL_REGEX_COMPONENT_SET_MATCHER_HPP

#include "../../common.hpp"

#include "regex-matcher.hpp"
#include "regex-component-matcher.hpp"

#include <set>

namespace ndn {

class RegexComponentSetMatcher : public RegexMatcher
{
public:
  /**
   * @brief Create a RegexComponentSetMatcher matcher from expr
   * @param expr The standard regular expression to match a component
   * @param backrefManager Shared pointer to back-reference manager
   */
  RegexComponentSetMatcher(const std::string& expr, shared_ptr<RegexBackrefManager> backrefManager);

  virtual
  ~RegexComponentSetMatcher();

  virtual bool
  match(const Name& name, size_t offset, size_t len = 1);

protected:
  /**
   * @brief Compile the regular expression to generate the more matchers when necessary
   * @returns true if compiling succeeds
   */
  virtual void
  compile();

private:
  size_t
  extractComponent(size_t index);

  void
  compileSingleComponent();

  void
  compileMultipleComponents(size_t start, size_t lastIndex);

private:
  typedef std::set<shared_ptr<RegexComponentMatcher> > ComponentsSet;
  ComponentsSet m_components;
  bool m_isInclusion;
};


inline
RegexComponentSetMatcher::RegexComponentSetMatcher(const std::string& expr,
                                                   shared_ptr<RegexBackrefManager> backrefManager)
  : RegexMatcher(expr, EXPR_COMPONENT_SET, backrefManager)
  , m_isInclusion(true)
{
  compile();
}

inline
RegexComponentSetMatcher::~RegexComponentSetMatcher()
{
}

inline void
RegexComponentSetMatcher::compile()
{
  if (m_expr.size() < 2)
    throw RegexMatcher::Error("Regexp compile error (cannot parse " + m_expr + ")");

  switch (m_expr[0]) {
  case '<':
    return compileSingleComponent();
  case '[':
    {
      size_t lastIndex = m_expr.size() - 1;
      if (']' != m_expr[lastIndex])
        throw RegexMatcher::Error("Regexp compile error (no matching ']' in " + m_expr + ")");

      if ('^' == m_expr[1]) {
        m_isInclusion = false;
        compileMultipleComponents(2, lastIndex);
      }
      else
        compileMultipleComponents(1, lastIndex);
      break;
    }
  default:
    throw RegexMatcher::Error("Regexp compile error (cannot parse " + m_expr + ")");
  }
}

inline void
RegexComponentSetMatcher::compileSingleComponent()
{
  size_t end = extractComponent(1);

  if (m_expr.size() != end)
    {
      throw RegexMatcher::Error("Component expr error " + m_expr);
    }
  else
    {
      shared_ptr<RegexComponentMatcher> component =
        make_shared<RegexComponentMatcher>(m_expr.substr(1, end - 2), m_backrefManager);

      m_components.insert(component);
    }
}

inline void
RegexComponentSetMatcher::compileMultipleComponents(size_t start, size_t lastIndex)
{
  size_t index = start;
  size_t tempIndex = start;

  while (index < lastIndex) {
    if ('<' != m_expr[index])
      throw RegexMatcher::Error("Component expr error " + m_expr);

    tempIndex = index + 1;
    index = extractComponent(tempIndex);

    shared_ptr<RegexComponentMatcher> component =
      make_shared<RegexComponentMatcher>(m_expr.substr(tempIndex, index - tempIndex - 1),
                                         m_backrefManager);

    m_components.insert(component);
  }

  if (index != lastIndex)
    throw RegexMatcher::Error("Not sufficient expr to parse " + m_expr);
}

inline bool
RegexComponentSetMatcher::match(const Name& name, size_t offset, size_t len)
{
  bool isMatched = false;

  /* componentset only matches one component */
  if (len != 1)
    {
      return false;
    }

  for (ComponentsSet::iterator it = m_components.begin();
       it != m_components.end();
       ++it)
    {
      if ((*it)->match(name, offset, len))
        {
          isMatched = true;
          break;
        }
    }

  m_matchResult.clear();

  if (m_isInclusion ? isMatched : !isMatched)
    {
      m_matchResult.push_back(name.get(offset));
      return true;
    }
  else
    return false;
}

inline size_t
RegexComponentSetMatcher::extractComponent(size_t index)
{
  size_t lcount = 1;
  size_t rcount = 0;

  while (lcount > rcount) {
    switch (m_expr[index]) {
    case '<':
      lcount++;
      break;

    case '>':
      rcount++;
      break;

    case 0:
      throw RegexMatcher::Error("Error: square brackets mismatch");
      break;
    }
    index++;

  }

  return index;
}

} // namespace ndn

#endif // NDN_UTIL_REGEX_COMPONENT_SET_MATCHER_HPP

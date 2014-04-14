/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UTIL_REGEX_COMPONENT_SET_MATCHER_HPP
#define NDN_UTIL_REGEX_COMPONENT_SET_MATCHER_HPP

#include "../../common.hpp"

#include "regex-matcher.hpp"
#include "regex-component-matcher.hpp"

namespace ndn {

class RegexComponentSetMatcher : public RegexMatcher {

public:
  /**
   * @brief Create a RegexComponentSetMatcher matcher from expr
   * @param expr The standard regular expression to match a component
   * @param exact The flag to provide exact match
   * @param backRefNum The starting back reference number
   */
  RegexComponentSetMatcher(const std::string& expr, shared_ptr<RegexBackrefManager> backRefManager);

  virtual ~RegexComponentSetMatcher();

  virtual bool
  match(const Name& name, const int& offset, const int& len = 1);

protected:
  /**
   * @brief Compile the regular expression to generate the more matchers when necessary
   * @returns true if compiling succeeds
   */
  virtual void
  compile();

private:
  int
  extractComponent(int index);

  void
  compileSingleComponent();

  void
  compileMultipleComponents(const int start, const int lastIndex);

private:
  typedef std::set<shared_ptr<RegexComponentMatcher> > ComponentsSet;
  ComponentsSet m_components;
  bool m_include;
};


inline
RegexComponentSetMatcher::RegexComponentSetMatcher(const std::string& expr,
                                                   shared_ptr<RegexBackrefManager> backRefManager)
  : RegexMatcher(expr, EXPR_COMPONENT_SET, backRefManager),
    m_include(true)
{
  // _LOG_TRACE ("Enter RegexComponentSetMatcher Constructor");
  compile();
  // _LOG_TRACE ("Exit RegexComponentSetMatcher Constructor");
}

inline
RegexComponentSetMatcher::~RegexComponentSetMatcher()
{
  // ComponentsSet::iterator it = m_components.begin();

  // for(; it != m_components.end(); it++)
  //   delete *it;
}

inline void
RegexComponentSetMatcher::compile()
{
  switch (m_expr[0]){
  case '<':
    return compileSingleComponent();
  case '[':
    {
      int lastIndex = m_expr.size() - 1;
      if (']' != m_expr[lastIndex])
        throw RegexMatcher::Error(std::string("Error: RegexComponentSetMatcher.compile(): ")
                                  + " No matched ']' " + m_expr);

      if ('^' == m_expr[1]){
        m_include = false;
        compileMultipleComponents(2, lastIndex);
      }
      else
        compileMultipleComponents(1, lastIndex);
      break;
    }
  default:
    throw RegexMatcher::Error(std::string("Error: RegexComponentSetMatcher.compile(): ")
                              + "Parsing error in expr " + m_expr);
  }
}

inline void
RegexComponentSetMatcher::compileSingleComponent()
{
  size_t end = extractComponent(1);

  if (m_expr.size() != end)
    {
      throw RegexMatcher::Error(
        std::string("Error: RegexComponentSetMatcher.compileSingleComponent: ") + m_expr);
    }
  else
    {
      shared_ptr<RegexComponentMatcher> component =
        make_shared<RegexComponentMatcher>(m_expr.substr(1, end - 2), m_backrefManager);

      m_components.insert(component);
    }
}

inline void
RegexComponentSetMatcher::compileMultipleComponents(const int start, const int lastIndex)
{
  int index = start;
  int tmp_index = start;

  while(index < lastIndex){
    if ('<' != m_expr[index])
      throw RegexMatcher::Error(
        std::string("Error: RegexComponentSetMatcher.compileMultipleComponents: ") +
        "Component expr error " + m_expr);

    tmp_index = index + 1;
    index = extractComponent(tmp_index);

    shared_ptr<RegexComponentMatcher> component =
      make_shared<RegexComponentMatcher>(m_expr.substr(tmp_index, index - tmp_index - 1),
                                         m_backrefManager);

    m_components.insert(component);
  }

  if (index != lastIndex)
    throw RegexMatcher::Error(
      std::string("Error: RegexComponentSetMatcher.compileMultipleComponents: ") +
      "Not sufficient expr to parse " + m_expr);
}

inline bool
RegexComponentSetMatcher::match(const Name& name, const int& offset, const int& len)
{
  bool matched = false;

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
          matched = true;
          break;
        }
    }

  m_matchResult.clear();

  if (m_include ? matched : !matched)
    {
      m_matchResult.push_back(name.get(offset));
      return true;
    }
  else
    return false;
}

inline int
RegexComponentSetMatcher::extractComponent(int index)
{
  int lcount = 1;
  int rcount = 0;

  while(lcount > rcount){
    switch (m_expr[index]){
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

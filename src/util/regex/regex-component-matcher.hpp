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

#ifndef NDN_UTIL_REGEX_REGEX_COMPONENT_MATCHER_HPP
#define NDN_UTIL_REGEX_REGEX_COMPONENT_MATCHER_HPP

#include <boost/regex.hpp>

#include "regex-matcher.hpp"
#include "regex-pseudo-matcher.hpp"

namespace ndn {

class RegexComponentMatcher : public RegexMatcher
{
public:
  /**
   * @brief Create a RegexComponent matcher from expr
   * @param expr The standard regular expression to match a component
   * @param backrefManager The back reference manager
   * @param isExactMatch The flag to provide exact match
   */
  RegexComponentMatcher(const std::string& expr,
                        shared_ptr<RegexBackrefManager> backrefManager,
                        bool isExactMatch = true);

  virtual
  ~RegexComponentMatcher()
  {
  };

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
  bool m_isExactMatch;
  boost::regex m_componentRegex;
  std::vector<shared_ptr<RegexPseudoMatcher> > m_pseudoMatchers;

};


inline
RegexComponentMatcher::RegexComponentMatcher(const std::string& expr,
                                             shared_ptr<RegexBackrefManager> backrefManager,
                                             bool isExactMatch)
  : RegexMatcher(expr, EXPR_COMPONENT, backrefManager)
  , m_isExactMatch(isExactMatch)
{
  compile();
}

inline void
RegexComponentMatcher::compile()
{
  m_componentRegex = boost::regex(m_expr);

  m_pseudoMatchers.clear();
  m_pseudoMatchers.push_back(make_shared<RegexPseudoMatcher>());

  for (size_t i = 1; i < m_componentRegex.mark_count(); i++)
    {
      shared_ptr<RegexPseudoMatcher> pMatcher = make_shared<RegexPseudoMatcher>();
      m_pseudoMatchers.push_back(pMatcher);
      m_backrefManager->pushRef(static_pointer_cast<RegexMatcher>(pMatcher));
    }
}

inline bool
RegexComponentMatcher::match(const Name& name, size_t offset, size_t len)
{
  m_matchResult.clear();

  if (m_expr.empty())
    {
      m_matchResult.push_back(name.get(offset));
      return true;
    }

  if (m_isExactMatch)
    {
      boost::smatch subResult;
      std::string targetStr = name.get(offset).toEscapedString();
      if (boost::regex_match(targetStr, subResult, m_componentRegex))
        {
          for (size_t i = 1; i < m_componentRegex.mark_count(); i++)
            {
              m_pseudoMatchers[i]->resetMatchResult();
              m_pseudoMatchers[i]->setMatchResult(subResult[i]);
            }
          m_matchResult.push_back(name.get(offset));
          return true;
        }
    }
  else
    {
      throw RegexMatcher::Error("Non-exact component search is not supported yet!");
    }

  return false;
}


} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_COMPONENT_MATCHER_HPP

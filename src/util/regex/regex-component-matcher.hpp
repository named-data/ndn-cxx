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
   * @param backRefManager The back reference manager
   * @param exact The flag to provide exact match
   */
  RegexComponentMatcher(const std::string& expr,
                        ptr_lib::shared_ptr<RegexBackrefManager> backRefManager,
                        bool exact = true);

  virtual ~RegexComponentMatcher() {};

  virtual bool
  match(const Name & name, const int & offset, const int &len = 1);

protected:
  /**
   * @brief Compile the regular expression to generate the more matchers when necessary
   * @returns true if compiling succeeds
   */
  virtual void
  compile();

private:
  bool m_exact;
  boost::regex m_componentRegex;
  std::vector<ptr_lib::shared_ptr<RegexPseudoMatcher> > m_pseudoMatcher;

};


inline
RegexComponentMatcher::RegexComponentMatcher (const std::string& expr,
                                              shared_ptr<RegexBackrefManager> backRefManager,
                                              bool exact)
  : RegexMatcher (expr, EXPR_COMPONENT, backRefManager),
    m_exact(exact)
{
  // _LOG_TRACE ("Enter RegexComponentMatcher Constructor: ");
  compile();
  // _LOG_TRACE ("Exit RegexComponentMatcher Constructor: ");
}

inline void
RegexComponentMatcher::compile ()
{
  // _LOG_TRACE ("Enter RegexComponentMatcher::compile");

  m_componentRegex = boost::regex (m_expr);

  m_pseudoMatcher.clear();
  m_pseudoMatcher.push_back(make_shared<RegexPseudoMatcher>());

  for (size_t i = 1; i < m_componentRegex.mark_count(); i++)
    {
      shared_ptr<RegexPseudoMatcher> pMatcher = make_shared<RegexPseudoMatcher>();
      m_pseudoMatcher.push_back(pMatcher);
      m_backrefManager->pushRef(static_pointer_cast<RegexMatcher>(pMatcher));
    }


  // _LOG_TRACE ("Exit RegexComponentMatcher::compile");
}

inline bool
RegexComponentMatcher::match (const Name & name, const int & offset, const int & len)
{
  // _LOG_TRACE ("Enter RegexComponentMatcher::match ");

  m_matchResult.clear();

  if ("" == m_expr)
    {
      m_matchResult.push_back(name.get(offset));
      return true;
    }

  if (true == m_exact)
    {
      boost::smatch subResult;
      std::string targetStr = name.get(offset).toEscapedString();
      if (boost::regex_match(targetStr, subResult, m_componentRegex))
        {
          for (size_t i = 1; i < m_componentRegex.mark_count(); i++)
            {
              m_pseudoMatcher[i]->resetMatchResult();
              m_pseudoMatcher[i]->setMatchResult(subResult[i]);
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

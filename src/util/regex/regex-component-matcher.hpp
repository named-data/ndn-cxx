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

// Re: http://www.boost.org/users/history/version_1_56_0.html
//
//   Breaking change: corrected behavior of basic_regex<>::mark_count() to match existing
//   documentation, basic_regex<>::subexpression(n) changed to match, see
//   https://svn.boost.org/trac/boost/ticket/9227
static const size_t BOOST_REGEXP_MARK_COUNT_CORRECTION =
#if BOOST_VERSION < 105600
                    1;
#else
                    0;
#endif

inline void
RegexComponentMatcher::compile()
{
  m_componentRegex = boost::regex(m_expr);

  m_pseudoMatchers.clear();
  m_pseudoMatchers.push_back(make_shared<RegexPseudoMatcher>());

  for (size_t i = 1;
       i <= m_componentRegex.mark_count() - BOOST_REGEXP_MARK_COUNT_CORRECTION; i++)
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
      std::string targetStr = name.get(offset).toUri();
      if (boost::regex_match(targetStr, subResult, m_componentRegex))
        {
          for (size_t i = 1;
               i <= m_componentRegex.mark_count() - BOOST_REGEXP_MARK_COUNT_CORRECTION; i++)
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
      BOOST_THROW_EXCEPTION(RegexMatcher::Error("Non-exact component search is not supported "
                                                "yet"));
    }

  return false;
}


} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_COMPONENT_MATCHER_HPP

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

#include "regex-component-matcher.hpp"
#include "regex-pseudo-matcher.hpp"

namespace ndn {

// Re: http://www.boost.org/users/history/version_1_56_0.html
//
//   Breaking change: corrected behavior of basic_regex<>::mark_count() to match existing
//   documentation, basic_regex<>::subexpression(n) changed to match, see
//   https://svn.boost.org/trac/boost/ticket/9227
//
static constexpr size_t BOOST_REGEXP_MARK_COUNT_CORRECTION =
#if BOOST_VERSION < 105600
    1;
#else
    0;
#endif

RegexComponentMatcher::RegexComponentMatcher(const std::string& expr,
                                             shared_ptr<RegexBackrefManager> backrefManager,
                                             bool isExactMatch)
  : RegexMatcher(expr, EXPR_COMPONENT, std::move(backrefManager))
  , m_isExactMatch(isExactMatch)
{
  compile();
}

void
RegexComponentMatcher::compile()
{
  m_componentRegex = boost::regex(m_expr);

  m_pseudoMatchers.clear();
  m_pseudoMatchers.push_back(make_shared<RegexPseudoMatcher>());

  for (size_t i = 1; i <= m_componentRegex.mark_count() - BOOST_REGEXP_MARK_COUNT_CORRECTION; i++) {
    m_pseudoMatchers.push_back(make_shared<RegexPseudoMatcher>());
    m_backrefManager->pushRef(m_pseudoMatchers.back());
  }
}

bool
RegexComponentMatcher::match(const Name& name, size_t offset, size_t len)
{
  m_matchResult.clear();

  if (m_expr.empty()) {
    m_matchResult.push_back(name.get(offset));
    return true;
  }

  if (!m_isExactMatch)
    BOOST_THROW_EXCEPTION(Error("Non-exact component search is not supported yet"));

  boost::smatch subResult;
  std::string targetStr = name.get(offset).toUri();
  if (boost::regex_match(targetStr, subResult, m_componentRegex)) {
    for (size_t i = 1; i <= m_componentRegex.mark_count() - BOOST_REGEXP_MARK_COUNT_CORRECTION; i++) {
      m_pseudoMatchers[i]->resetMatchResult();
      m_pseudoMatchers[i]->setMatchResult(subResult[i]);
    }
    m_matchResult.push_back(name.get(offset));
    return true;
  }

  return false;
}

} // namespace ndn

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

#ifndef NDN_UTIL_REGEX_REGEX_BACKREF_MATCHER_HPP
#define NDN_UTIL_REGEX_REGEX_BACKREF_MATCHER_HPP

#include "../../common.hpp"

#include "regex-matcher.hpp"

namespace ndn {

class RegexBackrefMatcher : public RegexMatcher
{
public:
  RegexBackrefMatcher(const std::string& expr, shared_ptr<RegexBackrefManager> backrefManager);

  virtual
  ~RegexBackrefMatcher()
  {
  }

  void
  lateCompile()
  {
    compile();
  }

protected:
  virtual void
  compile();
};

} // namespace ndn

#include "regex-pattern-list-matcher.hpp"

namespace ndn {

inline
RegexBackrefMatcher::RegexBackrefMatcher(const std::string& expr,
                                         shared_ptr<RegexBackrefManager> backrefManager)
  : RegexMatcher(expr, EXPR_BACKREF, backrefManager)
{
  // compile();
}

inline void
RegexBackrefMatcher::compile()
{
  if (m_expr.size() < 2)
    BOOST_THROW_EXCEPTION(RegexMatcher::Error("Unrecognized format: " + m_expr));

  size_t lastIndex = m_expr.size() - 1;
  if ('(' == m_expr[0] && ')' == m_expr[lastIndex]) {
    // m_backRefManager->pushRef(this);

    shared_ptr<RegexMatcher> matcher(new RegexPatternListMatcher(m_expr.substr(1, lastIndex - 1),
                                                                 m_backrefManager));
    m_matchers.push_back(matcher);
  }
  else
    BOOST_THROW_EXCEPTION(RegexMatcher::Error("Unrecognized format: " + m_expr));
}


} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_BACKREF_MATCHER_HPP

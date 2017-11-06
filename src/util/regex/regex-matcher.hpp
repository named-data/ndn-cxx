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

#ifndef NDN_UTIL_REGEX_REGEX_MATCHER_HPP
#define NDN_UTIL_REGEX_REGEX_MATCHER_HPP

#include "regex-backref-manager.hpp"
#include "../../name.hpp"

namespace ndn {

class RegexMatcher
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  enum RegexExprType {
    EXPR_TOP,
    EXPR_PATTERN_LIST,
    EXPR_REPEAT_PATTERN,
    EXPR_BACKREF,
    EXPR_COMPONENT_SET,
    EXPR_COMPONENT,
    EXPR_PSEUDO
  };

  RegexMatcher(const std::string& expr, const RegexExprType& type,
               shared_ptr<RegexBackrefManager> backrefManager = nullptr);

  virtual
  ~RegexMatcher();

  virtual bool
  match(const Name& name, size_t offset, size_t len);

  /**
   * @brief get the matched name components
   * @returns the matched name components
   */
  const std::vector<name::Component>&
  getMatchResult() const
  {
    return m_matchResult;
  }

  const std::string&
  getExpr() const
  {
    return m_expr;
  }

protected:
  /**
   * @brief Compile the regular expression to generate the more matchers when necessary
   */
  virtual void
  compile() = 0;

private:
  bool
  recursiveMatch(size_t matcherNo, const Name& name, size_t offset, size_t len);

protected:
  const std::string m_expr;
  const RegexExprType m_type;
  shared_ptr<RegexBackrefManager> m_backrefManager;
  std::vector<shared_ptr<RegexMatcher>> m_matchers;
  std::vector<name::Component> m_matchResult;
};

std::ostream&
operator<<(std::ostream& os, const RegexMatcher& rm);

} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_MATCHER_HPP

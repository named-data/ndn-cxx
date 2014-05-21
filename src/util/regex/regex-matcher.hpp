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

#ifndef NDN_UTIL_REGEX_REGEX_MATCHER_H
#define NDN_UTIL_REGEX_REGEX_MATCHER_H

#include "../../common.hpp"
#include "../../name.hpp"

namespace ndn {

class RegexBackrefManager;

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

  RegexMatcher(const std::string& expr,
               const RegexExprType& type,
               shared_ptr<RegexBackrefManager> backrefManager = shared_ptr<RegexBackrefManager>());

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
   * @returns true if compiling succeeds
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
  std::vector<shared_ptr<RegexMatcher> > m_matchers;
  std::vector<name::Component> m_matchResult;
};

inline std::ostream&
operator<<(std::ostream& os, const RegexMatcher& regex)
{
  os << regex.getExpr();
  return os;
}

} // namespace ndn

#include "regex-backref-manager.hpp"

namespace ndn {

inline
RegexMatcher::RegexMatcher(const std::string& expr,
                           const RegexExprType& type,
                           shared_ptr<RegexBackrefManager> backrefManager)
  : m_expr(expr)
  , m_type(type)
  , m_backrefManager(backrefManager)
{
  if (!static_cast<bool>(m_backrefManager))
    m_backrefManager = make_shared<RegexBackrefManager>();
}

inline
RegexMatcher::~RegexMatcher()
{
}

inline bool
RegexMatcher::match(const Name& name, size_t offset, size_t len)
{
  bool result = false;

  m_matchResult.clear();

  if (recursiveMatch(0, name, offset, len))
    {
      for (size_t i = offset; i < offset + len ; i++)
        m_matchResult.push_back(name.get(i));
      result = true;
    }
  else
    {
      result = false;
    }

  return result;
}

inline bool
RegexMatcher::recursiveMatch(size_t matcherNo, const Name& name, size_t offset, size_t len)
{
  ssize_t tried = len;

  if (matcherNo >= m_matchers.size())
    return (len == 0);

  shared_ptr<RegexMatcher> matcher = m_matchers[matcherNo];

  while (tried >= 0)
    {
      if (matcher->match(name, offset, tried) &&
          recursiveMatch(matcherNo + 1, name, offset + tried, len - tried))
        return true;
      tried--;
    }

  return false;
}


} // namespace ndn


#endif // NDN_UTIL_REGEX_REGEX_MATCHER_H

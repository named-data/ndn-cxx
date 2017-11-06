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

#include "regex-top-matcher.hpp"

#include "regex-backref-manager.hpp"
#include "regex-pattern-list-matcher.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {

RegexTopMatcher::RegexTopMatcher(const std::string& expr, const std::string& expand)
  : RegexMatcher(expr, EXPR_TOP)
  , m_expand(expand)
  , m_isSecondaryUsed(false)
{
  m_primaryBackrefManager = make_shared<RegexBackrefManager>();
  m_secondaryBackrefManager = make_shared<RegexBackrefManager>();
  compile();
}

void
RegexTopMatcher::compile()
{
  std::string expr = m_expr;

  if ('$' != expr[expr.size() - 1])
    expr = expr + "<.*>*";
  else
    expr = expr.substr(0, expr.size() - 1);

  if ('^' != expr[0]) {
    m_secondaryMatcher = make_shared<RegexPatternListMatcher>("<.*>*" + expr,
                                                              m_secondaryBackrefManager);
  }
  else {
    expr = expr.substr(1, expr.size() - 1);
  }

  m_primaryMatcher = make_shared<RegexPatternListMatcher>(expr, m_primaryBackrefManager);
}

bool
RegexTopMatcher::match(const Name& name)
{
  m_isSecondaryUsed = false;

  m_matchResult.clear();

  if (m_primaryMatcher->match(name, 0, name.size())) {
    m_matchResult = m_primaryMatcher->getMatchResult();
    return true;
  }
  else {
    if (m_secondaryMatcher != nullptr && m_secondaryMatcher->match(name, 0, name.size())) {
      m_matchResult = m_secondaryMatcher->getMatchResult();
      m_isSecondaryUsed = true;
      return true;
    }
    return false;
  }
}

bool
RegexTopMatcher::match(const Name& name, size_t, size_t)
{
  return match(name);
}

Name
RegexTopMatcher::expand(const std::string& expandStr)
{
  auto backrefManager = m_isSecondaryUsed ? m_secondaryBackrefManager : m_primaryBackrefManager;
  size_t backrefNo = backrefManager->size();

  std::string expand;
  if (!expandStr.empty())
    expand = expandStr;
  else
    expand = m_expand;

  Name result;
  size_t offset = 0;
  while (offset < expand.size()) {
    std::string item = getItemFromExpand(expand, offset);
    if (item[0] == '<') {
      result.append(item.substr(1, item.size() - 2));
    }
    if (item[0] == '\\') {
      size_t index = boost::lexical_cast<size_t>(item.substr(1, item.size() - 1));
      if (index == 0) {
        for (const auto& i : m_matchResult)
          result.append(i);
      }
      else if (index <= backrefNo) {
        for (const auto& i : backrefManager->getBackref(index - 1)->getMatchResult())
          result.append(i);
      }
      else
        BOOST_THROW_EXCEPTION(Error("Exceed the range of back reference"));
    }
  }

  return result;
}

std::string
RegexTopMatcher::getItemFromExpand(const std::string& expand, size_t& offset)
{
  size_t begin = offset;

  if (expand[offset] == '\\') {
    offset++;
    if (offset >= expand.size())
      BOOST_THROW_EXCEPTION(Error("Wrong format of expand string"));

    while (expand[offset] <= '9' and expand[offset] >= '0') {
      offset++;
      if (offset > expand.size())
        BOOST_THROW_EXCEPTION(Error("Wrong format of expand string"));
    }
    if (offset > begin + 1)
      return expand.substr(begin, offset - begin);
    else
      BOOST_THROW_EXCEPTION(Error("Wrong format of expand string"));
  }
  else if (expand[offset] == '<') {
    offset++;
    if (offset >= expand.size())
      BOOST_THROW_EXCEPTION(Error("Wrong format of expand string"));

    size_t left = 1;
    size_t right = 0;
    while (right < left) {
      if (expand[offset] == '<')
        left++;
      if (expand[offset] == '>')
        right++;
      offset++;
      if (offset >= expand.size())
        BOOST_THROW_EXCEPTION(Error("Wrong format of expand string"));
    }
    return expand.substr(begin, offset - begin);
  }
  else
    BOOST_THROW_EXCEPTION(Error("Wrong format of expand string"));
}

shared_ptr<RegexTopMatcher>
RegexTopMatcher::fromName(const Name& name, bool hasAnchor)
{
  std::string regexStr("^");

  for (auto it = name.begin(); it != name.end(); it++) {
    regexStr.append("<");
    regexStr.append(convertSpecialChar(it->toUri()));
    regexStr.append(">");
  }

  if (hasAnchor)
    regexStr.append("$");

  return make_shared<RegexTopMatcher>(regexStr);
}

std::string
RegexTopMatcher::convertSpecialChar(const std::string& str)
{
  std::string newStr;

  for (size_t i = 0; i < str.size(); i++) {
    char c = str[i];
    switch (c) {
      case '.':
      case '[':
      case '{':
      case '}':
      case '(':
      case ')':
      case '\\':
      case '*':
      case '+':
      case '?':
      case '|':
      case '^':
      case '$':
        newStr.push_back('\\');
        NDN_CXX_FALLTHROUGH;
      default:
        newStr.push_back(c);
        break;
    }
  }

  return newStr;
}

} // namespace ndn

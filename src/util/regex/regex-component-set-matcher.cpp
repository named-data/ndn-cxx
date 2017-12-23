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

#include "regex-component-set-matcher.hpp"
#include "regex-component-matcher.hpp"

namespace ndn {

RegexComponentSetMatcher::RegexComponentSetMatcher(const std::string& expr,
                                                   shared_ptr<RegexBackrefManager> backrefManager)
  : RegexMatcher(expr, EXPR_COMPONENT_SET, std::move(backrefManager))
  , m_isInclusion(true)
{
  compile();
}

void
RegexComponentSetMatcher::compile()
{
  if (m_expr.size() < 2)
    BOOST_THROW_EXCEPTION(Error("Regexp compile error (cannot parse " + m_expr + ")"));

  switch (m_expr[0]) {
    case '<':
      return compileSingleComponent();
    case '[': {
      size_t lastIndex = m_expr.size() - 1;
      if (']' != m_expr[lastIndex])
        BOOST_THROW_EXCEPTION(Error("Regexp compile error (no matching ']' in " + m_expr + ")"));

      if ('^' == m_expr[1]) {
        m_isInclusion = false;
        compileMultipleComponents(2, lastIndex);
      }
      else
        compileMultipleComponents(1, lastIndex);
      break;
    }
    default:
      BOOST_THROW_EXCEPTION(Error("Regexp compile error (cannot parse " + m_expr + ")"));
  }
}

void
RegexComponentSetMatcher::compileSingleComponent()
{
  size_t end = extractComponent(1);
  if (m_expr.size() != end)
    BOOST_THROW_EXCEPTION(Error("Component expr error " + m_expr));

  m_components.push_back(make_shared<RegexComponentMatcher>(m_expr.substr(1, end - 2), m_backrefManager));
}

void
RegexComponentSetMatcher::compileMultipleComponents(size_t start, size_t lastIndex)
{
  size_t index = start;
  size_t tempIndex = start;

  while (index < lastIndex) {
    if ('<' != m_expr[index])
      BOOST_THROW_EXCEPTION(Error("Component expr error " + m_expr));

    tempIndex = index + 1;
    index = extractComponent(tempIndex);
    m_components.push_back(make_shared<RegexComponentMatcher>(m_expr.substr(tempIndex, index - tempIndex - 1),
                                                              m_backrefManager));
  }

  if (index != lastIndex)
    BOOST_THROW_EXCEPTION(Error("Not sufficient expr to parse " + m_expr));
}

bool
RegexComponentSetMatcher::match(const Name& name, size_t offset, size_t len)
{
  // componentset only matches one component
  if (len != 1)
    return false;

  bool isMatched = false;
  for (const auto& comp : m_components) {
    if (comp->match(name, offset, len)) {
      isMatched = true;
      break;
    }
  }

  m_matchResult.clear();

  if (m_isInclusion ? isMatched : !isMatched) {
    m_matchResult.push_back(name.get(offset));
    return true;
  }
  else
    return false;
}

size_t
RegexComponentSetMatcher::extractComponent(size_t index) const
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
        BOOST_THROW_EXCEPTION(Error("Square brackets mismatch"));
        break;
    }
    index++;
  }

  return index;
}

} // namespace ndn

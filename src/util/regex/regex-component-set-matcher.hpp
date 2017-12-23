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

#ifndef NDN_UTIL_REGEX_REGEX_COMPONENT_SET_MATCHER_HPP
#define NDN_UTIL_REGEX_REGEX_COMPONENT_SET_MATCHER_HPP

#include "regex-matcher.hpp"

#include <vector>

namespace ndn {

class RegexComponentMatcher;

class RegexComponentSetMatcher : public RegexMatcher
{
public:
  /**
   * @brief Create a RegexComponentSetMatcher matcher from expr
   * @param expr The standard regular expression to match a component
   * @param backrefManager Shared pointer to back-reference manager
   */
  RegexComponentSetMatcher(const std::string& expr, shared_ptr<RegexBackrefManager> backrefManager);

  bool
  match(const Name& name, size_t offset, size_t len = 1) override;

protected:
  /**
   * @brief Compile the regular expression to generate the more matchers when necessary
   */
  void
  compile() override;

private:
  void
  compileSingleComponent();

  void
  compileMultipleComponents(size_t start, size_t lastIndex);

  size_t
  extractComponent(size_t index) const;

private:
  std::vector<shared_ptr<RegexComponentMatcher>> m_components;
  bool m_isInclusion;
};

} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_COMPONENT_SET_MATCHER_HPP

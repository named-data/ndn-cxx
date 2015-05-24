/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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
  ~RegexComponentMatcher() NDN_CXX_DECL_FINAL;

  virtual bool
  match(const Name& name, size_t offset, size_t len = 1) NDN_CXX_DECL_FINAL;

protected:
  /**
   * @brief Compile the regular expression to generate the more matchers when necessary
   * @returns true if compiling succeeds
   */
  virtual void
  compile() NDN_CXX_DECL_FINAL;

private:
  bool m_isExactMatch;
  boost::regex m_componentRegex;
  std::vector<shared_ptr<RegexPseudoMatcher> > m_pseudoMatchers;

};

} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_COMPONENT_MATCHER_HPP

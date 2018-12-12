/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#ifndef NDN_UTIL_REGEX_REGEX_BACKREF_MANAGER_HPP
#define NDN_UTIL_REGEX_REGEX_BACKREF_MANAGER_HPP

#include "ndn-cxx/detail/common.hpp"

#include <vector>

namespace ndn {

class RegexMatcher;

class RegexBackrefManager
{
public:
  size_t
  pushRef(const shared_ptr<RegexMatcher>& matcher);

  void
  popRef()
  {
    m_backrefs.pop_back();
  }

  size_t
  size() const
  {
    return m_backrefs.size();
  }

  shared_ptr<RegexMatcher>
  getBackref(size_t i) const;

private:
  std::vector<weak_ptr<RegexMatcher>> m_backrefs;
};

} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_BACKREF_MANAGER_HPP

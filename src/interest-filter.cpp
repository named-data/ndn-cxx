/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "interest-filter.hpp"
#include "util/regex/regex-pattern-list-matcher.hpp"

namespace ndn {

InterestFilter::InterestFilter(const Name& prefix)
  : m_prefix(prefix)
{
}

InterestFilter::InterestFilter(const char* prefixUri)
  : m_prefix(prefixUri)
{
}

InterestFilter::InterestFilter(const std::string& prefixUri)
  : m_prefix(prefixUri)
{
}

InterestFilter::InterestFilter(const Name& prefix, const std::string& regexFilter)
  : m_prefix(prefix)
  , m_regexFilter(make_shared<RegexPatternListMatcher>(regexFilter, nullptr))
{
}

InterestFilter::operator const Name&() const
{
  if (hasRegexFilter()) {
    BOOST_THROW_EXCEPTION(Error("Please update InterestCallback to accept const InterestFilter&"
                                " (non-trivial InterestFilter is being used)"));
  }
  return m_prefix;
}

bool
InterestFilter::doesMatch(const Name& name) const
{
  return m_prefix.isPrefixOf(name) &&
         (!hasRegexFilter() ||
          m_regexFilter->match(name, m_prefix.size(), name.size() - m_prefix.size()));
}

std::ostream&
operator<<(std::ostream& os, const InterestFilter& filter)
{
  os << filter.getPrefix();
  if (filter.hasRegexFilter()) {
    os << "?regex=" << filter.getRegexFilter();
  }
  return os;
}

} // namespace ndn

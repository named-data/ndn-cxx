/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#include "interest-filter.hpp"

#include "util/regex/regex-pattern-list-matcher.hpp"

namespace ndn {

InterestFilter::InterestFilter(const Name& prefix, const std::string& regexFilter)
  : m_prefix(prefix)
  , m_regexFilter(ndn::make_shared<RegexPatternListMatcher>(regexFilter,
                                                            shared_ptr<RegexBackrefManager>()))
{
}

bool
InterestFilter::doesMatch(const Name& name) const
{
  if (name.size() < m_prefix.size())
    return false;

  if (hasRegexFilter()) {
    // perform prefix match and regular expression match for the remaining components
    bool isMatch = m_prefix.isPrefixOf(name);

    if (!isMatch)
      return false;

    return m_regexFilter->match(name, m_prefix.size(), name.size() - m_prefix.size());
  }
  else {
    // perform just prefix match

    return m_prefix.isPrefixOf(name);
  }
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

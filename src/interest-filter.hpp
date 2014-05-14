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

#ifndef NDN_INTEREST_FILTER_HPP
#define NDN_INTEREST_FILTER_HPP

#include "name.hpp"

namespace ndn {

class RegexPatternListMatcher;

class InterestFilter
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

  /**
   * @brief Create an Interest filter to match Interests by prefix
   *
   * This filter will match all Interests, whose name start with the given prefix
   *
   * Any Name can be implicitly converted to the InterestFilter.
   */
  InterestFilter(const Name& prefix);

  /**
   * @brief Create an Interest filter to match Interests by prefix URI
   *
   * This filter will match all Interests, whose name start with the given prefix
   *
   * Any const char* can be implicitly converted to the InterestFilter.
   */
  InterestFilter(const char* prefixUri);

  /**
   * @brief Create an Interest filter to match Interests by prefix URI
   *
   * This filter will match all Interests, whose name start with the given prefix
   *
   * Any std::string can be implicitly converted to the InterestFilter.
   */
  InterestFilter(const std::string& prefixUri);

  /**
   * @brief Create an Interest filter to match Interest by prefix and regular expression
   *
   * This filter will match all Interests, whose name start with the given prefix and
   * other components of the Interest name match the given regular expression.
   * For example, the following InterestFilter:
   *
   *    InterestFilter("/hello", "<world><>+")
   *
   * will match all Interests, whose name has prefix `/hello`, which is followed by
   * component `world` and has at least one more component after it.  Examples:
   *
   *    /hello/world/!
   *    /hello/world/x/y/z
   *
   * Note that regular expression will need to match all components (e.g., there is
   * an implicit heading `^` and trailing `$` symbols in the regular expression).
   */
  InterestFilter(const Name& prefix, const std::string& regexFilter);

  /**
   * @brief Implicit conversion to Name (to provide backwards compatibility for onInterest callback)
   */
  operator const Name&() const
  {
    if (static_cast<bool>(m_regexFilter)) {
      throw Error("Please update OnInterest callback to accept const InterestFilter& "
                  "(non-trivial Interest filter is being used)");
    }
    return m_prefix;
  }

  /**
   * @brief Check if specified name matches the filter
   */
  bool
  doesMatch(const Name& name) const;

  const Name&
  getPrefix() const
  {
    return m_prefix;
  }

  bool
  hasRegexFilter() const
  {
    return static_cast<bool>(m_regexFilter);
  }

  const RegexPatternListMatcher&
  getRegexFilter() const
  {
    return *m_regexFilter;
  }

private:
  Name m_prefix;
  shared_ptr<RegexPatternListMatcher> m_regexFilter;
};

std::ostream&
operator<<(std::ostream& os, const InterestFilter& filter);


inline
InterestFilter::InterestFilter(const Name& prefix)
  : m_prefix(prefix)
{
}

inline
InterestFilter::InterestFilter(const char* prefixUri)
  : m_prefix(prefixUri)
{
}

inline
InterestFilter::InterestFilter(const std::string& prefixUri)
  : m_prefix(prefixUri)
{
}

} // namespace ndn

#endif // NDN_INTEREST_FILTER_HPP

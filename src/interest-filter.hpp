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
 */

#ifndef NDN_INTEREST_FILTER_HPP
#define NDN_INTEREST_FILTER_HPP

#include "name.hpp"

namespace ndn {

class RegexPatternListMatcher;

/**
 * @brief declares the set of Interests a producer can serve,
 *        which starts with a name prefix, plus an optional regular expression
 */
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
   * @brief Construct an InterestFilter to match Interests by prefix
   *
   * This filter matches Interests whose name start with the given prefix.
   *
   * @note InterestFilter is implicitly convertible from Name.
   */
  InterestFilter(const Name& prefix);

  /**
   * @brief Construct an InterestFilter to match Interests by prefix
   *
   * This filter matches Interests whose name start with the given prefix.
   *
   * @param prefixUri name prefix, interpreted as ndn URI
   * @note InterestFilter is implicitly convertible from null-terminated byte string.
   */
  InterestFilter(const char* prefixUri);

  /**
   * @brief Construct an InterestFilter to match Interests by prefix
   *
   * This filter matches Interests whose name start with the given prefix.
   *
   * @param prefixUri name prefix, interpreted as ndn URI
   * @note InterestFilter is implicitly convertible from std::string.
   */
  InterestFilter(const std::string& prefixUri);

  /**
   * @brief Construct an InterestFilter to match Interests by prefix and regular expression
   *
   * This filter matches Interests whose name start with the given prefix and
   * the remaining components match the given regular expression.
   * For example, the following InterestFilter:
   *
   *    InterestFilter("/hello", "<world><>+")
   *
   * matches Interests whose name has prefix `/hello` followed by component `world`
   * and has at least one more component after it, such as:
   *
   *    /hello/world/%21
   *    /hello/world/x/y/z
   *
   * Note that regular expression will need to match all components (e.g., there are
   * implicit heading `^` and trailing `$` symbols in the regular expression).
   */
  InterestFilter(const Name& prefix, const std::string& regexFilter);

  /**
   * @brief Implicit conversion to Name
   * @note This allows InterestCallback to be declared with `Name` rather than `InterestFilter`,
   *       but this does not work if InterestFilter has regular expression.
   */
  operator const Name&() const;

  /**
   * @brief Check if specified Interest name matches the filter
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
    return m_regexFilter != nullptr;
  }

  const RegexPatternListMatcher&
  getRegexFilter() const
  {
    return *m_regexFilter;
  }

  /** \brief Get whether Interest loopback is allowed
   */
  bool
  allowsLoopback() const
  {
    return m_allowsLoopback;
  }

  /** \brief Set whether Interest loopback is allowed
   *  \param wantLoopback if true, this InterestFilter may receive Interests that are expressed
   *                      locally on the same \p ndn::Face ; if false, this InterestFilter can only
   *                      receive Interests received from the forwarder. The default is true.
   */
  InterestFilter&
  allowLoopback(bool wantLoopback)
  {
    m_allowsLoopback = wantLoopback;
    return *this;
  }

private:
  Name m_prefix;
  shared_ptr<RegexPatternListMatcher> m_regexFilter;
  bool m_allowsLoopback = true;
};

std::ostream&
operator<<(std::ostream& os, const InterestFilter& filter);

} // namespace ndn

#endif // NDN_INTEREST_FILTER_HPP

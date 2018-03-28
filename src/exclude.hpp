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
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#ifndef NDN_EXCLUDE_HPP
#define NDN_EXCLUDE_HPP

#include "name-component.hpp"
#include "encoding/encoding-buffer.hpp"

#include <map>

namespace ndn {

/**
 * @brief Represents Exclude selector in NDN Interest
 *
 * NDN Packet Format v0.3 defines name component types other than GenericNameComponent and
 * ImplicitSha256DigestComponent, and removes Exclude selector. This implementation follows v0.2
 * semantics and can only store GenericNameComponent and ImplicitSha256DigestComponent.
 * The behavior of \c isExcluded on a name component of other types is unspecified.
 */
class Exclude
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  /**
   * @brief Constructs an empty Exclude
   */
  Exclude();

  /**
   * @brief Create from wire encoding
   */
  explicit
  Exclude(const Block& wire);

  /**
   * @brief Fast encoding or block size estimation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /**
   * @brief Encode to a wire format
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode from the wire format
   */
  void
  wireDecode(const Block& wire);

  /**
   * @brief Get escaped string representation (e.g., for use in URI) of the exclude filter
   */
  std::string
  toUri() const;

public: // high-level API
  /**
   * @brief Check if name component is excluded
   * @param comp Name component to check against exclude filter
   */
  bool
  isExcluded(const name::Component& comp) const;

  /**
   * @brief Exclude specific name component
   * @param comp component to exclude
   * @returns *this to allow chaining
   */
  Exclude&
  excludeOne(const name::Component& comp);

  /**
   * @brief Exclude components in range [from, to]
   * @param from first element of the range
   * @param to last element of the range
   * @throw Error \p from equals or comes after \p to in canonical ordering
   * @returns *this to allow chaining
   */
  Exclude&
  excludeRange(const name::Component& from, const name::Component& to);

  /**
   * @brief Exclude all components in range (-Inf, to]
   * @param to last element of the range
   * @returns *this to allow chaining
   */
  Exclude&
  excludeBefore(const name::Component& to);

  /**
   * @brief Exclude all components in range [from, +Inf)
   * @param from the first element of the range
   * @returns *this to allow chaining
   */
  Exclude&
  excludeAfter(const name::Component& from);

public: // EqualityComparable concept
  bool
  operator==(const Exclude& other) const;

  bool
  operator!=(const Exclude& other) const;

public: // internal storage
  /**
   * @brief either a name::Component or "negative infinity"
   */
  class ExcludeComponent
  {
  public:
    /**
     * @brief implicitly construct a regular infinity ExcludeComponent
     * @param component a name component which is excluded
     */
    ExcludeComponent(const name::Component& component);

    /**
     * @brief construct a negative infinity ExcludeComponent
     * @param isNegInf must be true
     */
    explicit
    ExcludeComponent(bool isNegInf);

  public:
    bool isNegInf;
    name::Component component;
  };

  /**
   * @brief a map of exclude entries
   *
   * Each key, except "negative infinity", is a name component that is excluded.
   * The mapped boolean indicates whether the range between a key and the next greater key
   * is also excluded. If true, the wire encoding shall have an ANY element.
   *
   * The map is ordered in descending order to simplify \p isExcluded.
   */
  typedef std::map<ExcludeComponent, bool, std::greater<ExcludeComponent>> ExcludeMap;
  typedef ExcludeMap::value_type Entry;

public: // enumeration API
  /**
   * @brief represent an excluded component or range
   */
  class Range
  {
  public:
    Range();

    Range(bool fromInfinity, const name::Component& from, bool toInfinity, const name::Component& to);

    /**
     * @retval true A single component is excluded
     * @retval false A range of more than one components are excluded
     */
    bool
    isSingular() const;

    bool
    operator==(const Exclude::Range& other) const;

    bool
    operator!=(const Exclude::Range& other) const;

  public:
    /**
     * @brief from negative infinity?
     */
    bool fromInfinity;

    /**
     * @brief from component (inclusive)
     * @pre valid only if !fromInfinity
     */
    name::Component from;

    /**
     * @brief to positive infinity?
     */
    bool toInfinity;

    /**
     * @brief to component (inclusive)
     * @pre valid only if !toInfinity
     */
    name::Component to;
  };

  class const_iterator : public std::iterator<std::forward_iterator_tag, const Range>
  {
  public:
    const_iterator() = default;

    const_iterator(ExcludeMap::const_reverse_iterator it, ExcludeMap::const_reverse_iterator rend);

    const Range&
    operator*() const;

    const Range*
    operator->() const;

    const_iterator&
    operator++();

    const_iterator
    operator++(int);

    bool
    operator==(const const_iterator& other) const;

    bool
    operator!=(const const_iterator& other) const;

  private:
    void
    update();

  private:
    ExcludeMap::const_reverse_iterator m_it;
    ExcludeMap::const_reverse_iterator m_rend;
    Range m_range;
    friend class Exclude;
  };

  const_iterator
  begin() const;

  const_iterator
  end() const;

  bool
  empty() const;

  size_t
  size() const;

  /// \todo const_iterator erase(const_iterator i);

  void
  clear();

private:
  /**
   * @brief directly append exclude element
   * @tparam T either name::Component or bool
   *
   * This method is used during conversion from wire format of exclude filter
   */
  template<typename T>
  void
  appendEntry(const T& component, bool hasAny);

  Exclude&
  excludeRange(const ExcludeComponent& from, const name::Component& to);

private:
  ExcludeMap m_entries;
  mutable Block m_wire;

  friend std::ostream&
  operator<<(std::ostream& os, const Exclude& name);
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(Exclude);

bool
operator==(const Exclude::ExcludeComponent& a, const Exclude::ExcludeComponent& b);

bool
operator>(const Exclude::ExcludeComponent& a, const Exclude::ExcludeComponent& b);

std::ostream&
operator<<(std::ostream& os, const Exclude::Range& range);

std::ostream&
operator<<(std::ostream& os, const Exclude& name);

inline Exclude::const_iterator
Exclude::begin() const
{
  return const_iterator(m_entries.rbegin(), m_entries.rend());
}

inline Exclude::const_iterator
Exclude::end() const
{
  return const_iterator(m_entries.rend(), m_entries.rend());
}

inline bool
Exclude::empty() const
{
  return m_entries.empty();
}

inline bool
Exclude::operator!=(const Exclude& other) const
{
  return !(*this == other);
}

inline bool
Exclude::Range::isSingular() const
{
  return !this->fromInfinity && !this->toInfinity && this->from == this->to;
}

inline bool
Exclude::Range::operator!=(const Exclude::Range& other) const
{
  return !this->operator==(other);
}

inline const Exclude::Range&
Exclude::const_iterator::operator*() const
{
  BOOST_ASSERT(m_it != m_rend);
  return m_range;
}

inline const Exclude::Range*
Exclude::const_iterator::operator->() const
{
  BOOST_ASSERT(m_it != m_rend);
  return &m_range;
}

inline bool
Exclude::const_iterator::operator==(const const_iterator& other) const
{
  return m_it == other.m_it;
}

inline bool
Exclude::const_iterator::operator!=(const const_iterator& other) const
{
  return !this->operator==(other);
}

} // namespace ndn

#endif // NDN_EXCLUDE_HPP

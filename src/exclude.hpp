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
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#ifndef NDN_EXCLUDE_H
#define NDN_EXCLUDE_H

#include "name-component.hpp"
#include "encoding/encoding-buffer.hpp"

#include <sstream>
#include <map>

namespace ndn {

/**
 * @brief Represents Exclude selector in NDN Interest
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

  /**
   * @brief Check if exclude filter is empty
   */
  bool
  empty() const;

  /**
   * @brief Clear the exclude filter
   */
  void
  clear();

public: // EqualityComparable concept
  bool
  operator==(const Exclude& other) const;

  bool
  operator!=(const Exclude& other) const;

public: // low-level exclude entry API
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
  typedef std::map<ExcludeComponent, bool, std::greater<ExcludeComponent>> ExcludeType;
  typedef ExcludeType::value_type Entry;
  typedef ExcludeType::iterator iterator;
  typedef ExcludeType::const_iterator const_iterator;
  typedef ExcludeType::reverse_iterator reverse_iterator;
  typedef ExcludeType::const_reverse_iterator const_reverse_iterator;

  /**
   * @brief Get number of exclude terms
   */
  size_t
  size() const;

  /**
   * @brief Get begin iterator of the exclude terms
   */
  const_iterator
  begin() const;

  /**
   * @brief Get end iterator of the exclude terms
   */
  const_iterator
  end() const;

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
  ExcludeType m_entries;

  mutable Block m_wire;
};

std::ostream&
operator<<(std::ostream& os, const Exclude& name);

bool
operator>(const Exclude::ExcludeComponent& a, const Exclude::ExcludeComponent& b);

inline bool
Exclude::empty() const
{
  return m_entries.empty();
}

inline size_t
Exclude::size() const
{
  return m_entries.size();
}

inline Exclude::const_iterator
Exclude::begin() const
{
  return m_entries.begin();
}

inline Exclude::const_iterator
Exclude::end() const
{
  return m_entries.end();
}

inline bool
Exclude::operator!=(const Exclude& other) const
{
  return !(*this == other);
}

} // namespace ndn

#endif // NDN_EXCLUDE_H

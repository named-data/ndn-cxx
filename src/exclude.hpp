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
 * @brief Class to represent Exclude component in NDN interests
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
   * @brief Default constructor an empty exclude
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
   * @brief Exclude components from range [from, to]
   * @param from first element of the range
   * @param to last element of the range
   * @returns *this to allow chaining
   */
  Exclude&
  excludeRange(const name::Component& from, const name::Component& to);

  /**
   * @brief Exclude all components from range [/, to]
   * @param to last element of the range
   * @returns *this to allow chaining
   */
  Exclude&
  excludeBefore(const name::Component& to);

  /**
   * @brief Exclude all components from range [from, +Inf]
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

public: // low-level exclude element API
  typedef std::map< name::Component, bool /*any*/, std::greater<name::Component> > exclude_type;

  typedef exclude_type::iterator iterator;
  typedef exclude_type::const_iterator const_iterator;
  typedef exclude_type::reverse_iterator reverse_iterator;
  typedef exclude_type::const_reverse_iterator const_reverse_iterator;

  /**
   * @brief Method to directly append exclude element
   * @param name excluded name component
   * @param any  flag indicating if there is a postfix ANY component after the name
   *
   * This method is used during conversion from wire format of exclude filter
   *
   * If there is an error with ranges (e.g., order of components is wrong) an exception is thrown
   */
  void
  appendExclude(const name::Component& name, bool any);

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

  /**
   * @brief Get begin iterator of the exclude terms
   */
  const_reverse_iterator
  rbegin() const;

  /**
   * @brief Get end iterator of the exclude terms
   */
  const_reverse_iterator
  rend() const;

private:
  Exclude&
  excludeRange(iterator fromLowerBound, iterator toLowerBound);

private:
  exclude_type m_exclude;

  mutable Block m_wire;
};

std::ostream&
operator<<(std::ostream& os, const Exclude& name);

inline Exclude&
Exclude::excludeBefore(const name::Component& to)
{
  return excludeRange(name::Component(), to);
}

inline void
Exclude::appendExclude(const name::Component& name, bool any)
{
  m_exclude[name] = any;
}

inline bool
Exclude::empty() const
{
  return m_exclude.empty();
}

inline void
Exclude::clear()
{
  m_exclude.clear();
  m_wire.reset();
}

inline size_t
Exclude::size() const
{
  return m_exclude.size();
}

inline Exclude::const_iterator
Exclude::begin() const
{
  return m_exclude.begin();
}

inline Exclude::const_iterator
Exclude::end() const
{
  return m_exclude.end();
}

inline Exclude::const_reverse_iterator
Exclude::rbegin() const
{
  return m_exclude.rbegin();
}

inline Exclude::const_reverse_iterator
Exclude::rend() const
{
  return m_exclude.rend();
}

inline bool
Exclude::operator!=(const Exclude& other) const
{
  return !(*this == other);
}

} // ndn

#endif // NDN_EXCLUDE_H

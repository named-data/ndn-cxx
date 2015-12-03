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

#ifndef NDN_TAG_HPP
#define NDN_TAG_HPP

namespace ndn {

/**
 * @brief Base class for packet tags that can hold any arbitrary information
 */
class Tag
{
public:
  virtual
  ~Tag();

  /**
   * @fn static constexpr int getTypeId()
   * @return an integer that uniquely identifies this Tag type
   * @sa http://redmine.named-data.net/projects/ndn-cxx/wiki/PacketTagTypes
   */
#ifdef DOXYGEN
  static constexpr int
  getTypeId()
  {
    return <type-identifier>;
  }
#endif // DOXYGEN
};

inline
Tag::~Tag() = default;

/** @brief provides a tag type for simple types
 *  @tparam T the value type
 *  @tparam TypeId the TypeId
 */
template<typename T, int TypeId>
class SimpleTag : public Tag
{
public:
  static constexpr int
  getTypeId()
  {
    return TypeId;
  }

  /** \brief explicitly convertible from T
   */
  explicit
  SimpleTag(const T& value)
    : m_value(value)
  {
  }

  /** \brief implicitly convertible to T
   *  \return a copy of the enclosed value
   */
  operator T() const
  {
    return m_value;
  }

  /** \return the enclosed value
   */
  const T&
  get() const
  {
    return m_value;
  }

private:
  T m_value;
};

} // namespace ndn

#endif // NDN_TAG_HPP

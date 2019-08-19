/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#ifndef NDN_KEY_LOCATOR_HPP
#define NDN_KEY_LOCATOR_HPP

#include "ndn-cxx/name.hpp"

namespace ndn {

class KeyLocator
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  enum
#ifndef DOXYGEN
  [[deprecated]] // apparently doxygen can't handle this attribute on enums
#endif
  Type {
    /// KeyLocator is empty
    KeyLocator_None = tlv::Invalid,
    /// KeyLocator contains a Name
    KeyLocator_Name = tlv::Name,
    /// KeyLocator contains a KeyDigest
    KeyLocator_KeyDigest = tlv::KeyDigest,
  };

public: // constructors
  /** \brief Construct an empty KeyLocator.
   *  \post `empty() == true`
   */
  KeyLocator();

  /** \brief Construct from Name.
   *  \note Implicit conversion is permitted.
   *  \post `getType() == tlv::Name`
   */
  KeyLocator(const Name& name);

  /** \brief Construct from wire encoding.
   */
  explicit
  KeyLocator(const Block& wire);

public: // encode and decode
  /** \brief Prepend wire encoding to \p encoder.
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  const Block&
  wireEncode() const;

  /** \brief Decode from wire encoding.
   *  \throw Error outer TLV type is not KeyLocator
   *  \note No error is raised for an unrecognized nested TLV, but attempting to reencode will throw.
   */
  void
  wireDecode(const Block& wire);

public: // attributes
  NDN_CXX_NODISCARD bool
  empty() const
  {
    return holds_alternative<monostate>(m_locator);
  }

  uint32_t
  getType() const;

  /** \brief Reset KeyLocator to its default-constructed state.
   *  \post `empty() == true`
   *  \post `getType() == tlv::Invalid`
   *  \return self
   */
  KeyLocator&
  clear();

  /** \brief Get nested Name element.
   *  \throw Error if type is not tlv::Name
   */
  const Name&
  getName() const;

  /** \brief Set nested Name element.
   *  \post `getType() == tlv::Name`
   *  \return self
   */
  KeyLocator&
  setName(const Name& name);

  /** \brief Get nested KeyDigest element.
   *  \throw Error if type is not tlv::KeyDigest
   */
  const Block&
  getKeyDigest() const;

  /** \brief Set nested KeyDigest element (whole TLV).
   *  \post `getType() == tlv::KeyDigest`
   *  \throw std::invalid_argument Block type is not tlv::KeyDigest
   *  \return self
   */
  KeyLocator&
  setKeyDigest(const Block& keyDigest);

  /** \brief Set nested KeyDigest element value.
   *  \param keyDigest buffer to use as TLV-VALUE of the nested KeyDigest element.
   *  \post `getType() == tlv::KeyDigest`
   *  \return self
   */
  KeyLocator&
  setKeyDigest(const ConstBufferPtr& keyDigest);

private: // non-member operators
  // NOTE: the following "hidden friend" operators are available via
  //       argument-dependent lookup only and must be defined inline.

  friend bool
  operator==(const KeyLocator& lhs, const KeyLocator& rhs)
  {
    return lhs.m_locator == rhs.m_locator;
  }

  friend bool
  operator!=(const KeyLocator& lhs, const KeyLocator& rhs)
  {
    return lhs.m_locator != rhs.m_locator;
  }

private:
  // - monostate represents an empty KeyLocator, without any nested TLVs
  // - Name is used when the nested TLV contains a name
  // - Block is used when the nested TLV is a KeyDigest
  // - in all other (unsupported) cases the nested TLV type is stored as uint32_t
  variant<monostate, Name, Block, uint32_t> m_locator;

  mutable Block m_wire;

  friend std::ostream& operator<<(std::ostream&, const KeyLocator&);
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(KeyLocator);

std::ostream&
operator<<(std::ostream& os, const KeyLocator& keyLocator);

} // namespace ndn

#endif // NDN_KEY_LOCATOR_HPP

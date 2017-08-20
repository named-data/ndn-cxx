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

#ifndef NDN_KEY_LOCATOR_HPP
#define NDN_KEY_LOCATOR_HPP

#include "encoding/encoding-buffer.hpp"
#include "name.hpp"

namespace ndn {

class KeyLocator
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

  enum Type {
    /** \brief indicates KeyLocator is empty (internal use only)
     */
    KeyLocator_None = 65535,
    /** \brief indicates KeyLocator contains a Name
     */
    KeyLocator_Name = 0,
    /** \brief indicates KeyLocator contains a KeyDigest
     */
    KeyLocator_KeyDigest = 1,
    /** \brief indicates KeyLocator contains an unknown element
     */
    KeyLocator_Unknown = 255
  };

public: // constructors
  /** \brief construct an empty KeyLocator
   */
  KeyLocator();

  /** \brief construct from wire encoding
   */
  explicit
  KeyLocator(const Block& wire);

  /** \brief construct from Name
   *  \note implicit conversion is permitted
   */
  KeyLocator(const Name& name);

public: // encode and decode
  /** \brief prepend wire encoding
   *  \param encoder EncodingBuffer or Estimator
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** \return wire encoding
   */
  const Block&
  wireEncode() const;

  /** \brief decode from wire encoding
   *  \throw Error outer TLV type is not KeyLocator
   *  \note No error is thrown for unrecognized inner TLV, but type becomes KeyLocator_Unknown.
   */
  void
  wireDecode(const Block& wire);

public: // attributes
  bool
  empty() const
  {
    return m_type == KeyLocator_None;
  }

  Type
  getType() const
  {
    return m_type;
  }

  /** \brief clear KeyLocator
   *  \details type becomes KeyLocator_None
   *  \return self
   */
  KeyLocator&
  clear();

  /** \brief get Name element
   *  \throw Error if type is not KeyLocator_Name
   */
  const Name&
  getName() const;

  /** \brief set Name element
   *  \details type becomes KeyLocator_Name
   *  \return self
   */
  KeyLocator&
  setName(const Name& name);

  /** \brief get KeyDigest element
   *  \throw Error if type is not KeyLocator_KeyDigest
   */
  const Block&
  getKeyDigest() const;

  /** \brief set KeyDigest element
   *  \details type becomes KeyLocator_KeyDigest
   *  \throw Error if Block type is not KeyDigest
   *  \return self
   */
  KeyLocator&
  setKeyDigest(const Block& keyDigest);

  /** \brief set KeyDigest value
   *  \details type becomes KeyLocator_KeyDigest
   *  \return self
   */
  KeyLocator&
  setKeyDigest(const ConstBufferPtr& keyDigest);

public: // EqualityComparable concept
  bool
  operator==(const KeyLocator& other) const;

  bool
  operator!=(const KeyLocator& other) const
  {
    return !this->operator==(other);
  }

private:
  Type m_type;
  Name m_name;
  Block m_keyDigest;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(KeyLocator);

std::ostream&
operator<<(std::ostream& os, const KeyLocator& keyLocator);

} // namespace ndn

#endif // NDN_KEY_LOCATOR_HPP

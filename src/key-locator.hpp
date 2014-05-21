/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "encoding/block.hpp"
#include "name.hpp"

namespace ndn {

class KeyLocator
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

  enum {
    KeyLocator_None = 65535, // just an arbitrarily large number (used only internally)
    KeyLocator_Name = 0,

    KeyLocator_Unknown = 255
  };

  KeyLocator()
    : m_type(KeyLocator_None)
  {
  }

  KeyLocator(const Name& name)
  {
    setName(name);
  }

  /**
   * @brief Create from wire encoding
   */
  explicit
  KeyLocator(const Block& wire)
  {
    wireDecode(wire);
  }

  ///////////////////////////////////////////////////////////////////////////////

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& block) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

  ///////////////////////////////////////////////////////////////////////////////

  bool
  empty() const
  {
    return m_type == KeyLocator_None;
  }

  uint32_t
  getType() const { return m_type; }

  ////////////////////////////////////////////////////////
  // Helper methods for different types of key locators
  //
  // For now only Name type is actually supported

  const Name&
  getName() const;

  void
  setName(const Name& name);

public: // EqualityComparable concept
  bool
  operator==(const KeyLocator& other) const;

  bool
  operator!=(const KeyLocator& other) const;

private:
  uint32_t m_type;
  Name m_name;

  mutable Block m_wire;
};

template<bool T>
inline size_t
KeyLocator::wireEncode(EncodingImpl<T>& block) const
{
  // KeyLocator ::= KEY-LOCATOR-TYPE TLV-LENGTH KeyLocatorValue

  // KeyLocatorValue ::= Name |
  //                     KeyLocatorDigest |     (not supported yet)
  //                     ...

  // KeyLocatorDigest ::= KEY-LOCATOR-DIGEST-TYPE TLV-LENGTH BYTE+

  size_t totalLength = 0;

  switch (m_type) {
  case KeyLocator_None:
    break;
  case KeyLocator_Name:
    totalLength += m_name.wireEncode(block);
    break;
  default:
    throw Error("Unsupported KeyLocator type");
  }

  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(Tlv::KeyLocator);
  return totalLength;
}

inline const Block&
KeyLocator::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

inline void
KeyLocator::wireDecode(const Block& value)
{
  if (value.type() != Tlv::KeyLocator)
    throw Error("Unexpected TLV type during KeyLocator decoding");

  m_wire = value;
  m_wire.parse();

  if (!m_wire.elements().empty() && m_wire.elements_begin()->type() == Tlv::Name)
    {
      m_type = KeyLocator_Name;
      m_name.wireDecode(*m_wire.elements_begin());
    }
  else
    {
      m_type = KeyLocator_Unknown;
    }
}

inline const Name&
KeyLocator::getName() const
{
  if (m_type != KeyLocator_Name)
    throw Error("Requested Name, but KeyLocator is not of the Name type");

  return m_name;
}

inline void
KeyLocator::setName(const Name& name)
{
  m_wire.reset();
  m_type = KeyLocator_Name;
  m_name = name;
}

inline bool
KeyLocator::operator==(const KeyLocator& other) const
{
  return wireEncode() == other.wireEncode();
}

inline bool
KeyLocator::operator!=(const KeyLocator& other) const
{
  return !this->operator==(other);
}

} // namespace ndn

#endif

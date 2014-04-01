/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_LOCATOR_HPP
#define NDN_KEY_LOCATOR_HPP

#include "encoding/block.hpp"
#include "name.hpp"

namespace ndn {

class KeyLocator {
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  enum {
    KeyLocator_None = 65535, // just an arbitrarily large number (used only internally)
    KeyLocator_Name = 0,

    KeyLocator_Unknown = 255
  };

  inline
  KeyLocator()
    : m_type(KeyLocator_None)
  {
  }

  inline
  KeyLocator(const Name &name);

  ///////////////////////////////////////////////////////////////////////////////

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T> &block) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block &wire);

  ///////////////////////////////////////////////////////////////////////////////

  inline bool
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

  inline const Name&
  getName() const;

  inline void
  setName(const Name &name);

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

inline
KeyLocator::KeyLocator(const Name &name)
{
  setName(name);
}

template<bool T>
inline size_t
KeyLocator::wireEncode(EncodingImpl<T>& block) const
{
  // KeyLocator ::= KEY-LOCATOR-TYPE TLV-LENGTH KeyLocatorValue

  // KeyLocatorValue ::= Name |
  //                     KeyLocatorDigest |     (not supported yet)
  //                     ...

  // KeyLocatorDigest ::= KEY-LOCATOR-DIGEST-TYPE TLV-LENGTH BYTE+

  size_t total_len = 0;

  switch (m_type) {
  case KeyLocator_None:
    break;
  case KeyLocator_Name:
    total_len += m_name.wireEncode(block);
    break;
  default:
    throw Error("Unsupported KeyLocator type");
  }

  total_len += block.prependVarNumber (total_len);
  total_len += block.prependVarNumber (Tlv::KeyLocator);
  return total_len;
}

inline const Block&
KeyLocator::wireEncode() const
{
  if (m_wire.hasWire ())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

inline void
KeyLocator::wireDecode(const Block &value)
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
KeyLocator::setName(const Name &name)
{
  m_type = KeyLocator_Name;
  m_name = name;
}

inline bool
KeyLocator::operator==(const KeyLocator& other) const
{
  if (this->getType() != other.getType()) {
    return false;
  }

  switch (this->getType()) {
    case KeyLocator_Name:
      if (this->getName() != other.getName()) {
        return false;
      }
      break;
  }

  return true;
}

inline bool
KeyLocator::operator!=(const KeyLocator& other) const
{
  return !this->operator==(other);
}

} // namespace ndn

#endif

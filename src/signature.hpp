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

#ifndef NDN_SIGNATURE_HPP
#define NDN_SIGNATURE_HPP

namespace ndn {

/**
 * A Signature is storage for the signature-related information (info and value) in a Data packet.
 */
class Signature
{
public:
  class Error : public Tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Tlv::Error(what)
    {
    }
  };

  enum {
    Sha256 = 0,
    Sha256WithRsa = 1
  };

  Signature()
    : m_type(-1)
  {
  }

  Signature(const Block& info, const Block& value = Block())
    : m_value(value)
  {
    setInfo(info);
  }

  operator bool() const
  {
    return m_type != -1;
  }

  uint32_t
  getType() const
  {
    return m_type;
  }

  const Block&
  getInfo() const
  {
    m_info.encode(); // will do nothing if wire already exists
    return m_info;
  }

  void
  setInfo(const Block& info)
  {
    m_info = info;
    if (m_info.hasWire() || m_info.hasValue())
      {
        m_info.parse();
        const Block& signatureType = m_info.get(Tlv::SignatureType);
        m_type = readNonNegativeInteger(signatureType);
      }
    else
      {
        m_type = -1;
      }
  }

  const Block&
  getValue() const
  {
    m_value.encode(); // will do nothing if wire already exists
    return m_value;
  }

  void
  setValue(const Block& value)
  {
    m_value = value;
  }

  void
  reset()
  {
    m_type = -1;
    m_info.reset();
    m_value.reset();
  }

public: // EqualityComparable concept
  bool
  operator==(const Signature& other) const
  {
    return getInfo() == other.getInfo() &&
      getValue() == other.getValue();
  }

  bool
  operator!=(const Signature& other) const
  {
    return !(*this == other);
  }

protected:
  int32_t m_type;

  mutable Block m_info;
  mutable Block m_value;
};

} // namespace ndn

#endif // NDN_SIGNATURE_HPP

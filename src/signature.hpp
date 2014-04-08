/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013-2014 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SIGNATURE_HPP
#define NDN_SIGNATURE_HPP

namespace ndn {

/**
 * A Signature is storage for the signature-related information (info and value) in a Data packet.
 */
class Signature {
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

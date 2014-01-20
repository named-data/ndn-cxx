/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
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
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  enum {
    Sha256 = 0,
    Sha256WithRsa = 1
  };
  
  Signature()
    : type_(-1)
  {
  }
  
  Signature(const Block &info, const Block &value = Block())
    : value_(value)
  {
    setInfo(info);
  }

  operator bool() const
  {
    return type_ != -1;
  }

  uint32_t
  getType() const
  {
    return type_;
  }
  
  const Block&
  getInfo() const
  {
    info_.encode(); // will do nothing if wire already exists
    return info_;
  }

  void
  setInfo(const Block &info)
  {
    info_ = info;
    if (info_.hasWire() || info_.hasValue())
      {
        info_.parse();
        const Block &signatureType = info_.get(Tlv::SignatureType);
        type_ = readNonNegativeInteger(signatureType);
      }
    else
      {
        type_ = -1;
      }
  }  

  const Block&
  getValue() const
  {
    value_.encode(); // will do nothing if wire already exists
    return value_;
  }

  void
  setValue(const Block &value)
  {
    value_ = value;
  }

  void
  reset()
  {
    type_ = -1;
    info_.reset();
    value_.reset();
  }

protected:
  int32_t type_;
  
  mutable Block info_;
  mutable Block value_;
};

} // namespace ndn

#endif // NDN_SIGNATURE_HPP

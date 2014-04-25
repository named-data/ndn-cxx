/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#ifndef NDN_SECURITY_SIGNATURE_SHA256_HPP
#define NDN_SECURITY_SIGNATURE_SHA256_HPP

#include "../data.hpp"
#include "../encoding/tlv.hpp"

namespace ndn {

/**
 * Representing of SHA256 signature in a data packet.
 */
class SignatureSha256 : public Signature
{
public:
  class Error : public Signature::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Signature::Error(what)
    {
    }
  };

  SignatureSha256()
  {
    m_info = Block(Tlv::SignatureInfo);

    m_type = Signature::Sha256;
    m_info.push_back(nonNegativeIntegerBlock(Tlv::SignatureType, Tlv::DigestSha256));
  }

  SignatureSha256(const Signature& signature)
    : Signature(signature)
  {
    if (getType() != Signature::Sha256)
      throw Error("Incorrect signature type");
  }
};

} // namespace ndn

#endif //NDN_SECURITY_SIGNATURE_SHA256_HPP

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_SIGNATURE_SHA256_HPP
#define NDN_SECURITY_SIGNATURE_SHA256_HPP

#include "../data.hpp"
#include "../encoding/tlv.hpp"

namespace ndn {

/**
 * Representing of SHA256 signature in a data packet.
 */
class SignatureSha256 : public Signature {
public:
  SignatureSha256()
  {
    info_ = Block(Tlv::SignatureInfo);
    
    type_ = Signature::Sha256;
    info_.push_back(nonNegativeIntegerBlock(Tlv::SignatureType, Tlv::DigestSha256));
  }
  
  SignatureSha256(const Signature &signature)
    : Signature(signature)
  {
    if (getType() != Signature::Sha256)
      throw Signature::Error("Incorrect signature type");
  }
};

} // namespace ndn

#endif //NDN_SECURITY_SIGNATURE_SHA256_HPP

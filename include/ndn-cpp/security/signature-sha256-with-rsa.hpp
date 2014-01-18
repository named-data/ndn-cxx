/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SIGNATURE_SHA256_WITH_RSA_HPP
#define NDN_SIGNATURE_SHA256_WITH_RSA_HPP

#include "../data.hpp"

namespace ndn {

/**
 * Representing of SHA256-with-RSA signature in a data packet.
 */
class SignatureSha256WithRsa : public Signature {
public:
  SignatureSha256WithRsa()
  {
    info_ = Block(Tlv::SignatureInfo);
    
    type_ = Signature::Sha256WithRsa;
    info_.push_back(nonNegativeIntegerBlock(Tlv::SignatureType, Tlv::SignatureSha256WithRsa));
    info_.push_back(keyLocator_.wireEncode());
  }
  
  SignatureSha256WithRsa(const Signature &signature)
    : Signature(signature)
  {
    if (getType() != Signature::Sha256WithRsa)
      throw Signature::Error("Incorrect signature type");

    info_.parse();
    Block::element_iterator i = info_.find(Tlv::KeyLocator);
    if (i != info_.getAll().end())
      {
        keyLocator_.wireDecode(*i);
      }
  }
  
  const KeyLocator& 
  getKeyLocator() const
  {
    return keyLocator_;
  }

  void 
  setKeyLocator(const KeyLocator& keyLocator)
  {
    keyLocator_ = keyLocator;

    info_.remove(ndn::Tlv::KeyLocator);
    info_.push_back(keyLocator_.wireEncode());
  }

private:
  KeyLocator keyLocator_;
};

} // namespace ndn

#endif

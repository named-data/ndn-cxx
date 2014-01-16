/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SHA256_RSA_HANDLER_HPP
#define NDN_SHA256_RSA_HANDLER_HPP

#include "../../data.hpp"
#include "../certificate/public-key.hpp"

namespace ndn{

class Sha256WithRsaHandler {
public:
  Sha256WithRsaHandler() {}

  virtual
  ~Sha256WithRsaHandler() {}

 /**
  * Verify the signature on the data packet using the given public key. If there is no data.getDefaultWireEncoding(),
  * this calls data.wireEncode() to set it.
  * @param data The data packet with the signed portion and the signature to verify. The data packet must have a
  * Sha256WithRsaSignature.
  * @param publicKey The public key used to verify the signature.
  * @return true if the signature verifies, false if not.
  * @throw SecurityException if data does not have a Sha256WithRsaSignature.
  */
  static bool
  verifySignature(const Data& data, const PublicKey& publicKey);

};

}
#endif

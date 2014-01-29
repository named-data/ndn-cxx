/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_PUBLIC_KEY_HPP
#define NDN_PUBLIC_KEY_HPP

#include <stdexcept>
#include "../encoding/oid.hpp"
#include "../encoding/buffer.hpp"
#include "security-common.hpp"

namespace ndn {

class PublicKey {
public:    
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  /**
   * The default constructor.
   */
  PublicKey();

  /**
   * Create a new PublicKey with the given values.
   * @param algorithm The algorithm of the public key.
   * @param keyDer The blob of the PublicKeyInfo in terms of DER.
   *
   * @throws PublicKey::Error If algorithm is not supported or keyDer cannot be decoded
   */
  PublicKey(const uint8_t *keyDerBuf, size_t keyDerSize);

  inline const Buffer&
  get() const
  {
    return key_;
  }

  inline void
  set(const uint8_t *keyDerBuf, size_t keyDerSize)
  {
    Buffer buf(keyDerBuf, keyDerSize);
    key_.swap(buf);
  }

  void
  encode(CryptoPP::BufferedTransformation &out) const;

  void
  decode(CryptoPP::BufferedTransformation &in);

  // /*
  //  * Get the digest of the public key.
  //  * @param digestAlgorithm The digest algorithm. If omitted, use DIGEST_ALGORITHM_SHA256 by default.
  //  */
  // Blob 
  // getDigest(DigestAlgorithm digestAlgorithm = DIGEST_ALGORITHM_SHA256) const;

  inline bool
  operator ==(const PublicKey &key) const
  {
    return key_ == key.key_;
  }

  inline bool
  operator !=(const PublicKey &key) const
  {
    return key_ != key.key_;
  }
  
private:
  Buffer key_;
};

std::ostream &
operator <<(std::ostream &os, const PublicKey &key);

}

#endif

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

// We can use ndnboost::iostreams because this is internal and will not conflict with the application if it uses boost::iostreams.
#include <ndnboost/iostreams/stream.hpp>
#include <ndnboost/iostreams/device/array.hpp>
#include <ndn-cpp/security//security-exception.hpp>
#include "../../c/util/crypto.h"
#include "../../encoding/der/der.hpp"
#include <ndn-cpp/security/certificate/public-key.hpp>

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

shared_ptr<der::DerNode>
PublicKey::toDer()
{
  ndnboost::iostreams::stream<ndnboost::iostreams::array_source> is((const char*)keyDer_.buf (), keyDer_.size ());

  return der::DerNode::parse(reinterpret_cast<der::InputIterator&> (is));
}

static int RSA_OID[] = { 1, 2, 840, 113549, 1, 1, 1 };

shared_ptr<PublicKey>
PublicKey::fromDer(const Blob& keyDer)
{
  // Use a temporary pointer since d2i updates it.
  const uint8_t *derPointer = keyDer.buf();
  RSA *publicKey = d2i_RSA_PUBKEY(NULL, &derPointer, keyDer.size());
  if (!publicKey)
    throw UnrecognizedKeyFormatException("Error decoding public key DER");  
  RSA_free(publicKey);
  
  return shared_ptr<PublicKey>(new PublicKey(OID(vector<int>(RSA_OID, RSA_OID + sizeof(RSA_OID))), keyDer));
}

Blob
PublicKey::getDigest(DigestAlgorithm digestAlgorithm) const
{
  if (digestAlgorithm == DIGEST_ALGORITHM_SHA256) {
    uint8_t digest[SHA256_DIGEST_LENGTH];
    ndn_digestSha256(keyDer_.buf(), keyDer_.size(), digest);
    
    return Blob(digest, sizeof(digest));
  }
  else
    throw UnrecognizedDigestAlgorithmException("Wrong format!");
}

}

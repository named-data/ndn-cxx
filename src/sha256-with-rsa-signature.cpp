/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "c/data.h"
#include <ndn-cpp/sha256-with-rsa-signature.hpp>

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

shared_ptr<Signature> 
Sha256WithRsaSignature::clone() const
{
  return shared_ptr<Signature>(new Sha256WithRsaSignature(*this));
}

void 
Sha256WithRsaSignature::get(struct ndn_Signature& signatureStruct) const 
{
  digestAlgorithm_.get(signatureStruct.digestAlgorithm);
  witness_.get(signatureStruct.witness);
  signature_.get(signatureStruct.signature);  
  publisherPublicKeyDigest_.get(signatureStruct.publisherPublicKeyDigest);
  keyLocator_.get(signatureStruct.keyLocator);
}

void 
Sha256WithRsaSignature::set(const struct ndn_Signature& signatureStruct)
{
  digestAlgorithm_ = Blob(signatureStruct.digestAlgorithm);
  witness_ = Blob(signatureStruct.witness);
  signature_ = Blob(signatureStruct.signature);
  publisherPublicKeyDigest_.set(signatureStruct.publisherPublicKeyDigest);
  keyLocator_.set(signatureStruct.keyLocator);
}

}

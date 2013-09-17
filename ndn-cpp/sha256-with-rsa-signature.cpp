/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "sha256-with-rsa-signature.hpp"

using namespace std;

namespace ndn {

ptr_lib::shared_ptr<Signature> 
Sha256WithRsaSignature::clone() const
{
  return ptr_lib::shared_ptr<Signature>(new Sha256WithRsaSignature(*this));
}

void 
Sha256WithRsaSignature::get(struct ndn_Signature& signatureStruct) const 
{
  signatureStruct.digestAlgorithmLength = digestAlgorithm_.size();
  if (digestAlgorithm_.size() > 0)
    signatureStruct.digestAlgorithm = (unsigned char *)digestAlgorithm_.buf();
  else
    signatureStruct.digestAlgorithm = 0;

  signatureStruct.witnessLength = witness_.size();
  if (witness_.size() > 0)
    signatureStruct.witness = (unsigned char *)witness_.buf();
  else
    signatureStruct.witness = 0;

  signatureStruct.signatureLength = signature_.size();
  if (signature_.size() > 0)
    signatureStruct.signature = (unsigned char *)signature_.buf();
  else
    signatureStruct.signature = 0;
  
  publisherPublicKeyDigest_.get(signatureStruct.publisherPublicKeyDigest);
  keyLocator_.get(signatureStruct.keyLocator);
}

void 
Sha256WithRsaSignature::set(const struct ndn_Signature& signatureStruct)
{
  digestAlgorithm_ = Blob(signatureStruct.digestAlgorithm, signatureStruct.digestAlgorithmLength);
  witness_ = Blob(signatureStruct.witness, signatureStruct.witnessLength);
  signature_ = Blob(signatureStruct.signature, signatureStruct.signatureLength);
  publisherPublicKeyDigest_.set(signatureStruct.publisherPublicKeyDigest);
  keyLocator_.set(signatureStruct.keyLocator);
}

}

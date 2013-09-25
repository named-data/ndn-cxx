/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SHA256_WITH_RSA_SIGNATURE_HPP
#define	NDN_SHA256_WITH_RSA_SIGNATURE_HPP

#include "data.hpp"
#include "key.hpp"
#include "publisher-public-key-digest.hpp"

namespace ndn {

/**
 * A Sha256WithRsaSignature extends Signature and holds the signature bits and other info representing a
 * SHA256-with-RSA signature in a data packet.
 */
class Sha256WithRsaSignature : public Signature {
public:
  /**
   * Return a pointer to a new Sha256WithRsaSignature which is a copy of this signature.
   */
  virtual ptr_lib::shared_ptr<Signature> 
  clone() const;

  /**
   * Set the signatureStruct to point to the values in this signature object, without copying any memory.
   * WARNING: The resulting pointers in signatureStruct are invalid after a further use of this object which could reallocate memory.
   * @param signatureStruct a C ndn_Signature struct where the name components array is already allocated.
   */
  virtual void 
  get(struct ndn_Signature& signatureStruct) const;

  /**
   * Clear this signature, and set the values by copying from the ndn_Signature struct.
   * @param signatureStruct a C ndn_Signature struct
   */
  virtual void 
  set(const struct ndn_Signature& signatureStruct);

  const Blob& 
  getDigestAlgorithm() const { return digestAlgorithm_; }

  const Blob& 
  getWitness() const { return witness_; }

  const Blob& 
  getSignature() const { return signature_; }
  
  const PublisherPublicKeyDigest& 
  getPublisherPublicKeyDigest() const { return publisherPublicKeyDigest_; }
  
  PublisherPublicKeyDigest& 
  getPublisherPublicKeyDigest() { return publisherPublicKeyDigest_; }
  
  const KeyLocator& 
  getKeyLocator() const { return keyLocator_; }
  
  KeyLocator& 
  getKeyLocator() { return keyLocator_; }

  void 
  setDigestAlgorithm(const std::vector<uint8_t>& digestAlgorithm) { digestAlgorithm_ = digestAlgorithm; }
  
  void 
  setDigestAlgorithm(const uint8_t *digestAlgorithm, size_t digestAlgorithmLength) 
  { 
    digestAlgorithm_ = Blob(digestAlgorithm, digestAlgorithmLength); 
  }

  void 
  setWitness(const std::vector<uint8_t>& witness) { witness_ = witness; }
  
  void 
  setWitness(const uint8_t *witness, size_t witnessLength) 
  { 
    witness_ = Blob(witness, witnessLength); 
  }

  void 
  setSignature(const std::vector<uint8_t>& signature) { signature_ = signature; }
  
  void 
  setSignature(const uint8_t *signature, size_t signatureLength) 
  { 
    signature_ = Blob(signature, signatureLength); 
  }

  /**
   * Set signature to point to an existing byte array.  IMPORTANT: After calling this,
   * if you keep a pointer to the array then you must treat the array as immutable and promise not to change it.
   * @param signature A pointer to a vector with the byte array.  This takes another reference and does not copy the bytes.
   */
  void 
  setSignature(const ptr_lib::shared_ptr<std::vector<uint8_t> > &signature) { signature_ = signature; }

  void 
  setSignature(const ptr_lib::shared_ptr<const std::vector<uint8_t> > &signature) { signature_ = signature; }

  void 
  setPublisherPublicKeyDigest(const PublisherPublicKeyDigest& publisherPublicKeyDigest) { publisherPublicKeyDigest_ = publisherPublicKeyDigest; }
  
  void 
  setKeyLocator(const KeyLocator& keyLocator) { keyLocator_ = keyLocator; }
  
  /**
   * Clear all the fields.
   */
  void 
  clear()
  {
    digestAlgorithm_.reset();
    witness_.reset();
    signature_.reset();
    publisherPublicKeyDigest_.clear();
    keyLocator_.clear();
  }

private:
  Blob digestAlgorithm_; /**< if empty, the default is 2.16.840.1.101.3.4.2.1 (sha-256) */
  Blob witness_;
  Blob signature_;
  PublisherPublicKeyDigest publisherPublicKeyDigest_;
  KeyLocator keyLocator_;
};

}

#endif

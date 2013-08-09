/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DATA_HPP
#define NDN_DATA_HPP

#include "common.hpp"
#include "name.hpp"
#include "publisher-public-key-digest.hpp"
#include "key.hpp"
#include "c/data.h"

namespace ndn {

class Signature {
public:
  /**
   * Set the signatureStruct to point to the values in this signature object, without copying any memory.
   * WARNING: The resulting pointers in signatureStruct are invalid after a further use of this object which could reallocate memory.
   * @param signatureStruct a C ndn_Signature struct where the name components array is already allocated.
   */
  void get(struct ndn_Signature &signatureStruct) const;

  /**
   * Clear this signature, and set the values by copying from the ndn_Signature struct.
   * @param signatureStruct a C ndn_Signature struct
   */
  void set(const struct ndn_Signature &signatureStruct);

  const std::vector<unsigned char> getDigestAlgorithm() const { return digestAlgorithm_; }
  std::vector<unsigned char> getDigestAlgorithm() { return digestAlgorithm_; }

  const std::vector<unsigned char> getWitness() const { return witness_; }
  std::vector<unsigned char> getWitness() { return witness_; }

  const std::vector<unsigned char> getSignature() const { return signature_; }
  std::vector<unsigned char> getSignature() { return signature_; }

  void setDigestAlgorithm(const std::vector<unsigned char> &digestAlgorithm) { digestAlgorithm_ = digestAlgorithm; }
  void setDigestAlgorithm(const unsigned char *digestAlgorithm, unsigned int digestAlgorithmLength) 
  { 
    setVector(digestAlgorithm_, digestAlgorithm, digestAlgorithmLength); 
  }

  void setWitness(const std::vector<unsigned char> &witness) { witness_ = witness; }
  void setWitness(const unsigned char *witness, unsigned int witnessLength) 
  { 
    setVector(witness_, witness, witnessLength); 
  }

  void setSignature(const std::vector<unsigned char> &signature) { signature_ = signature; }
  void setSignature(const unsigned char *signature, unsigned int signatureLength) 
  { 
    setVector(signature_, signature, signatureLength); 
  }

private:
  std::vector<unsigned char> digestAlgorithm_; /**< if empty, the default is 2.16.840.1.101.3.4.2.1 (sha-256) */
  std::vector<unsigned char> witness_;
  std::vector<unsigned char> signature_;
};

class SignedInfo {
public:
  SignedInfo() 
  {   
    type_ = ndn_ContentType_DATA;
    freshnessSeconds_ = -1;
  }

  /**
   * Set the signedInfoStruct to point to the values in this signed info object, without copying any memory.
   * WARNING: The resulting pointers in signedInfoStruct are invalid after a further use of this object which could reallocate memory.
   * @param signedInfoStruct a C ndn_SignedInfo struct where the name components array is already allocated.
   */
  void get(struct ndn_SignedInfo &signedInfoStruct) const;

  /**
   * Clear this signed info, and set the values by copying from the ndn_SignedInfo struct.
   * @param signedInfoStruct a C ndn_SignedInfo struct
   */
  void set(const struct ndn_SignedInfo &signedInfoStruct);

  const PublisherPublicKeyDigest &getPublisherPublicKeyDigest() const { return publisherPublicKeyDigest_; }
  PublisherPublicKeyDigest &getPublisherPublicKeyDigest() { return publisherPublicKeyDigest_; }
  
  double getTimestampMilliseconds() const { return timestampMilliseconds_; }
  
  int getType() const { return type_; }
  
  int getFreshnessSeconds() const { return freshnessSeconds_; }
  
  const std::vector<unsigned char> getFinalBlockID() const { return finalBlockID_; }
  std::vector<unsigned char> getFinalBlockID() { return finalBlockID_; }
  
  const KeyLocator &getKeyLocator() const { return keyLocator_; }
  KeyLocator &getKeyLocator() { return keyLocator_; }

  void setPublisherPublicKeyDigest(const PublisherPublicKeyDigest &publisherPublicKeyDigest) { publisherPublicKeyDigest_ = publisherPublicKeyDigest; }
  
  void setTimestampMilliseconds(double timestampMilliseconds) { timestampMilliseconds_ = timestampMilliseconds; }
  
  void setType(int type) { type_ = type; }
  
  void setFreshnessSeconds(int freshnessSeconds) { freshnessSeconds_ = freshnessSeconds; }
  
  void setFinalBlockID(const std::vector<unsigned char> &finalBlockID) { finalBlockID_ = finalBlockID; }
  void setFinalBlockID(const unsigned char *finalBlockID, unsigned int finalBlockIdLength) 
  { 
    setVector(finalBlockID_, finalBlockID, finalBlockIdLength); 
  }
  
  void setKeyLocator(const KeyLocator &keyLocator) { keyLocator_ = keyLocator; }
  
private:
  PublisherPublicKeyDigest publisherPublicKeyDigest_;
  double timestampMilliseconds_; /**< milliseconds since 1/1/1970. -1 for none */
  int type_;                     /**< default is ndn_ContentType_DATA. -1 for none */
  int freshnessSeconds_;         /**< -1 for none */
  std::vector<unsigned char> finalBlockID_; /** size 0 for none */
  KeyLocator keyLocator_;
};
  
class Data {
public:
  Data() 
  {
  }
  
  Data(const Name &name)
  : name_(name)
  {
  }
  
  ptr_lib::shared_ptr<std::vector<unsigned char> > wireEncode(WireFormat &wireFormat) const 
  {
    return wireFormat.encodeData(*this);
  }
  ptr_lib::shared_ptr<std::vector<unsigned char> > wireEncode() const 
  {
    return wireEncode(*WireFormat::getDefaultWireFormat());
  }
  void wireDecode(const unsigned char *input, unsigned int inputLength, WireFormat &wireFormat) 
  {
    wireFormat.decodeData(*this, input, inputLength);
  }
  void wireDecode(const unsigned char *input, unsigned int inputLength) 
  {
    wireDecode(input, inputLength, *WireFormat::getDefaultWireFormat());
  }
  void wireDecode(const std::vector<unsigned char> &input, WireFormat &wireFormat) 
  {
    wireDecode(&input[0], input.size(), wireFormat);
  }
  void wireDecode(const std::vector<unsigned char> &input) 
  {
    wireDecode(&input[0], input.size());
  }
  
  /**
   * Set the dataStruct to point to the values in this interest, without copying any memory.
   * WARNING: The resulting pointers in dataStruct are invalid after a further use of this object which could reallocate memory.
   * @param dataStruct a C ndn_Data struct where the name components array is already allocated.
   */
  void get(struct ndn_Data &dataStruct) const;

  /**
   * Clear this data object, and set the values by copying from the ndn_Data struct.
   * @param dataStruct a C ndn_Data struct
   */
  void set(const struct ndn_Data &dataStruct);

  const Signature &getSignature() const { return signature_; }
  
  const Name &getName() const { return name_; }
  
  const SignedInfo &getSignedInfo() const { return signedInfo_; }
  
  const std::vector<unsigned char> getContent() const { return content_; }

  Signature &getSignature() { return signature_; }
  void setSignature(const Signature &signature) { signature_ = signature; }
  
  Name &getName() { return name_; }
  void setName(const Name &name) { name_ = name; }
  
  SignedInfo &getSignedInfo() { return signedInfo_; }
  void setSignedInfo(const SignedInfo &signedInfo) { signedInfo_ = signedInfo; }

  void setContent(const std::vector<unsigned char> &content) { content_ = content; }
  void setContent(const unsigned char *content, unsigned int contentLength) 
  { 
    setVector(content_, content, contentLength); 
  }
    
private:
  Signature signature_;
  Name name_;
  SignedInfo signedInfo_;
  std::vector<unsigned char> content_;
};
  
}

#endif

/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_CONTENTOBJECT_HPP
#define	NDN_CONTENTOBJECT_HPP

#include "Name.hpp"
#include "PublisherPublicKeyDigest.hpp"
#include "Key.hpp"
#include "c/ContentObject.h"

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

  const std::vector<unsigned char> getWitness() const { return witness_; }

  const std::vector<unsigned char> getSignature() const { return signature_; }
  
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
  
  // TODO: Implement timestamp
  
  int getType() const { return type_; }
  
  int getFreshnessSeconds() const { return freshnessSeconds_; }
  
  const std::vector<unsigned char> getFinalBlockID() const { return finalBlockID_; }
  
  const KeyLocator &getKeyLocator() const { return keyLocator_; }
  
private:
  PublisherPublicKeyDigest publisherPublicKeyDigest_;
  // TODO: Implement timestamp
  int type_;
  int freshnessSeconds_;
  std::vector<unsigned char> finalBlockID_;
  KeyLocator keyLocator_;
};
  
class ContentObject {
public:
  void encode(std::vector<unsigned char> &output, WireFormat &wireFormat) const 
  {
    wireFormat.encodeContentObject(*this, output);
  }
  void encode(std::vector<unsigned char> &output) const 
  {
    encode(output, BinaryXMLWireFormat::instance());
  }
  void decode(const unsigned char *input, unsigned int inputLength, WireFormat &wireFormat) 
  {
    wireFormat.decodeContentObject(*this, input, inputLength);
  }
  void decode(const unsigned char *input, unsigned int inputLength) 
  {
    decode(input, inputLength, BinaryXMLWireFormat::instance());
  }
  void decode(const std::vector<unsigned char> &input, WireFormat &wireFormat) 
  {
    decode(&input[0], input.size(), wireFormat);
  }
  void decode(const std::vector<unsigned char> &input) 
  {
    decode(&input[0], input.size());
  }
  
  /**
   * Set the contentObjectStruct to point to the values in this interest, without copying any memory.
   * WARNING: The resulting pointers in contentObjectStruct are invalid after a further use of this object which could reallocate memory.
   * @param contentObjectStruct a C ndn_ContentObject struct where the name components array is already allocated.
   */
  void get(struct ndn_ContentObject &contentObjectStruct) const;

  /**
   * Clear this content object, and set the values by copying from the ndn_ContentObject struct.
   * @param contentObjectStruct a C ndn_ContentObject struct
   */
  void set(const struct ndn_ContentObject &contentObjectStruct);

  const Signature &getSignature() const { return signature_; }
  
  const Name &getName() const { return name_; }
  
  const SignedInfo &getSignedInfo() const { return signedInfo_; }
  
  const std::vector<unsigned char> getContent() const { return content_; }
  
private:
  Signature signature_;
  Name name_;
  SignedInfo signedInfo_;
  std::vector<unsigned char> content_;
};
  
}

#endif

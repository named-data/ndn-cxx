/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DATA_HPP
#define NDN_DATA_HPP

#include "common.hpp"
#include "name.hpp"
#include "util/signed-blob.hpp"
#include "c/data.h"

namespace ndn {

/**
 * A Signature is an abstract base class providing methods to work with the signature information in a Data packet.
 * You must create an object of a subclass, for example Sha256WithRsaSignature.
 */
class Signature {
public:
  /**
   * Return a pointer to a new Signature which is a copy of this signature.
   * This is pure virtual, the subclass must implement it.
   */
  virtual ptr_lib::shared_ptr<Signature> 
  clone() const = 0;
  
  /**
   * The virtual destructor.
   */
  virtual 
  ~Signature();
  
    /**
   * Set the signatureStruct to point to the values in this signature object, without copying any memory.
   * WARNING: The resulting pointers in signatureStruct are invalid after a further use of this object which could reallocate memory.
   * This is pure virtual, the subclass must implement it.
   * @param signatureStruct a C ndn_Signature struct where the name components array is already allocated.
   */
  virtual void 
  get(struct ndn_Signature& signatureStruct) const = 0;

  /**
   * Clear this signature, and set the values by copying from the ndn_Signature struct.
   * This is pure virtual, the subclass must implement it.
   * @param signatureStruct a C ndn_Signature struct
   */
  virtual void 
  set(const struct ndn_Signature& signatureStruct) = 0;
};

/**
 * An MetaInfo holds the meta info which is signed inside the data packet.
 */
class MetaInfo {
public:
  MetaInfo() 
  {   
    type_ = ndn_ContentType_DATA;
    freshnessSeconds_ = -1;
  }

  /**
   * Set the metaInfoStruct to point to the values in this meta info object, without copying any memory.
   * WARNING: The resulting pointers in metaInfoStruct are invalid after a further use of this object which could reallocate memory.
   * @param metaInfoStruct a C ndn_MetaInfo struct where the name components array is already allocated.
   */
  void 
  get(struct ndn_MetaInfo& metaInfoStruct) const;

  /**
   * Clear this meta info, and set the values by copying from the ndn_MetaInfo struct.
   * @param metaInfoStruct a C ndn_MetaInfo struct
   */
  void 
  set(const struct ndn_MetaInfo& metaInfoStruct);

  double 
  getTimestampMilliseconds() const { return timestampMilliseconds_; }
  
  ndn_ContentType 
  getType() const { return type_; }
  
  int 
  getFreshnessSeconds() const { return freshnessSeconds_; }
  
  const Name::Component& 
  getFinalBlockID() const { return finalBlockID_; }
  
  void 
  setTimestampMilliseconds(double timestampMilliseconds) { timestampMilliseconds_ = timestampMilliseconds; }
  
  void 
  setType(ndn_ContentType type) { type_ = type; }
  
  void 
  setFreshnessSeconds(int freshnessSeconds) { freshnessSeconds_ = freshnessSeconds; }
  
  void 
  setFinalBlockID(const std::vector<uint8_t>& finalBlockID) { finalBlockID_ = Name::Component(finalBlockID); }
  
  void 
  setFinalBlockID(const uint8_t* finalBlockID, size_t finalBlockIdLength) 
  { 
    finalBlockID_ = Name::Component(finalBlockID, finalBlockIdLength); 
  }
  
private:
  double timestampMilliseconds_; /**< milliseconds since 1/1/1970. -1 for none */
  ndn_ContentType type_;         /**< default is ndn_ContentType_DATA. -1 for none */
  int freshnessSeconds_;         /**< -1 for none */
  Name::Component finalBlockID_; /** size 0 for none */
};
  
class Data {
public:
  /**
   * Create a new Data object with default values and where the signature is a blank Sha256WithRsaSignature.
   */
  Data();

  /**
   * Create a new Data object with the given name and default values and where the signature is a blank Sha256WithRsaSignature.
   * @param name A reference to the name which is copied.
   */
  Data(const Name& name);
  
  /**
   * Encode this Data for a particular wire format. Also, set the wireEncoding field to the encoded result.
   * This is not const because it updates the wireEncoding.
   * @param wireFormat A WireFormat object used to encode the input. If omitted, use WireFormat getDefaultWireFormat().
   * @return The encoded byte array.
   */
  SignedBlob 
  wireEncode(WireFormat& wireFormat = *WireFormat::getDefaultWireFormat());
  
  /**
   * Decode the input using a particular wire format and update this Data. Also, set the wireEncoding field to the input.
   * @param input The input byte array to be decoded.
   * @param inputLength The length of input.
   * @param wireFormat A WireFormat object used to decode the input. If omitted, use WireFormat getDefaultWireFormat().
   */
  void 
  wireDecode(const uint8_t* input, size_t inputLength, WireFormat& wireFormat = *WireFormat::getDefaultWireFormat());
  
  /**
   * Decode the input using a particular wire format and update this Data. Also, set the wireEncoding field to the input.
   * @param input The input byte array to be decoded.
   * @param wireFormat A WireFormat object used to decode the input. If omitted, use WireFormat getDefaultWireFormat().
   */
  void 
  wireDecode(const std::vector<uint8_t>& input, WireFormat& wireFormat = *WireFormat::getDefaultWireFormat()) 
  {
    wireDecode(&input[0], input.size(), wireFormat);
  }
  
  /**
   * Set the dataStruct to point to the values in this interest, without copying any memory.
   * WARNING: The resulting pointers in dataStruct are invalid after a further use of this object which could reallocate memory.
   * @param dataStruct a C ndn_Data struct where the name components array is already allocated.
   */
  void 
  get(struct ndn_Data& dataStruct) const;

  /**
   * Clear this data object, and set the values by copying from the ndn_Data struct.
   * @param dataStruct a C ndn_Data struct
   */
  void 
  set(const struct ndn_Data& dataStruct);

  const Signature* 
  getSignature() const { return signature_.get(); }
  
  Signature* 
  getSignature() 
  { 
    // TODO: Should add an OnChanged listener instead of always calling onChanged.
    onChanged();
    return signature_.get(); 
  }
  
  const Name& 
  getName() const { return name_; }
  
  Name& 
  getName() 
  { 
    // TODO: Should add an OnChanged listener instead of always calling onChanged.
    onChanged();
    return name_; 
  }
  
  const MetaInfo& 
  getMetaInfo() const { return metaInfo_; }
  
  MetaInfo& 
  getMetaInfo() 
  { 
    // TODO: Should add an OnChanged listener instead of always calling onChanged.
    onChanged();
    return metaInfo_; 
  }
  
  const Blob& 
  getContent() const { return content_; }

  /**
   * Return a pointer to the wireEncoding.  It may be null.
   */
  const SignedBlob&
  getWireEncoding() const { return wireEncoding_; }
  
  /**
   * Set the signature to a copy of the given signature.
   * @param signature The signature object which is cloned.
   */
  void 
  setSignature(const Signature& signature) 
  { 
    signature_ = signature.clone(); 
    onChanged();
  }
  
  /**
   * Set name to a copy of the given Name.
   * @param name The Name which is copied.
   */
  void 
  setName(const Name& name) 
  { 
    name_ = name; 
    onChanged();
  }
  
  /**
   * Set metaInfo to a copy of the given MetaInfo.
   * @param metaInfo The MetaInfo which is copied.
   */
  void 
  setMetainfo(const MetaInfo& metaInfo) 
  { 
    metaInfo_ = metaInfo; 
    onChanged();
  }

  /**
   * Set the content to a copy of the data in the vector.
   * @param content A vector whose contents are copied.
   */
  void 
  setContent(const std::vector<uint8_t>& content) 
  { 
    content_ = content; 
    onChanged();
  }
  
  void 
  setContent(const uint8_t* content, size_t contentLength) 
  { 
    content_ = Blob(content, contentLength); 
    onChanged();
  }
      
  /**
   * Set content to point to an existing byte array.  IMPORTANT: After calling this,
   * if you keep a pointer to the array then you must treat the array as immutable and promise not to change it.
   * @param content A pointer to a vector with the byte array.  This takes another reference and does not copy the bytes.
   */
  void 
  setContent(const ptr_lib::shared_ptr<std::vector<uint8_t> > &content) 
  { 
    content_ = content;
    onChanged();
  }
  
  void 
  setContent(const ptr_lib::shared_ptr<const std::vector<uint8_t> > &content) 
  { 
    content_ = content;
    onChanged();
  }

private:
  /**
   * Clear the wire encoding.
   */
  void 
  onChanged();
  
  ptr_lib::shared_ptr<Signature> signature_;
  Name name_;
  MetaInfo metaInfo_;
  Blob content_;
  SignedBlob wireEncoding_;
};
  
}

#endif

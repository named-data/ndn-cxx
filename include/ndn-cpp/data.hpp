/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DATA_HPP
#define NDN_DATA_HPP

#include "common.hpp"
#include "name.hpp"
#include "encoding/block.hpp"


namespace ndn {

/**
 * A Signature is storage for the signature-related information (info and value) in a Data packet.
 */
class Signature {
public:
  enum {
    DigestSha256 = 0,
    SignatureSha256WithRsa = 1
  };
  
  Signature()
    : type_(-1)
  {
  }
  
  Signature(const Block &info, const Block &value)
    : info_(info)
    , value_(value)
  {
    Buffer::const_iterator i = info_.value_begin();
    type_ = Tlv::readVarNumber(i, info_.value_end());
  }

  operator bool() const
  {
    return type_ != -1;
  }

  uint32_t
  getType() const
  {
    return type_;
  }
  
  const Block&
  getInfo() const
  {
    return info_;
  }

  void
  setInfo(const Block &info)
  {
    info_ = info;
    if (info_.hasWire() || info_.hasValue())
      {
        info_.parse();
        const Block &signatureType = info_.get(Tlv::SignatureType);
        
        Buffer::const_iterator i = signatureType.value_begin();
        type_ = Tlv::readVarNumber(i, signatureType.value_end());
      }
    else
      {
        type_ = -1;
      }
  }  

  const Block&
  getValue() const
  {
    return value_;
  }

  void
  setValue(const Block &value)
  {
    value_ = value;
  }

  void
  reset()
  {
    type_ = -1;
    info_.reset();
    value_.reset();
  }

private:
  int32_t type_;
  
  Block info_;
  Block value_;
};

/**
 * An MetaInfo holds the meta info which is signed inside the data packet.
 */
class MetaInfo {
public:
  enum {
    TYPE_DEFAULT = 0,
    TYPE_LINK = 1,
    TYPE_KEY = 2
  };
  
  MetaInfo()
    : type_(TYPE_DEFAULT)
    , freshnessPeriod_(-1)
  {   
  }
  
  uint32_t 
  getType() const
  { return type_; }
  
  void 
  setType(uint32_t type)
  { type_ = type; }
  
  Milliseconds 
  getFreshnessPeriod() const
  { return freshnessPeriod_; }
  
  void 
  setFreshnessPeriod(Milliseconds freshnessPeriod)
  { freshnessPeriod_ = freshnessPeriod; }

private:
  uint32_t type_;
  Milliseconds freshnessPeriod_;

  Block wire_;
};
  
class Data {
public:
  /**
   * Create a new Data object with default values and where the signature is a blank Sha256WithRsaSignature.
   */
  Data()
  {
  }

  /**
   * Create a new Data object with the given name and default values and where the signature is a blank Sha256WithRsaSignature.
   * @param name A reference to the name which is copied.
   */
  Data(const Name& name)
    : name_(name)
  {
  }
  
  /**
   * The virtual destructor.
   */
  virtual ~Data()
  {
  }
  
  /**
   * Encode this Data for a particular wire format. If wireFormat is the default wire format, also set the defaultWireEncoding 
   * field to the encoded result.
   * Even though this is const, if wireFormat is the default wire format we update the defaultWireEncoding.
   * @param wireFormat A WireFormat object used to encode the input. If omitted, use WireFormat getDefaultWireFormat().
   * @return The encoded byte array.
   */
  const Block& 
  wireEncode() const;
  
  void
  wireDecode(const Block &wire);
  
  /**
   * Decode the input using a particular wire format and update this Data. If wireFormat is the default wire format, also 
   * set the defaultWireEncoding field to the input.
   * @param input The input byte array to be decoded.
   * @param inputLength The length of input.
   * @param wireFormat A WireFormat object used to decode the input. If omitted, use WireFormat getDefaultWireFormat().
   */
  void
  wireDecode(const uint8_t* input, size_t inputLength);

  const Signature&
  getSignature() const
  {
    return signature_;
  }
  
  /**
   * Set the signature to a copy of the given signature.
   * @param signature The signature object which is cloned.
   * @return This Data so that you can chain calls to update values.
   */
  Data& 
  setSignature(const Signature& signature) 
  {
    signature_ = signature;
    onChanged();
    return *this;
  }
  
  const Name& 
  getName() const
  {
    return name_;
  }
  
  /**
   * Set name to a copy of the given Name.  This is virtual so that a subclass can override to validate the name.
   * @param name The Name which is copied.
   * @return This Data so that you can chain calls to update values.
   */
  void
  setName(const Name& name)
  { 
    name_ = name; 
    onChanged();
  }
  
  const MetaInfo& 
  getMetaInfo() const { return metaInfo_; }
  
  /**
   * Set metaInfo to a copy of the given MetaInfo.
   * @param metaInfo The MetaInfo which is copied.
   * @return This Data so that you can chain calls to update values.
   */
  void
  setMetaInfo(const MetaInfo& metaInfo) 
  { 
    metaInfo_ = metaInfo; 
    onChanged();
  }

  const Block& 
  getContent() const { return content_; }

  /**
   * Set the content to a copy of the data in the vector.
   * @param content A vector whose contents are copied.
   * @return This Data so that you can chain calls to update values.
   */
  void
  setContent(const std::vector<uint8_t>& content) 
  {
    setContent(&content[0], content.size());
    onChanged();
  }
  
  void
  setContent(const uint8_t* content, size_t contentLength) 
  {
    OBufferStream os;
    Tlv::writeVarNumber(os, Tlv::Content);
    Tlv::writeVarNumber(os, contentLength);
    os.write(reinterpret_cast<const char *>(content), contentLength);
    
    content_ = Block(os.buf());
    onChanged();
  }

  void
  setContent(const ConstBufferPtr &contentValue)
  {
    content_ = Block(Tlv::Content, contentValue); // not real a wire encoding yet
    onChanged();
  }
  
  void
  setContent(const Block& content) 
  { 
    content_ = content;
    onChanged();
  }

private:
  /**
   * Clear the wire encoding.
   */
  inline void 
  onChanged();

private:
  Name name_;
  MetaInfo metaInfo_;
  Block content_;
  Signature signature_;

  Block wire_;
};


inline void 
Data::onChanged()
{
  // The values have changed, so the signature and wire format is invalidated
  signature_.reset();
  wire_.reset();
}

} // namespace ndn

#endif

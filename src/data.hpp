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

#include "signature.hpp"
#include "meta-info.hpp"
#include "key-locator.hpp"

namespace ndn {
  
class Data {
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };
  
  /**
   * @brief Create an empty Data object
   */
  inline
  Data();
  
  /**
   * @brief Create a new Data object with the given name
   * @param name A reference to the name which is copied.
   */
  inline
  Data(const Name& name);
  
  /**
   * @brief The virtual destructor.
   */
  inline virtual
  ~Data();
  
  /**
   * @brief Encode this Data for a wire format.
   * @return The encoded byte array.
   */
  const Block& 
  wireEncode() const;
  
  /**
   * @brief Decode the input using a particular wire format and update this Data. 
   * @param input The input byte array to be decoded.
   */
  void
  wireDecode(const Block &wire);

  inline const Name& 
  getName() const;
  
  /**
   * @brief Set name to a copy of the given Name.
   *
   * @param name The Name which is copied.
   * @return This Data so that you can chain calls to update values.
   */
  inline void
  setName(const Name& name);

  inline const MetaInfo& 
  getMetaInfo() const;
  
  /**
   * @brief Set metaInfo to a copy of the given MetaInfo.
   * @param metaInfo The MetaInfo which is copied.
   * @return This Data so that you can chain calls to update values.
   */
  inline void
  setMetaInfo(const MetaInfo& metaInfo);

  ///////////////////////////////////////////////////////////////
  // MetaInfo proxy methods
  inline uint32_t 
  getContentType() const;
  
  inline void 
  setContentType(uint32_t type);
  
  inline Milliseconds 
  getFreshnessPeriod() const;
  
  inline void 
  setFreshnessPeriod(Milliseconds freshnessPeriod);
  
  /**
   * @brief Get content Block
   *
   * To access content value, one can use value()/value_size() or
   * value_begin()/value_end() methods of the Block class
   */
  inline const Block& 
  getContent() const;

  /**
   * @brief Set the content to a copy of the data in the vector.
   * @param content A vector whose contents are copied.
   * @return This Data so that you can chain calls to update values.
   */
  inline void
  setContent(const uint8_t* content, size_t contentLength);

  inline void
  setContent(const Block& content);

  inline void
  setContent(const ConstBufferPtr &contentValue);
  
  inline const Signature&
  getSignature() const;
  
  /**
   * @brief Set the signature to a copy of the given signature.
   * @param signature The signature object which is cloned.
   */
  inline void
  setSignature(const Signature& signature);

  inline void
  setSignatureValue(const Block &value);
  
private:
  /**
   * @brief Clear the wire encoding.
   */
  inline void 
  onChanged();

private:
  Name name_;
  MetaInfo metaInfo_;
  mutable Block content_;
  Signature signature_;

  mutable Block wire_;
};

inline
Data::Data()
  : content_(Tlv::Content) // empty content
{
}

inline
Data::Data(const Name& name)
  : name_(name)
{
}

inline
Data::~Data()
{
}

inline const Name& 
Data::getName() const
{
  return name_;
}
  
inline void
Data::setName(const Name& name)
{ 
  onChanged();
  name_ = name; 
}
  
inline const MetaInfo& 
Data::getMetaInfo() const
{
  return metaInfo_;
}
  
inline void
Data::setMetaInfo(const MetaInfo& metaInfo) 
{ 
  onChanged();
  metaInfo_ = metaInfo; 
}

inline uint32_t 
Data::getContentType() const
{
  return metaInfo_.getType();
}
  
inline void 
Data::setContentType(uint32_t type)
{
  onChanged();
  metaInfo_.setType(type);
}
  
inline Milliseconds 
Data::getFreshnessPeriod() const
{
  return metaInfo_.getFreshnessPeriod();
}
  
inline void 
Data::setFreshnessPeriod(Milliseconds freshnessPeriod)
{
  onChanged();
  metaInfo_.setFreshnessPeriod(freshnessPeriod);
}

inline const Block& 
Data::getContent() const
{
  if (!content_.hasWire())
      content_.encode();
  return content_;
}

inline void
Data::setContent(const uint8_t* content, size_t contentLength) 
{
  onChanged();

  content_ = dataBlock(Tlv::Content, content, contentLength);
}

inline void
Data::setContent(const ConstBufferPtr &contentValue)
{
  onChanged();

  content_ = Block(Tlv::Content, contentValue); // not real a wire encoding yet
}
  
inline void
Data::setContent(const Block& content) 
{
  onChanged();

  if (content.type() == Tlv::Content)
    content_ = content;
  else {
    content_ = Block(Tlv::Content, content);
  }
}

inline const Signature&
Data::getSignature() const
{
  return signature_;
}
  
inline void
Data::setSignature(const Signature& signature) 
{
  onChanged();
  signature_ = signature;
}

inline void
Data::setSignatureValue(const Block &value)
{
  onChanged();
  signature_.setValue(value);
}


inline void 
Data::onChanged()
{
  // The values have changed, so the wire format is invalidated

  // !!!Note!!! Signature is not invalidated and it is responsibility of
  // the application to do proper re-signing if necessary
  
  wire_.reset();
}

std::ostream&
operator << (std::ostream &os, const Data &data);

} // namespace ndn

#endif

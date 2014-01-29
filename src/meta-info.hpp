/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_META_INFO_HPP
#define NDN_META_INFO_HPP

namespace ndn {

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

  inline const Block& 
  wireEncode() const;
  
  inline void
  wireDecode(const Block &wire);  
  
private:
  uint32_t type_;
  Milliseconds freshnessPeriod_;

  mutable Block wire_;
};

inline const Block& 
MetaInfo::wireEncode() const
{
  if (wire_.hasWire())
    return wire_;

  // MetaInfo ::= META-INFO-TYPE TLV-LENGTH
  //                ContentType?
  //                FreshnessPeriod?
  
  wire_ = Block(Tlv::MetaInfo);

  // ContentType
  if (type_ != TYPE_DEFAULT) {
    wire_.push_back
      (nonNegativeIntegerBlock(Tlv::ContentType, type_));
  }

  // FreshnessPeriod
  if (freshnessPeriod_ >= 0) {
    wire_.push_back
      (nonNegativeIntegerBlock(Tlv::FreshnessPeriod, freshnessPeriod_));
  }
  
  wire_.encode();
  return wire_;  
}
  
inline void
MetaInfo::wireDecode(const Block &wire)
{
  wire_ = wire;
  wire_.parse();

  // MetaInfo ::= META-INFO-TYPE TLV-LENGTH
  //                ContentType?
  //                FreshnessPeriod?
  
  // ContentType
  Block::element_iterator val = wire_.find(Tlv::ContentType);
  if (val != wire_.getAll().end())
    {
      type_ = readNonNegativeInteger(*val);
    }

  // FreshnessPeriod
  val = wire_.find(Tlv::FreshnessPeriod);
  if (val != wire_.getAll().end())
    {
      freshnessPeriod_ = readNonNegativeInteger(*val);
    }
}

inline std::ostream&
operator << (std::ostream &os, const MetaInfo &info)
{
  // ContentType
  os << "ContentType: " << info.getType();

  // FreshnessPeriod
  if (info.getFreshnessPeriod() >= 0) {
    os << ", FreshnessPeriod: " << info.getFreshnessPeriod();
  }
  return os;
}

} // namespace ndn

#endif // NDN_META_INFO_HPP

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

  wire_ = Block(Tlv::MetaInfo);

  // ContentType
  if (type_ != TYPE_DEFAULT) {
    OBufferStream os;
    Tlv::writeVarNumber(os, Tlv::ContentType);
    Tlv::writeVarNumber(os, Tlv::sizeOfNonNegativeInteger(type_));
    Tlv::writeNonNegativeInteger(os, type_);

    wire_.push_back(Block(os.buf()));
  }

  // FreshnessPeriod
  if (freshnessPeriod_ >= 0) {
    OBufferStream os;
    Tlv::writeVarNumber(os, Tlv::FreshnessPeriod);
    Tlv::writeVarNumber(os, Tlv::sizeOfNonNegativeInteger(freshnessPeriod_));
    Tlv::writeNonNegativeInteger(os, freshnessPeriod_);

    wire_.push_back(Block(os.buf()));
  }
  
  wire_.encode();
  return wire_;  
}
  
inline void
MetaInfo::wireDecode(const Block &wire)
{
  wire_ = wire;
  wire_.parse();

  // ContentType
  Block::element_iterator val = wire_.find(Tlv::ContentType);
  if (val != wire_.getAll().end())
    {
      Buffer::const_iterator begin = val->value_begin();
      type_ = Tlv::readNonNegativeInteger(val->value_size(), begin, val->value_end());
    }

  // FreshnessPeriod
  val = wire_.find(Tlv::FreshnessPeriod);
  if (val != wire_.getAll().end())
    {
      Buffer::const_iterator begin = val->value_begin();
      freshnessPeriod_ = Tlv::readNonNegativeInteger(val->value_size(), begin, val->value_end());
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

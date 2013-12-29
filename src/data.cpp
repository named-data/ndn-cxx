/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/common.hpp>
#include <ndn-cpp/data.hpp>

using namespace std;

namespace ndn {

const Block& 
Data::wireEncode() const
{
  if (wire_.hasWire())
    return wire_;

  // Data ::= DATA-TLV TLV-LENGTH
  //            Name
  //            MetaInfo
  //            Content
  //            Signature
  
  wire_ = Block(Tlv::Data);

  // Name
  wire_.push_back(getName().wireEncode());

  // MetaInfo
  wire_.push_back(getMetaInfo().wireEncode());

  // Content
  wire_.push_back(getContent());

  if (!signature_) {
    throw Error("Requested wire format, but data packet has not been signed yet");
  }

  ///////////////
  // Signature //
  ///////////////
  
  // SignatureInfo
  wire_.push_back(signature_.getInfo());
  
  // SignatureValue
  wire_.push_back(signature_.getValue());
  
  wire_.encode();
  return wire_;  
}
  
/**
 * Decode the input using a particular wire format and update this Data. 
 * @param input The input byte array to be decoded.
 */
void
Data::wireDecode(const Block &wire)
{
  wire_ = wire;
  wire_.parse();

  // Data ::= DATA-TLV TLV-LENGTH
  //            Name
  //            MetaInfo
  //            Content
  //            Signature
    
  // Name
  name_.wireDecode(wire_.get(Tlv::Name));

  // MetaInfo
  metaInfo_.wireDecode(wire_.get(Tlv::MetaInfo));

  // Content
  content_ = wire_.get(Tlv::Content);

  ///////////////
  // Signature //
  ///////////////
  
  // SignatureInfo
  signature_.setInfo(wire_.get(Tlv::SignatureInfo));
  
  // SignatureValue
  signature_.setValue(wire_.get(Tlv::SignatureValue));
}

std::ostream&
operator << (std::ostream &os, const Data &data)
{
  os << "Name: " << data.getName() << "\n";
  os << "MetaInfo: " << data.getMetaInfo() << "\n";
  os << "Content: (size: " << data.getContent().value_size() << ")\n";
  os << "Signature: (type: " << data.getSignature().getType() <<
    ", value_length: "<< data.getSignature().getValue().value_size() << ")";
  os << std::endl;

  return os;
}


}

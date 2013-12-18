/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/common.hpp>
#include <ndn-cpp/data.hpp>
#include <ndn-cpp/sha256-with-rsa-signature.hpp>
#include "c/data.h"

using namespace std;

namespace ndn {

Signature::~Signature()
{
}
  
void 
MetaInfo::get(struct ndn_MetaInfo& metaInfoStruct) const 
{
  metaInfoStruct.timestampMilliseconds = timestampMilliseconds_;
  metaInfoStruct.type = type_;
  metaInfoStruct.freshnessSeconds = freshnessSeconds_;
  finalBlockID_.get(metaInfoStruct.finalBlockID);
}

void 
MetaInfo::set(const struct ndn_MetaInfo& metaInfoStruct)
{
  timestampMilliseconds_ = metaInfoStruct.timestampMilliseconds;
  type_ = metaInfoStruct.type;
  freshnessSeconds_ = metaInfoStruct.freshnessSeconds;
  finalBlockID_ = Name::Component(Blob(metaInfoStruct.finalBlockID.value));
}

Data::Data()
: signature_(new Sha256WithRsaSignature())
{
}
  
Data::Data(const Name& name)
: name_(name), signature_(new Sha256WithRsaSignature())
{
}

Data::Data(const Data& data)
: name_(data.name_), metaInfo_(data.metaInfo_), content_(data.content_), defaultWireEncoding_(data.defaultWireEncoding_)
{
  if (data.signature_)
    signature_ = data.signature_->clone();
}

Data::~Data()
{
}

Data& Data::operator=(const Data& data)
{
  if (data.signature_)
    signature_ = data.signature_->clone();
  else
    signature_ = ptr_lib::shared_ptr<Signature>();
  
  name_ = data.name_;
  metaInfo_ = data.metaInfo_;
  content_ = data.content_;
  defaultWireEncoding_ = data.defaultWireEncoding_;

  return *this;
}

void 
Data::get(struct ndn_Data& dataStruct) const 
{
  signature_->get(dataStruct.signature);
  name_.get(dataStruct.name);
  metaInfo_.get(dataStruct.metaInfo);
  content_.get(dataStruct.content);
}

void 
Data::set(const struct ndn_Data& dataStruct)
{
  signature_->set(dataStruct.signature);
  name_.set(dataStruct.name);
  metaInfo_.set(dataStruct.metaInfo);
  content_ = Blob(dataStruct.content);

  onChanged();
}

Data& 
Data::setName(const Name& name) 
{ 
  name_ = name; 
  onChanged();
  return *this;
}

SignedBlob 
Data::wireEncode(WireFormat& wireFormat) const
{
  size_t signedPortionBeginOffset, signedPortionEndOffset;
  Blob encoding = wireFormat.encodeData(*this, &signedPortionBeginOffset, &signedPortionEndOffset);
  SignedBlob wireEncoding = SignedBlob(encoding, signedPortionBeginOffset, signedPortionEndOffset);
  
  if (&wireFormat == WireFormat::getDefaultWireFormat())
    // This is the default wire encoding.
    const_cast<Data*>(this)->defaultWireEncoding_ = wireEncoding;
  
  return wireEncoding;
}

void 
Data::wireDecode(const uint8_t* input, size_t inputLength, WireFormat& wireFormat) 
{
  size_t signedPortionBeginOffset, signedPortionEndOffset;
  wireFormat.decodeData(*this, input, inputLength, &signedPortionBeginOffset, &signedPortionEndOffset);
  
  if (&wireFormat == WireFormat::getDefaultWireFormat())
    // This is the default wire encoding.
    defaultWireEncoding_ = SignedBlob(input, inputLength, signedPortionBeginOffset, signedPortionEndOffset);
  else
    defaultWireEncoding_ = SignedBlob();
}

void 
Data::onChanged()
{
  // The values have changed, so the default wire encoding is invalidated.
  defaultWireEncoding_ = SignedBlob();
}

}

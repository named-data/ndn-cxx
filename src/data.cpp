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

const Block &
Data::wireEncode() const
{
  // size_t signedPortionBeginOffset, signedPortionEndOffset;
  // Blob encoding = wireFormat.encodeData(*this, &signedPortionBeginOffset, &signedPortionEndOffset);
  // SignedBlob wireEncoding = SignedBlob(encoding, signedPortionBeginOffset, signedPortionEndOffset);
  
  // if (&wireFormat == WireFormat::getDefaultWireFormat())
  //   // This is the default wire encoding.
  //   const_cast<Data*>(this)->defaultWireEncoding_ = wireEncoding;
  
  // return wireEncoding;
  return wire_;
}

void
Data::wireDecode(const Block &wire)
{
}

void 
Data::wireDecode(const uint8_t* input, size_t inputLength) 
{
  // size_t signedPortionBeginOffset, signedPortionEndOffset;
  // wireFormat.decodeData(*this, input, inputLength, &signedPortionBeginOffset, &signedPortionEndOffset);
  
  // if (&wireFormat == WireFormat::getDefaultWireFormat())
  //   // This is the default wire encoding.
  //   defaultWireEncoding_ = SignedBlob(input, inputLength, signedPortionBeginOffset, signedPortionEndOffset);
  // else
  //   defaultWireEncoding_ = SignedBlob();
}

}

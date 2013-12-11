/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include <ndn-cpp/interest.hpp>
#include <ndn-cpp/data.hpp>
#include <ndn-cpp/forwarding-entry.hpp>
#include <ndn-cpp/encoding/binary-xml-wire-format.hpp>
#include "../c/encoding/binary-xml-interest.h"
#include "../c/encoding/binary-xml-data.h"
#include "../c/encoding/binary-xml-forwarding-entry.h"
#include "binary-xml-encoder.hpp"
#include "binary-xml-decoder.hpp"

using namespace std;

namespace ndn {

// This is declared in the WireFormat class.
WireFormat*
WireFormat::newInitialDefaultWireFormat() 
{
  return new BinaryXmlWireFormat();
}
  
Blob 
BinaryXmlWireFormat::encodeInterest(const Interest& interest) 
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ExcludeEntry excludeEntries[100];
  struct ndn_Interest interestStruct;
  ndn_Interest_initialize
    (&interestStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     excludeEntries, sizeof(excludeEntries) / sizeof(excludeEntries[0]));
  interest.get(interestStruct);

  BinaryXmlEncoder encoder;
  ndn_Error error;
  if ((error = ndn_encodeBinaryXmlInterest(&interestStruct, &encoder)))
    throw runtime_error(ndn_getErrorString(error));
     
  return encoder.getOutput();
}

void 
BinaryXmlWireFormat::decodeInterest(Interest& interest, const uint8_t *input, size_t inputLength)
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ExcludeEntry excludeEntries[100];
  struct ndn_Interest interestStruct;
  ndn_Interest_initialize
    (&interestStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     excludeEntries, sizeof(excludeEntries) / sizeof(excludeEntries[0]));
    
  BinaryXmlDecoder decoder(input, inputLength);  
  ndn_Error error;
  if ((error = ndn_decodeBinaryXmlInterest(&interestStruct, &decoder)))
    throw runtime_error(ndn_getErrorString(error));

  interest.set(interestStruct);
}

Blob 
BinaryXmlWireFormat::encodeData(const Data& data, size_t *signedPortionBeginOffset, size_t *signedPortionEndOffset) 
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_NameComponent keyNameComponents[100];
  struct ndn_Data dataStruct;
  ndn_Data_initialize
    (&dataStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     keyNameComponents, sizeof(keyNameComponents) / sizeof(keyNameComponents[0]));
  data.get(dataStruct);

  BinaryXmlEncoder encoder(1500);
  ndn_Error error;
  if ((error = ndn_encodeBinaryXmlData(&dataStruct, signedPortionBeginOffset, signedPortionEndOffset, &encoder)))
    throw runtime_error(ndn_getErrorString(error));
     
  return encoder.getOutput();
}

void 
BinaryXmlWireFormat::decodeData
  (Data& data, const uint8_t *input, size_t inputLength, size_t *signedPortionBeginOffset, size_t *signedPortionEndOffset)
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_NameComponent keyNameComponents[100];
  struct ndn_Data dataStruct;
  ndn_Data_initialize
    (&dataStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     keyNameComponents, sizeof(keyNameComponents) / sizeof(keyNameComponents[0]));
    
  BinaryXmlDecoder decoder(input, inputLength);  
  ndn_Error error;
  if ((error = ndn_decodeBinaryXmlData(&dataStruct, signedPortionBeginOffset, signedPortionEndOffset, &decoder)))
    throw runtime_error(ndn_getErrorString(error));

  data.set(dataStruct);
}

Blob 
BinaryXmlWireFormat::encodeForwardingEntry(const ForwardingEntry& forwardingEntry) 
{
  struct ndn_NameComponent prefixNameComponents[100];
  struct ndn_ForwardingEntry forwardingEntryStruct;
  ndn_ForwardingEntry_initialize
    (&forwardingEntryStruct, prefixNameComponents, sizeof(prefixNameComponents) / sizeof(prefixNameComponents[0]));
  forwardingEntry.get(forwardingEntryStruct);

  BinaryXmlEncoder encoder;
  ndn_Error error;
  if ((error = ndn_encodeBinaryXmlForwardingEntry(&forwardingEntryStruct, &encoder)))
    throw runtime_error(ndn_getErrorString(error));
     
  return encoder.getOutput();
}

void 
BinaryXmlWireFormat::decodeForwardingEntry(ForwardingEntry& forwardingEntry, const uint8_t *input, size_t inputLength)
{
  struct ndn_NameComponent prefixNameComponents[100];
  struct ndn_ForwardingEntry forwardingEntryStruct;
  ndn_ForwardingEntry_initialize
    (&forwardingEntryStruct, prefixNameComponents, sizeof(prefixNameComponents) / sizeof(prefixNameComponents[0]));
    
  BinaryXmlDecoder decoder(input, inputLength);  
  ndn_Error error;
  if ((error = ndn_decodeBinaryXmlForwardingEntry(&forwardingEntryStruct, &decoder)))
    throw runtime_error(ndn_getErrorString(error));

  forwardingEntry.set(forwardingEntryStruct);
}

}

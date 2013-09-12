/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "../c/encoding/binary-xml-interest.h"
#include "../c/encoding/binary-xml-data.h"
#include "../c/encoding/binary-xml-forwarding-entry.h"
#include "../interest.hpp"
#include "../data.hpp"
#include "../forwarding-entry.hpp"
#include "binary-xml-encoder.hpp"
#include "binary-xml-decoder.hpp"
#include "binary-xml-wire-format.hpp"

using namespace std;

namespace ndn {

// This is declared in the WireFormat class.
WireFormat *WireFormat::newInitialDefaultWireFormat() 
{
  return new BinaryXmlWireFormat();
}
  
Blob BinaryXmlWireFormat::encodeInterest(const Interest& interest) 
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
    throw std::runtime_error(ndn_getErrorString(error));
     
  return encoder.getOutput();
}

void BinaryXmlWireFormat::decodeInterest(Interest& interest, const unsigned char *input, unsigned int inputLength)
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
    throw std::runtime_error(ndn_getErrorString(error));

  interest.set(interestStruct);
}

Blob BinaryXmlWireFormat::encodeData(const Data& data, unsigned int *signedFieldsBeginOffset, unsigned int *signedFieldsEndOffset) 
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_NameComponent keyNameComponents[100];
  struct ndn_Data dataStruct;
  ndn_Data_initialize
    (&dataStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     keyNameComponents, sizeof(keyNameComponents) / sizeof(keyNameComponents[0]));
  data.get(dataStruct);

  BinaryXmlEncoder encoder;
  ndn_Error error;
  if ((error = ndn_encodeBinaryXmlData(&dataStruct, signedFieldsBeginOffset, signedFieldsEndOffset, &encoder)))
    throw std::runtime_error(ndn_getErrorString(error));
     
  return encoder.getOutput();
}

void BinaryXmlWireFormat::decodeData
  (Data& data, const unsigned char *input, unsigned int inputLength, unsigned int *signedFieldsBeginOffset, unsigned int *signedFieldsEndOffset)
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_NameComponent keyNameComponents[100];
  struct ndn_Data dataStruct;
  ndn_Data_initialize
    (&dataStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     keyNameComponents, sizeof(keyNameComponents) / sizeof(keyNameComponents[0]));
    
  BinaryXmlDecoder decoder(input, inputLength);  
  ndn_Error error;
  if ((error = ndn_decodeBinaryXmlData(&dataStruct, signedFieldsBeginOffset, signedFieldsEndOffset, &decoder)))
    throw std::runtime_error(ndn_getErrorString(error));

  data.set(dataStruct);
}

Blob BinaryXmlWireFormat::encodeForwardingEntry(const ForwardingEntry& forwardingEntry) 
{
  struct ndn_NameComponent prefixNameComponents[100];
  struct ndn_ForwardingEntry forwardingEntryStruct;
  ndn_ForwardingEntry_initialize
    (&forwardingEntryStruct, prefixNameComponents, sizeof(prefixNameComponents) / sizeof(prefixNameComponents[0]));
  forwardingEntry.get(forwardingEntryStruct);

  BinaryXmlEncoder encoder;
  ndn_Error error;
  if ((error = ndn_encodeBinaryXmlForwardingEntry(&forwardingEntryStruct, &encoder)))
    throw std::runtime_error(ndn_getErrorString(error));
     
  return encoder.getOutput();
}

void BinaryXmlWireFormat::decodeForwardingEntry(ForwardingEntry& forwardingEntry, const unsigned char *input, unsigned int inputLength)
{
  struct ndn_NameComponent prefixNameComponents[100];
  struct ndn_ForwardingEntry forwardingEntryStruct;
  ndn_ForwardingEntry_initialize
    (&forwardingEntryStruct, prefixNameComponents, sizeof(prefixNameComponents) / sizeof(prefixNameComponents[0]));
    
  BinaryXmlDecoder decoder(input, inputLength);  
  ndn_Error error;
  if ((error = ndn_decodeBinaryXmlForwardingEntry(&forwardingEntryStruct, &decoder)))
    throw std::runtime_error(ndn_getErrorString(error));

  forwardingEntry.set(forwardingEntryStruct);
}

}

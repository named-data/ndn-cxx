/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "../c/encoding/binary-xml-interest.h"
#include "../c/encoding/binary-xml-data.h"
#include "../interest.hpp"
#include "../data.hpp"
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
  
ptr_lib::shared_ptr<vector<unsigned char> > BinaryXmlWireFormat::encodeInterest(const Interest &interest) 
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ExcludeEntry excludeEntries[100];
  struct ndn_Interest interestStruct;
  ndn_Interest_init
    (&interestStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     excludeEntries, sizeof(excludeEntries) / sizeof(excludeEntries[0]));
  interest.get(interestStruct);

  BinaryXmlEncoder encoder;
  ndn_Error error;
  if ((error = ndn_encodeBinaryXmlInterest(&interestStruct, &encoder)))
    throw std::runtime_error(ndn_getErrorString(error));
     
  return encoder.getOutput();
}

void BinaryXmlWireFormat::decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength)
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ExcludeEntry excludeEntries[100];
  struct ndn_Interest interestStruct;
  ndn_Interest_init
    (&interestStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     excludeEntries, sizeof(excludeEntries) / sizeof(excludeEntries[0]));
    
  BinaryXmlDecoder decoder(input, inputLength);  
  ndn_Error error;
  if ((error = ndn_decodeBinaryXmlInterest(&interestStruct, &decoder)))
    throw std::runtime_error(ndn_getErrorString(error));

  interest.set(interestStruct);
}

ptr_lib::shared_ptr<vector<unsigned char> > BinaryXmlWireFormat::encodeData(const Data &data) 
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_Data dataStruct;
  ndn_Data_init
    (&dataStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]));
  data.get(dataStruct);

  BinaryXmlEncoder encoder;
  unsigned int dummyBeginOffset, dummyEndOffset;
  ndn_Error error;
  if ((error = ndn_encodeBinaryXmlData(&dataStruct, &dummyBeginOffset, &dummyEndOffset, &encoder)))
    throw std::runtime_error(ndn_getErrorString(error));
     
  return encoder.getOutput();
}

void BinaryXmlWireFormat::decodeData(Data &data, const unsigned char *input, unsigned int inputLength)
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_Data dataStruct;
  ndn_Data_init
    (&dataStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]));
    
  BinaryXmlDecoder decoder(input, inputLength);  
  unsigned int dummyBeginOffset, dummyEndOffset;
  ndn_Error error;
  if ((error = ndn_decodeBinaryXmlData(&dataStruct, &dummyBeginOffset, &dummyEndOffset, &decoder)))
    throw std::runtime_error(ndn_getErrorString(error));

  data.set(dataStruct);
}

}

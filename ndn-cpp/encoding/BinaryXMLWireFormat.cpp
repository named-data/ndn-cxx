/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "../c/encoding/BinaryXMLInterest.h"
#include "../c/encoding/BinaryXMLContentObject.h"
#include "../Interest.hpp"
#include "../ContentObject.hpp"
#include "BinaryXMLEncoder.hpp"
#include "BinaryXMLDecoder.hpp"
#include "BinaryXMLWireFormat.hpp"

using namespace std;

namespace ndn {

BinaryXMLWireFormat BinaryXMLWireFormat::instance_;

ptr_lib::shared_ptr<vector<unsigned char> > BinaryXMLWireFormat::encodeInterest(const Interest &interest) 
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ExcludeEntry excludeEntries[100];
  struct ndn_Interest interestStruct;
  ndn_Interest_init
    (&interestStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     excludeEntries, sizeof(excludeEntries) / sizeof(excludeEntries[0]));
  interest.get(interestStruct);

  BinaryXMLEncoder encoder;
  ndn_encodeBinaryXMLInterest(&interestStruct, &encoder);
     
  return encoder.getOutput();
}

void BinaryXMLWireFormat::decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength)
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ExcludeEntry excludeEntries[100];
  struct ndn_Interest interestStruct;
  ndn_Interest_init
    (&interestStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     excludeEntries, sizeof(excludeEntries) / sizeof(excludeEntries[0]));
    
  BinaryXMLDecoder decoder(input, inputLength);  
  ndn_Error error;
  if (error = ndn_decodeBinaryXMLInterest(&interestStruct, &decoder))
    throw std::runtime_error(ndn_getErrorString(error));

  interest.set(interestStruct);
}

ptr_lib::shared_ptr<vector<unsigned char> > BinaryXMLWireFormat::encodeContentObject(const ContentObject &contentObject) 
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ContentObject contentObjectStruct;
  ndn_ContentObject_init
    (&contentObjectStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]));
  contentObject.get(contentObjectStruct);

  BinaryXMLEncoder encoder;
  ndn_encodeBinaryXMLContentObject(&contentObjectStruct, &encoder);
     
  return encoder.getOutput();
}

void BinaryXMLWireFormat::decodeContentObject(ContentObject &contentObject, const unsigned char *input, unsigned int inputLength)
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ContentObject contentObjectStruct;
  ndn_ContentObject_init
    (&contentObjectStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]));
    
  BinaryXMLDecoder decoder(input, inputLength);  
  ndn_Error error;
  if (error = ndn_decodeBinaryXMLContentObject(&contentObjectStruct, &decoder))
    throw std::runtime_error(ndn_getErrorString(error));

  contentObject.set(contentObjectStruct);
}

}

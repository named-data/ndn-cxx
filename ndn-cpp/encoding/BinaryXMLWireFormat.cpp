/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "../c/encoding/BinaryXMLName.h"
#include "../c/encoding/BinaryXMLInterest.h"
#include "../c/encoding/BinaryXMLContentObject.h"
#include "../Interest.hpp"
#include "../ContentObject.hpp"
#include "BinaryXMLEncoder.hpp"
#include "../c/encoding/BinaryXMLDecoder.h"
#include "BinaryXMLWireFormat.hpp"

using namespace std;

namespace ndn {

BinaryXMLWireFormat BinaryXMLWireFormat::instance_;

void BinaryXMLWireFormat::encodeName(const Name &name, vector<unsigned char> &output) 
{
  struct ndn_Name nameStruct;
  struct ndn_NameComponent components[100];
  ndn_Name_init(&nameStruct, components, sizeof(components) / sizeof(components[0]));
  name.get(nameStruct);

  BinaryXMLEncoder encoder;
  ndn_encodeBinaryXMLName(&nameStruct, encoder.getEncoder());
     
  encoder.appendTo(output);
}

void BinaryXMLWireFormat::decodeName(Name &name, const unsigned char *input, unsigned int inputLength)
{
  struct ndn_NameComponent components[100];
  struct ndn_Name nameStruct;
  ndn_Name_init(&nameStruct, components, sizeof(components) / sizeof(components[0]));
    
  struct ndn_BinaryXMLDecoder decoder;
  ndn_BinaryXMLDecoder_init(&decoder, (unsigned char *)input, inputLength);
  
  ndn_Error error;
  if (error = ndn_decodeBinaryXMLName(&nameStruct, &decoder))
    throw std::runtime_error(ndn_getErrorString(error));

  name.set(nameStruct);
}

void BinaryXMLWireFormat::encodeInterest(const Interest &interest, vector<unsigned char> &output) 
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ExcludeEntry excludeEntries[100];
  struct ndn_Interest interestStruct;
  ndn_Interest_init
    (&interestStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     excludeEntries, sizeof(excludeEntries) / sizeof(excludeEntries[0]));
  interest.get(interestStruct);

  BinaryXMLEncoder encoder;
  ndn_encodeBinaryXMLInterest(&interestStruct, encoder.getEncoder());
     
  encoder.appendTo(output);
}

void BinaryXMLWireFormat::decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength)
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ExcludeEntry excludeEntries[100];
  struct ndn_Interest interestStruct;
  ndn_Interest_init
    (&interestStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     excludeEntries, sizeof(excludeEntries) / sizeof(excludeEntries[0]));
    
  struct ndn_BinaryXMLDecoder decoder;
  ndn_BinaryXMLDecoder_init(&decoder, (unsigned char *)input, inputLength);
  
  ndn_Error error;
  if (error = ndn_decodeBinaryXMLInterest(&interestStruct, &decoder))
    throw std::runtime_error(ndn_getErrorString(error));

  interest.set(interestStruct);
}

void BinaryXMLWireFormat::encodeContentObject(const ContentObject &contentObject, vector<unsigned char> &output) 
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ContentObject contentObjectStruct;
  ndn_ContentObject_init
    (&contentObjectStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]));
  contentObject.get(contentObjectStruct);

  BinaryXMLEncoder encoder;
  ndn_encodeBinaryXMLContentObject(&contentObjectStruct, encoder.getEncoder());
     
  encoder.appendTo(output);
}

void BinaryXMLWireFormat::decodeContentObject(ContentObject &contentObject, const unsigned char *input, unsigned int inputLength)
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ContentObject contentObjectStruct;
  ndn_ContentObject_init
    (&contentObjectStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]));
    
  struct ndn_BinaryXMLDecoder decoder;
  ndn_BinaryXMLDecoder_init(&decoder, (unsigned char *)input, inputLength);
  
  ndn_Error error;
  if (error = ndn_decodeBinaryXMLContentObject(&contentObjectStruct, &decoder))
    throw std::runtime_error(ndn_getErrorString(error));

  contentObject.set(contentObjectStruct);
}

}

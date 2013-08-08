/**
 * @author: Jeff Thompson
 * Derived from Name.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "binary-xml-encoder.h"
#include "binary-xml-decoder.h"
#include "binary-xml-name.h"

ndn_Error ndn_encodeBinaryXmlName(struct ndn_Name *name, struct ndn_BinaryXmlEncoder *encoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_Name)))
    return error;
  
  unsigned int i;
  for (i = 0; i < name->nComponents; ++i) {
    if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
        (encoder, ndn_BinaryXml_DTag_Component, name->components[i].value, name->components[i].valueLength)))
      return error;
	}
  
	if ((error = ndn_BinaryXmlEncoder_writeElementClose(encoder)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_decodeBinaryXmlName(struct ndn_Name *name, struct ndn_BinaryXmlDecoder *decoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(decoder, ndn_BinaryXml_DTag_Name)))
    return error;
    
  name->nComponents = 0;
  while (1) {
    int gotExpectedTag;
    if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_Component, &gotExpectedTag)))
      return error;
    
    if (!gotExpectedTag)
      // No more components.
      break;
    
    unsigned char *component;
    unsigned int componentLen;
    if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement(decoder, ndn_BinaryXml_DTag_Component, 0, &component, &componentLen)))
      return error;
    
    // Add the component to the name.
    if (name->nComponents >= name->maxComponents)
      return NDN_ERROR_read_a_component_past_the_maximum_number_of_components_allowed_in_the_name;
    ndn_NameComponent_init(name->components + name->nComponents, component, componentLen);
    ++name->nComponents;
  }
  
  if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
    return error;
  
  return NDN_ERROR_success;
}

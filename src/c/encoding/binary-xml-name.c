/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
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
  
  size_t i;
  for (i = 0; i < name->nComponents; ++i) {
    if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement(encoder, ndn_BinaryXml_DTag_Component, &name->components[i].value)))
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
    
    struct ndn_Blob component;
    if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement(decoder, ndn_BinaryXml_DTag_Component, 0, &component)))
      return error;
    if ((error = ndn_Name_appendBlob(name, &component)))
      return error;
  }
  
  if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
    return error;
  
  return NDN_ERROR_success;
}

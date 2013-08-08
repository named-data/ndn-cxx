/**
 * @author: Jeff Thompson
 * Derived from Key.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "binary-xml.h"
#include "binary-xml-structure-decoder.h"
#include "binary-xml-key.h"

ndn_Error ndn_encodeBinaryXmlKeyLocator(struct ndn_KeyLocator *keyLocator, struct ndn_BinaryXmlEncoder *encoder)
{
  if (keyLocator->type < 0)
    return 0;

  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_KeyLocator)))
    return error;

  if (keyLocator->type == ndn_KeyLocatorType_KEY) {
    if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
        (encoder, ndn_BinaryXml_DTag_Key, keyLocator->keyOrCertificate, keyLocator->keyOrCertificateLength)))
      return error;    
  }
  else if (keyLocator->type == ndn_KeyLocatorType_CERTIFICATE) {
    if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
        (encoder, ndn_BinaryXml_DTag_Certificate, keyLocator->keyOrCertificate, keyLocator->keyOrCertificateLength)))
      return error;    
  }
  else if (keyLocator->type == ndn_KeyLocatorType_KEYNAME) {
    // TODO: Implement keyName
  }
  else
    return NDN_ERROR_unrecognized_ndn_KeyLocatorType;
  
	if ((error = ndn_BinaryXmlEncoder_writeElementClose(encoder)))
    return error;
  
  return 0;
}

ndn_Error ndn_decodeBinaryXmlKeyLocator(struct ndn_KeyLocator *keyLocator, struct ndn_BinaryXmlDecoder *decoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(decoder, ndn_BinaryXml_DTag_KeyLocator)))
    return error;

  int gotExpectedTag;
  if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_Key, &gotExpectedTag)))
    return error;
  if (gotExpectedTag) {
    keyLocator->type = ndn_KeyLocatorType_KEY;
    
    if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement
        (decoder, ndn_BinaryXml_DTag_Key, 0, &keyLocator->keyOrCertificate, &keyLocator->keyOrCertificateLength)))
      return error;
  }
  else {
    if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_Certificate, &gotExpectedTag)))
      return error;
    if (gotExpectedTag) {
      keyLocator->type = ndn_KeyLocatorType_CERTIFICATE;
    
      if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement
          (decoder, ndn_BinaryXml_DTag_Certificate, 0, &keyLocator->keyOrCertificate, &keyLocator->keyOrCertificateLength)))
        return error;
    }
    else {
      if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_KeyName, &gotExpectedTag)))
        return error;
      if (gotExpectedTag) {
        // TODO: Implement keyName. For now, just use a structure decoder to skip it.
        struct ndn_BinaryXmlStructureDecoder structureDecoder;
        ndn_BinaryXmlStructureDecoder_init(&structureDecoder);
        
        ndn_BinaryXmlStructureDecoder_seek(&structureDecoder, decoder->offset);
        if ((error = ndn_BinaryXmlStructureDecoder_findElementEnd(&structureDecoder, decoder->input, decoder->inputLength)))
          return error;
        if (!structureDecoder.gotElementEnd)
          return NDN_ERROR_read_past_the_end_of_the_input;
        ndn_BinaryXmlDecoder_seek(decoder, structureDecoder.offset);        
      }
      else
        return NDN_ERROR_decodeBinaryXmlKeyLocator_unrecognized_key_locator_type;
    }
  }

  if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
    return error;
  
  return 0;
}

ndn_Error ndn_decodeOptionalBinaryXmlKeyLocator(struct ndn_KeyLocator *keyLocator, struct ndn_BinaryXmlDecoder *decoder)
{
  int gotExpectedTag;
  ndn_Error error; 
  if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_KeyLocator, &gotExpectedTag)))
    return error;
  if (gotExpectedTag) {
    if ((error = ndn_decodeBinaryXmlKeyLocator(keyLocator, decoder)))
      return error;
  }
  else
    ndn_KeyLocator_init(keyLocator);
  
  return 0;
}

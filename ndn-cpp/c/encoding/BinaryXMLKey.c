/**
 * @author: Jeff Thompson
 * Derived from Key.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "BinaryXML.h"
#include "BinaryXMLKey.h"

ndn_Error ndn_encodeBinaryXMLKeyLocator(struct ndn_KeyLocator *keyLocator, struct ndn_BinaryXMLEncoder *encoder)
{
  if (keyLocator->type < 0)
    return 0;

  ndn_Error error;
  if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(encoder, ndn_BinaryXML_DTag_KeyLocator))
    return error;

  if (keyLocator->type == ndn_KeyLocatorType_KEY) {
    if (error = ndn_BinaryXMLEncoder_writeBlobDTagElement
        (encoder, ndn_BinaryXML_DTag_Key, keyLocator->keyOrCertificate, keyLocator->keyOrCertificateLength))
      return error;    
  }
  else if (keyLocator->type == ndn_KeyLocatorType_CERTIFICATE) {
    if (error = ndn_BinaryXMLEncoder_writeBlobDTagElement
        (encoder, ndn_BinaryXML_DTag_Certificate, keyLocator->keyOrCertificate, keyLocator->keyOrCertificateLength))
      return error;    
  }
  else if (keyLocator->type == ndn_KeyLocatorType_KEYNAME) {
    // TODO: Implement keyName
  }
  else
    return NDN_ERROR_unrecognized_ndn_KeyLocatorType;
  
	if (error = ndn_BinaryXMLEncoder_writeElementClose(encoder))
    return error;
  
  return 0;
}

ndn_Error ndn_decodeBinaryXMLKeyLocator(struct ndn_KeyLocator *keyLocator, struct ndn_BinaryXMLDecoder *decoder)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLDecoder_readElementStartDTag(decoder, ndn_BinaryXML_DTag_KeyLocator))
    return error;

  int gotExpectedTag;
  if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_Key, &gotExpectedTag))
    return error;
  if (gotExpectedTag) {
    keyLocator->type = ndn_KeyLocatorType_KEY;
    
    if (error = ndn_BinaryXMLDecoder_readBinaryDTagElement
        (decoder, ndn_BinaryXML_DTag_Key, 0, &keyLocator->keyOrCertificate, &keyLocator->keyOrCertificateLength))
      return error;
  }
  else {
    if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_Certificate, &gotExpectedTag))
      return error;
    if (gotExpectedTag) {
      keyLocator->type = ndn_KeyLocatorType_CERTIFICATE;
    
      if (error = ndn_BinaryXMLDecoder_readBinaryDTagElement
          (decoder, ndn_BinaryXML_DTag_Certificate, 0, &keyLocator->keyOrCertificate, &keyLocator->keyOrCertificateLength))
        return error;
    }
    else {
      if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_KeyName, &gotExpectedTag))
        return error;
      if (gotExpectedTag) {
        // TODO: Implement keyName
      }
      else
        return NDN_ERROR_decodeBinaryXMLKeyLocator_unrecognized_key_locator_type;
    }
  }

  if (error = ndn_BinaryXMLDecoder_readElementClose(decoder))
    return error;
  
  return 0;
}

ndn_Error ndn_decodeOptionalBinaryXMLKeyLocator(struct ndn_KeyLocator *keyLocator, struct ndn_BinaryXMLDecoder *decoder)
{
  int gotExpectedTag;
  ndn_Error error; 
  if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_KeyLocator, &gotExpectedTag))
    return error;
  if (gotExpectedTag) {
    if (error = ndn_decodeBinaryXMLKeyLocator(keyLocator, decoder))
      return error;
  }
  else
    ndn_KeyLocator_init(keyLocator);
  
  return 0;
}

/**
 * @author: Jeff Thompson
 * Derived from Key.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "binary-xml.h"
#include "binary-xml-structure-decoder.h"
#include "binary-xml-key.h"

static ndn_Error decodeKeyNameData(struct ndn_KeyLocator *keyLocator, struct ndn_BinaryXmlDecoder *decoder)
{
  int gotExpectedTag;
  ndn_Error error; 
  if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_PublisherPublicKeyDigest, &gotExpectedTag)))
    return error;
  if (gotExpectedTag) {
    keyLocator->keyNameType = ndn_KeyNameType_PUBLISHER_PUBLIC_KEY_DIGEST;
    if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement
        (decoder, ndn_BinaryXml_DTag_PublisherPublicKeyDigest, 0, &keyLocator->keyData, &keyLocator->keyDataLength)))
      return error;
  }
  else {
    if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_PublisherCertificateDigest, &gotExpectedTag)))
      return error;
    if (gotExpectedTag) {
      keyLocator->keyNameType = ndn_KeyNameType_PUBLISHER_CERTIFICATE_DIGEST;
      if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement
          (decoder, ndn_BinaryXml_DTag_PublisherCertificateDigest, 0, &keyLocator->keyData, &keyLocator->keyDataLength)))
        return error;
    }
    else {
      if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_PublisherIssuerKeyDigest, &gotExpectedTag)))
        return error;
      if (gotExpectedTag) {
        keyLocator->keyNameType = ndn_KeyNameType_PUBLISHER_ISSUER_KEY_DIGEST;
        if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement
            (decoder, ndn_BinaryXml_DTag_PublisherIssuerKeyDigest, 0, &keyLocator->keyData, &keyLocator->keyDataLength)))
          return error;
      }
      else {
        if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_PublisherIssuerCertificateDigest, &gotExpectedTag)))
          return error;
        if (gotExpectedTag) {
          keyLocator->keyNameType = ndn_KeyNameType_PUBLISHER_ISSUER_CERTIFICATE_DIGEST;
          if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement
              (decoder, ndn_BinaryXml_DTag_PublisherIssuerCertificateDigest, 0, &keyLocator->keyData, &keyLocator->keyDataLength)))
            return error;
        }
        else
          return NDN_ERROR_decodeBinaryXmlKeyLocator_unrecognized_key_name_type;
      }
    }
  }
  
  return NDN_ERROR_success;
}

ndn_Error ndn_encodeBinaryXmlKeyLocator(struct ndn_KeyLocator *keyLocator, struct ndn_BinaryXmlEncoder *encoder)
{
  if (keyLocator->type < 0)
    return NDN_ERROR_success;

  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_KeyLocator)))
    return error;

  if (keyLocator->type == ndn_KeyLocatorType_KEY) {
    if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
        (encoder, ndn_BinaryXml_DTag_Key, keyLocator->keyData, keyLocator->keyDataLength)))
      return error;    
  }
  else if (keyLocator->type == ndn_KeyLocatorType_CERTIFICATE) {
    if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
        (encoder, ndn_BinaryXml_DTag_Certificate, keyLocator->keyData, keyLocator->keyDataLength)))
      return error;    
  }
  else if (keyLocator->type == ndn_KeyLocatorType_KEYNAME) {
    if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_KeyName)))
      return error;
    if ((error = ndn_encodeBinaryXmlName(&keyLocator->keyName, encoder)))
      return error;
    
    if (keyLocator->keyNameType == ndn_KeyNameType_PUBLISHER_PUBLIC_KEY_DIGEST) {
      if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
          (encoder, ndn_BinaryXml_DTag_PublisherPublicKeyDigest, keyLocator->keyData, keyLocator->keyDataLength)))
        return error;    
    }
    else if (keyLocator->keyNameType == ndn_KeyNameType_PUBLISHER_CERTIFICATE_DIGEST) {
      if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
          (encoder, ndn_BinaryXml_DTag_PublisherCertificateDigest, keyLocator->keyData, keyLocator->keyDataLength)))
        return error;    
    }
    else if (keyLocator->keyNameType == ndn_KeyNameType_PUBLISHER_ISSUER_KEY_DIGEST) {
      if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
          (encoder, ndn_BinaryXml_DTag_PublisherIssuerKeyDigest, keyLocator->keyData, keyLocator->keyDataLength)))
        return error;    
    }
    else if (keyLocator->keyNameType == ndn_KeyNameType_PUBLISHER_ISSUER_CERTIFICATE_DIGEST) {
      if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
          (encoder, ndn_BinaryXml_DTag_PublisherIssuerCertificateDigest, keyLocator->keyData, keyLocator->keyDataLength)))
        return error;    
    }
    else
      return NDN_ERROR_unrecognized_ndn_KeyNameType;

    if ((error = ndn_BinaryXmlEncoder_writeElementClose(encoder)))
      return error;
  }
  else
    return NDN_ERROR_unrecognized_ndn_KeyLocatorType;
  
  if ((error = ndn_BinaryXmlEncoder_writeElementClose(encoder)))
    return error;
  
  return NDN_ERROR_success;
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
        (decoder, ndn_BinaryXml_DTag_Key, 0, &keyLocator->keyData, &keyLocator->keyDataLength)))
      return error;
  }
  else {
    if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_Certificate, &gotExpectedTag)))
      return error;
    if (gotExpectedTag) {
      keyLocator->type = ndn_KeyLocatorType_CERTIFICATE;
    
      if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement
          (decoder, ndn_BinaryXml_DTag_Certificate, 0, &keyLocator->keyData, &keyLocator->keyDataLength)))
        return error;
    }
    else {
      if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_KeyName, &gotExpectedTag)))
        return error;
      if (gotExpectedTag) {
        keyLocator->type = ndn_KeyLocatorType_KEYNAME;
        
        if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(decoder, ndn_BinaryXml_DTag_KeyName)))
          return error;
        if ((error = ndn_decodeBinaryXmlName(&keyLocator->keyName, decoder)))
          return error;
        if ((error = decodeKeyNameData(keyLocator, decoder)))
          return error;        
        if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
          return error;
      }
      else
        return NDN_ERROR_decodeBinaryXmlKeyLocator_unrecognized_key_locator_type;
    }
  }

  if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
    return error;
  
  return NDN_ERROR_success;
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
    ndn_KeyLocator_init(keyLocator, keyLocator->keyName.components, keyLocator->keyName.maxComponents);
  
  return NDN_ERROR_success;
}

/**
 * @author: Jeff Thompson
 * Derived from ContentObject.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "BinaryXMLEncoder.h"
#include "BinaryXMLDecoder.h"
#include "BinaryXMLName.h"
#include "BinaryXMLPublisherPublicKeyDigest.h"
#include "BinaryXMLContentObject.h"

static ndn_Error encodeSignature(struct ndn_Signature *signature, struct ndn_BinaryXMLEncoder *encoder)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(encoder, ndn_BinaryXML_DTag_Signature))
    return error;
  
  // TODO: Check if digestAlgorithm is the same as the default, and skip it, otherwise encode it as UDATA.

  if (error = ndn_BinaryXMLEncoder_writeOptionalBlobDTagElement
      (encoder, ndn_BinaryXML_DTag_Witness, signature->witness, signature->witnessLength))
    return error;

  // Require a signature.
  if (error = ndn_BinaryXMLEncoder_writeBlobDTagElement
      (encoder, ndn_BinaryXML_DTag_SignatureBits, signature->signature, signature->signatureLength))
    return error;
  
	if (error = ndn_BinaryXMLEncoder_writeElementClose(encoder))
    return error;
  
  return 0;    
}

static ndn_Error decodeSignature(struct ndn_Signature *signature, struct ndn_BinaryXMLDecoder *decoder)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLDecoder_readElementStartDTag(decoder, ndn_BinaryXML_DTag_Signature))
    return error;
  
  /* TODO: digestAlgorithm as UDATA */ signature->digestAlgorithm = 0; signature->digestAlgorithmLength = 0;
  
  if (error = ndn_BinaryXMLDecoder_readOptionalBinaryDTagElement
      (decoder, ndn_BinaryXML_DTag_Witness, 0, &signature->witness, &signature->witnessLength))
    return error;
  
  // Require a signature.
  if (error = ndn_BinaryXMLDecoder_readBinaryDTagElement
      (decoder, ndn_BinaryXML_DTag_SignatureBits, 0, &signature->signature, &signature->signatureLength))
    return error;
  
  if (error = ndn_BinaryXMLDecoder_readElementClose(decoder))
    return error;
  
  return 0;
}

static ndn_Error encodeSignedInfo(struct ndn_SignedInfo *signedInfo, struct ndn_BinaryXMLEncoder *encoder)
{
  if (signedInfo->type < 0)
    return;

  ndn_Error error;
  if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(encoder, ndn_BinaryXML_DTag_SignedInfo))
    return error;

  // This will skip encoding if there is no publisherPublicKeyDigest.
  if (error = ndn_encodeBinaryXMLPublisherPublicKeyDigest(&signedInfo->publisherPublicKeyDigest, encoder))
    return error;
  
  // TODO: Implement timeStamp  
  
  if (signedInfo->type != ndn_ContentType_DATA) {
    // Not the default of DATA, so we need to encode the type.
    // TODO: Implement converting the type from an int and encoding.
  }
  
  if (error = ndn_BinaryXMLEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXML_DTag_FreshnessSeconds, signedInfo->freshnessSeconds))
    return error;
  
  if (error = ndn_BinaryXMLEncoder_writeOptionalBlobDTagElement
      (encoder, ndn_BinaryXML_DTag_FinalBlockID, signedInfo->finalBlockID, signedInfo->finalBlockIDLength))
    return error;
 
  // This will skip encoding if there is no key locator.
  if (error = ndn_encodeBinaryXMLKeyLocator(&signedInfo->keyLocator, encoder))
    return error;
  
	if (error = ndn_BinaryXMLEncoder_writeElementClose(encoder))
    return error;
  
  return 0;  
}

static ndn_Error decodeSignedInfo(struct ndn_SignedInfo *signedInfo, struct ndn_BinaryXMLDecoder *decoder)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLDecoder_readElementStartDTag(decoder, ndn_BinaryXML_DTag_SignedInfo))
    return error;
  
  if (error = ndn_decodeOptionalBinaryXMLPublisherPublicKeyDigest(&signedInfo->publisherPublicKeyDigest, decoder))
    return error;
  
  if (error= ndn_BinaryXMLDecoder_readOptionalTimeMillisecondsDTagElement
      (decoder, ndn_BinaryXML_DTag_Timestamp, &signedInfo->timestampMilliseconds))
    return error;
  
  // TODO: Implement reading the type and converting to an int.
  signedInfo->type = ndn_ContentType_DATA;
 
  if (error = ndn_BinaryXMLDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXML_DTag_FreshnessSeconds, &signedInfo->freshnessSeconds))
    return error;

  if (error = ndn_BinaryXMLDecoder_readOptionalBinaryDTagElement
      (decoder, ndn_BinaryXML_DTag_FinalBlockID, 0, &signedInfo->finalBlockID, &signedInfo->finalBlockIDLength))
    return error;

  if (error = ndn_decodeOptionalBinaryXMLKeyLocator(&signedInfo->keyLocator, decoder))
    return error;
  
  if (error = ndn_BinaryXMLDecoder_readElementClose(decoder))
    return error;
  
  return 0;
}

ndn_Error ndn_encodeBinaryXMLContentObject(struct ndn_ContentObject *contentObject, struct ndn_BinaryXMLEncoder *encoder)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(encoder, ndn_BinaryXML_DTag_ContentObject))
    return error;
  
  if (error = encodeSignature(&contentObject->signature, encoder))
    return 0;

  if (error = ndn_encodeBinaryXMLName(&contentObject->name, encoder))
    return error;

  if (error = encodeSignedInfo(&contentObject->signedInfo, encoder))
    return 0;

  if (error = ndn_BinaryXMLEncoder_writeBlobDTagElement
      (encoder, ndn_BinaryXML_DTag_Content, contentObject->content, contentObject->contentLength))
    return error;
  
	if (error = ndn_BinaryXMLEncoder_writeElementClose(encoder))
    return error;
  
  return 0;
}

ndn_Error ndn_decodeBinaryXMLContentObject(struct ndn_ContentObject *contentObject, struct ndn_BinaryXMLDecoder *decoder)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLDecoder_readElementStartDTag(decoder, ndn_BinaryXML_DTag_ContentObject))
    return error;

  int gotExpectedTag;
  if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_Signature, &gotExpectedTag))
    return error;
  if (gotExpectedTag) {
    if (error = decodeSignature(&contentObject->signature, decoder))
      return error;
  }
  else
    ndn_Signature_init(&contentObject->signature);
  
  if (error = ndn_decodeBinaryXMLName(&contentObject->name, decoder))
    return error;
  
  if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_SignedInfo, &gotExpectedTag))
    return error;
  if (gotExpectedTag) {
    if (error = decodeSignedInfo(&contentObject->signedInfo, decoder))
      return error;
  }
  else
    ndn_SignedInfo_init(&contentObject->signedInfo);

  // Require a Content element, but set allowNull to allow a missing BLOB.
  if (error = ndn_BinaryXMLDecoder_readBinaryDTagElement
      (decoder, ndn_BinaryXML_DTag_Content, 1, &contentObject->content, &contentObject->contentLength))
    return error; 
  
  if (error = ndn_BinaryXMLDecoder_readElementClose(decoder))
    return error;
  
  return 0;
}

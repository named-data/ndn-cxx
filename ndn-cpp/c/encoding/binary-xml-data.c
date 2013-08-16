/**
 * @author: Jeff Thompson
 * Derived from ContentObject.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "binary-xml-encoder.h"
#include "binary-xml-decoder.h"
#include "binary-xml-name.h"
#include "binary-xml-publisher-public-key-digest.h"
#include "binary-xml-data.h"

static ndn_Error encodeSignature(struct ndn_Signature *signature, struct ndn_BinaryXmlEncoder *encoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_Signature)))
    return error;
  
  // TODO: Check if digestAlgorithm is the same as the default, and skip it, otherwise encode it as UDATA.

  if ((error = ndn_BinaryXmlEncoder_writeOptionalBlobDTagElement
      (encoder, ndn_BinaryXml_DTag_Witness, signature->witness, signature->witnessLength)))
    return error;

  // Require a signature.
  if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
      (encoder, ndn_BinaryXml_DTag_SignatureBits, signature->signature, signature->signatureLength)))
    return error;
  
  if ((error = ndn_BinaryXmlEncoder_writeElementClose(encoder)))
    return error;
  
  return NDN_ERROR_success;    
}

static ndn_Error decodeSignature(struct ndn_Signature *signature, struct ndn_BinaryXmlDecoder *decoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(decoder, ndn_BinaryXml_DTag_Signature)))
    return error;
  
  /* TODO: digestAlgorithm as UDATA */ signature->digestAlgorithm = 0; signature->digestAlgorithmLength = 0;
  
  if ((error = ndn_BinaryXmlDecoder_readOptionalBinaryDTagElement
      (decoder, ndn_BinaryXml_DTag_Witness, 0, &signature->witness, &signature->witnessLength)))
    return error;
  
  // Require a signature.
  if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement
      (decoder, ndn_BinaryXml_DTag_SignatureBits, 0, &signature->signature, &signature->signatureLength)))
    return error;
  
  if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
    return error;
  
  return NDN_ERROR_success;
}

static ndn_Error encodeSignedInfo(struct ndn_SignedInfo *signedInfo, struct ndn_BinaryXmlEncoder *encoder)
{
  if (signedInfo->type < 0)
    return NDN_ERROR_success;

  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_SignedInfo)))
    return error;

  // This will skip encoding if there is no publisherPublicKeyDigest.
  if ((error = ndn_encodeBinaryXmlPublisherPublicKeyDigest(&signedInfo->publisherPublicKeyDigest, encoder)))
    return error;
  
  if ((error = ndn_BinaryXmlEncoder_writeOptionalTimeMillisecondsDTagElement
      (encoder, ndn_BinaryXml_DTag_Timestamp, signedInfo->timestampMilliseconds)))
    return error;
  
  if (signedInfo->type != ndn_ContentType_DATA) {
    // Not the default of DATA, so we need to encode the type.
    // TODO: Implement converting the type from an int and encoding.
  }
  
  if ((error = ndn_BinaryXmlEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXml_DTag_FreshnessSeconds, signedInfo->freshnessSeconds)))
    return error;
  
  if ((error = ndn_BinaryXmlEncoder_writeOptionalBlobDTagElement
      (encoder, ndn_BinaryXml_DTag_FinalBlockID, signedInfo->finalBlockID, signedInfo->finalBlockIDLength)))
    return error;
 
  // This will skip encoding if there is no key locator.
  if ((error = ndn_encodeBinaryXmlKeyLocator(&signedInfo->keyLocator, encoder)))
    return error;
  
  if ((error = ndn_BinaryXmlEncoder_writeElementClose(encoder)))
    return error;
  
  return NDN_ERROR_success;  
}

static ndn_Error decodeSignedInfo(struct ndn_SignedInfo *signedInfo, struct ndn_BinaryXmlDecoder *decoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(decoder, ndn_BinaryXml_DTag_SignedInfo)))
    return error;
  
  if ((error = ndn_decodeOptionalBinaryXmlPublisherPublicKeyDigest(&signedInfo->publisherPublicKeyDigest, decoder)))
    return error;
  
  if (error= ndn_BinaryXmlDecoder_readOptionalTimeMillisecondsDTagElement
      (decoder, ndn_BinaryXml_DTag_Timestamp, &signedInfo->timestampMilliseconds))
    return error;
  
  // TODO: Implement reading the type and converting to an int.
  signedInfo->type = ndn_ContentType_DATA;
 
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_FreshnessSeconds, &signedInfo->freshnessSeconds)))
    return error;

  if ((error = ndn_BinaryXmlDecoder_readOptionalBinaryDTagElement
      (decoder, ndn_BinaryXml_DTag_FinalBlockID, 0, &signedInfo->finalBlockID, &signedInfo->finalBlockIDLength)))
    return error;

  if ((error = ndn_decodeOptionalBinaryXmlKeyLocator(&signedInfo->keyLocator, decoder)))
    return error;
  
  if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_encodeBinaryXmlData
  (struct ndn_Data *data, unsigned int *signedFieldsBeginOffset, unsigned int *signedFieldsEndOffset, struct ndn_BinaryXmlEncoder *encoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_ContentObject)))
    return error;
  
  if ((error = encodeSignature(&data->signature, encoder)))
    return NDN_ERROR_success;
  
  *signedFieldsBeginOffset = encoder->offset;

  if ((error = ndn_encodeBinaryXmlName(&data->name, encoder)))
    return error;
  
  if ((error = encodeSignedInfo(&data->signedInfo, encoder)))
    return NDN_ERROR_success;

  if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
      (encoder, ndn_BinaryXml_DTag_Content, data->content, data->contentLength)))
    return error;

  *signedFieldsEndOffset = encoder->offset;

  if ((error = ndn_BinaryXmlEncoder_writeElementClose(encoder)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_decodeBinaryXmlData
  (struct ndn_Data *data, unsigned int *signedFieldsBeginOffset, unsigned int *signedFieldsEndOffset, struct ndn_BinaryXmlDecoder *decoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(decoder, ndn_BinaryXml_DTag_ContentObject)))
    return error;

  int gotExpectedTag;
  if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_Signature, &gotExpectedTag)))
    return error;
  if (gotExpectedTag) {
    if ((error = decodeSignature(&data->signature, decoder)))
      return error;
  }
  else
    ndn_Signature_init(&data->signature);
  
  *signedFieldsBeginOffset = decoder->offset;
  
  if ((error = ndn_decodeBinaryXmlName(&data->name, decoder)))
    return error;
  
  if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_SignedInfo, &gotExpectedTag)))
    return error;
  if (gotExpectedTag) {
    if ((error = decodeSignedInfo(&data->signedInfo, decoder)))
      return error;
  }
  else
    ndn_SignedInfo_init(&data->signedInfo, data->signedInfo.keyLocator.keyName.components, data->signedInfo.keyLocator.keyName.maxComponents);

  // Require a Content element, but set allowNull to allow a missing BLOB.
  if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement
      (decoder, ndn_BinaryXml_DTag_Content, 1, &data->content, &data->contentLength)))
    return error; 
  
  *signedFieldsEndOffset = decoder->offset;

  if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
    return error;
  
  return NDN_ERROR_success;
}

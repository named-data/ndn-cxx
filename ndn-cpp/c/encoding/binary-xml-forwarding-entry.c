/**
 * @author: Jeff Thompson
 * Derived from ForwardingEntry.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "binary-xml.h"
#include "binary-xml-forwarding-entry.h"

ndn_Error ndn_encodeBinaryXmlForwardingEntry(struct ndn_ForwardingEntry *forwardingEntry, struct ndn_BinaryXmlEncoder *encoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_ForwardingEntry)))
    return error;
    
  if ((error = ndn_BinaryXmlEncoder_writeOptionalUDataDTagElement
      (encoder, ndn_BinaryXml_DTag_Action, forwardingEntry->action, forwardingEntry->actionLength)))
    return error;
  if ((error = ndn_encodeBinaryXmlName(&forwardingEntry->prefix, encoder)))
    return error;
  // This will skip encoding if there is no publisherPublicKeyDigest.
  if ((error = ndn_encodeBinaryXmlPublisherPublicKeyDigest(&forwardingEntry->publisherPublicKeyDigest, encoder)))
    return error;
  if ((error = ndn_BinaryXmlEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXml_DTag_FaceID, forwardingEntry->faceId)))
    return error;
  if ((error = ndn_BinaryXmlEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXml_DTag_ForwardingFlags, forwardingEntry->forwardingFlags)))
    return error;
  if ((error = ndn_BinaryXmlEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXml_DTag_FreshnessSeconds, forwardingEntry->freshnessSeconds)))
    return error;
  
  if ((error = ndn_BinaryXmlEncoder_writeElementClose(encoder)))
    return error;
  
  return NDN_ERROR_success;  
}

ndn_Error ndn_decodeBinaryXmlForwardingEntry(struct ndn_ForwardingEntry *forwardingEntry, struct ndn_BinaryXmlDecoder *decoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(decoder, ndn_BinaryXml_DTag_ForwardingEntry)))
    return error;
    
  if ((error = ndn_BinaryXmlDecoder_readOptionalUDataDTagElement
      (decoder, ndn_BinaryXml_DTag_Action, &forwardingEntry->action, &forwardingEntry->actionLength)))
    return error;
  if ((error = ndn_decodeBinaryXmlName(&forwardingEntry->prefix, decoder)))
    return error;
  if ((error = ndn_decodeOptionalBinaryXmlPublisherPublicKeyDigest(&forwardingEntry->publisherPublicKeyDigest, decoder)))
    return error;
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_FaceID, &forwardingEntry->faceId)))
    return error;
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_ForwardingFlags, &forwardingEntry->forwardingFlags)))
    return error;
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_FreshnessSeconds, &forwardingEntry->freshnessSeconds)))
    return error;

  if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
    return error;
  
  return NDN_ERROR_success;
}

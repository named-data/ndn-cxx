/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * Derived from ForwardingEntry.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "binary-xml.h"
#include "binary-xml-forwarding-entry.h"
#include "binary-xml-name.h"
#include "binary-xml-key.h"
#include "binary-xml-publisher-public-key-digest.h"

ndn_Error ndn_encodeBinaryXmlForwardingEntry(struct ndn_ForwardingEntry *forwardingEntry, struct ndn_BinaryXmlEncoder *encoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_ForwardingEntry)))
    return error;
    
  if ((error = ndn_BinaryXmlEncoder_writeOptionalUDataDTagElement
      (encoder, ndn_BinaryXml_DTag_Action, &forwardingEntry->action)))
    return error;
  if ((error = ndn_encodeBinaryXmlName(&forwardingEntry->prefix, encoder)))
    return error;
  // This will skip encoding if there is no publisherPublicKeyDigest.
  if ((error = ndn_encodeBinaryXmlPublisherPublicKeyDigest(&forwardingEntry->publisherPublicKeyDigest, encoder)))
    return error;
  if ((error = ndn_BinaryXmlEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXml_DTag_FaceID, forwardingEntry->faceId)))
    return error;
  if ((error = ndn_BinaryXmlEncoder_writeUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXml_DTag_ForwardingFlags, 
       ndn_ForwardingFlags_getForwardingEntryFlags(&forwardingEntry->forwardingFlags))))
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
      (decoder, ndn_BinaryXml_DTag_Action, &forwardingEntry->action)))
    return error;
  if ((error = ndn_decodeBinaryXmlName(&forwardingEntry->prefix, decoder)))
    return error;
  if ((error = ndn_decodeOptionalBinaryXmlPublisherPublicKeyDigest(&forwardingEntry->publisherPublicKeyDigest, decoder)))
    return error;
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_FaceID, &forwardingEntry->faceId)))
    return error;
  
  int forwardingEntryFlags;
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_ForwardingFlags, &forwardingEntryFlags)))
    return error;
  if (forwardingEntryFlags >= 0)
    ndn_ForwardingFlags_setForwardingEntryFlags(&forwardingEntry->forwardingFlags, forwardingEntryFlags);
  else
    // This sets the default flags.
    ndn_ForwardingFlags_initialize(&forwardingEntry->forwardingFlags);
  
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_FreshnessSeconds, &forwardingEntry->freshnessSeconds)))
    return error;

  if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
    return error;
  
  return NDN_ERROR_success;
}

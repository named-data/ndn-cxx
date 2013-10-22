/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * Derived from PublisherPublicKeyDigest.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "binary-xml.h"
#include "binary-xml-publisher-public-key-digest.h"

ndn_Error ndn_encodeBinaryXmlPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXmlEncoder *encoder)
{  
  if (!publisherPublicKeyDigest->publisherPublicKeyDigest.value || publisherPublicKeyDigest->publisherPublicKeyDigest.length == 0)
    return NDN_ERROR_success;
  
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
      (encoder, ndn_BinaryXml_DTag_PublisherPublicKeyDigest, &publisherPublicKeyDigest->publisherPublicKeyDigest)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_decodeBinaryXmlPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXmlDecoder *decoder)
{
  ndn_Error error; 
  if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement
      (decoder, ndn_BinaryXml_DTag_PublisherPublicKeyDigest, 0, &publisherPublicKeyDigest->publisherPublicKeyDigest)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_decodeOptionalBinaryXmlPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXmlDecoder *decoder)
{
  int gotExpectedTag;
  ndn_Error error; 
  if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_PublisherPublicKeyDigest, &gotExpectedTag)))
    return error;
  if (gotExpectedTag) {
    if ((error = ndn_decodeBinaryXmlPublisherPublicKeyDigest(publisherPublicKeyDigest, decoder)))
      return error;
  }
  else {
    publisherPublicKeyDigest->publisherPublicKeyDigest.value = 0;
    publisherPublicKeyDigest->publisherPublicKeyDigest.length = 0;
  }
  
  return NDN_ERROR_success;
}

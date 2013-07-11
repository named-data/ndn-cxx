/**
 * @author: Jeff Thompson
 * Derived from PublisherPublicKeyDigest.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "BinaryXML.h"
#include "BinaryXMLPublisherPublicKeyDigest.h"

ndn_Error ndn_encodeBinaryXMLPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXMLEncoder *encoder)
{  
  if (!publisherPublicKeyDigest->publisherPublicKeyDigest || publisherPublicKeyDigest->publisherPublicKeyDigestLength == 0)
    return;
  
  ndn_Error error;
  if (error = ndn_BinaryXMLEncoder_writeBlobDTagElement
      (encoder, ndn_BinaryXML_DTag_PublisherPublicKeyDigest, publisherPublicKeyDigest->publisherPublicKeyDigest, 
       publisherPublicKeyDigest->publisherPublicKeyDigestLength))
    return error;
  
  return 0;
}

ndn_Error ndn_decodeBinaryXMLPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXMLDecoder *decoder)
{
  ndn_Error error; 
  if (error = ndn_BinaryXMLDecoder_readBinaryDTagElement
      (decoder, ndn_BinaryXML_DTag_PublisherPublicKeyDigest, 0, &publisherPublicKeyDigest->publisherPublicKeyDigest,
       &publisherPublicKeyDigest->publisherPublicKeyDigestLength))
    return error;
  
  return 0;
}

ndn_Error ndn_decodeOptionalBinaryXMLPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXMLDecoder *decoder)
{
  int gotExpectedTag;
  ndn_Error error; 
  if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_PublisherPublicKeyDigest, &gotExpectedTag))
    return error;
  if (gotExpectedTag) {
    if (error = ndn_decodeBinaryXMLPublisherPublicKeyDigest(publisherPublicKeyDigest, decoder))
      return error;
  }
  else {
    publisherPublicKeyDigest->publisherPublicKeyDigest = 0;
    publisherPublicKeyDigest->publisherPublicKeyDigestLength = 0;
  }
  
  return 0;
}

/**
 * @author: Jeff Thompson
 * Derived from PublisherPublicKeyDigest.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "BinaryXML.h"
#include "BinaryXMLPublisherPublicKeyDigest.h"

ndn_Error ndn_encodeBinaryXmlPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXmlEncoder *encoder)
{  
  if (!publisherPublicKeyDigest->publisherPublicKeyDigest || publisherPublicKeyDigest->publisherPublicKeyDigestLength == 0)
    return;
  
  ndn_Error error;
  if (error = ndn_BinaryXmlEncoder_writeBlobDTagElement
      (encoder, ndn_BinaryXml_DTag_PublisherPublicKeyDigest, publisherPublicKeyDigest->publisherPublicKeyDigest, 
       publisherPublicKeyDigest->publisherPublicKeyDigestLength))
    return error;
  
  return 0;
}

ndn_Error ndn_decodeBinaryXmlPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXmlDecoder *decoder)
{
  ndn_Error error; 
  if (error = ndn_BinaryXmlDecoder_readBinaryDTagElement
      (decoder, ndn_BinaryXml_DTag_PublisherPublicKeyDigest, 0, &publisherPublicKeyDigest->publisherPublicKeyDigest,
       &publisherPublicKeyDigest->publisherPublicKeyDigestLength))
    return error;
  
  return 0;
}

ndn_Error ndn_decodeOptionalBinaryXmlPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXmlDecoder *decoder)
{
  int gotExpectedTag;
  ndn_Error error; 
  if (error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_PublisherPublicKeyDigest, &gotExpectedTag))
    return error;
  if (gotExpectedTag) {
    if (error = ndn_decodeBinaryXmlPublisherPublicKeyDigest(publisherPublicKeyDigest, decoder))
      return error;
  }
  else {
    publisherPublicKeyDigest->publisherPublicKeyDigest = 0;
    publisherPublicKeyDigest->publisherPublicKeyDigestLength = 0;
  }
  
  return 0;
}

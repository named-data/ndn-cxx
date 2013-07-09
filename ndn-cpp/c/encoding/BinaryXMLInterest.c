/**
 * @author: Jeff Thompson
 * Derived from Interest.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "BinaryXMLEncoder.h"
#include "BinaryXMLDecoder.h"
#include "BinaryXMLName.h"
#include "BinaryXMLInterest.h"

ndn_Error ndn_encodeBinaryXMLInterest(struct ndn_Interest *interest, struct ndn_BinaryXMLEncoder *encoder)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(encoder, ndn_BinaryXML_DTag_Interest))
    return error;
    
  if (error = ndn_encodeBinaryXMLName(&interest->name, encoder))
    return error;
  
  if (interest->minSuffixComponents >= 0) {
    if (error = ndn_BinaryXMLEncoder_writeUnsignedDecimalIntDTagElement
        (encoder, ndn_BinaryXML_DTag_MinSuffixComponents, (unsigned int)interest->minSuffixComponents))
      return error;
  }
  if (interest->maxSuffixComponents >= 0) {
    if (error = ndn_BinaryXMLEncoder_writeUnsignedDecimalIntDTagElement
        (encoder, ndn_BinaryXML_DTag_MaxSuffixComponents, (unsigned int)interest->maxSuffixComponents))
      return error;
  }
    
  if (interest->publisherPublicKeyDigest && interest->publisherPublicKeyDigestLength > 0) {
    if (error = ndn_BinaryXMLEncoder_writeBlobDTagElement
        (encoder, ndn_BinaryXML_DTag_PublisherPublicKeyDigest, interest->publisherPublicKeyDigest, interest->publisherPublicKeyDigestLength))
      return error;
  }
  
  // TODO: Implement exclude

  if (interest->childSelector >= 0) {
    if (error = ndn_BinaryXMLEncoder_writeUnsignedDecimalIntDTagElement
        (encoder, ndn_BinaryXML_DTag_ChildSelector, (unsigned int)interest->childSelector))
      return error;
  }
  if (interest->answerOriginKind >= 0 && interest->answerOriginKind != ndn_Interest_DEFAULT_ANSWER_ORIGIN_KIND) {
    if (error = ndn_BinaryXMLEncoder_writeUnsignedDecimalIntDTagElement
        (encoder, ndn_BinaryXML_DTag_AnswerOriginKind, (unsigned int)interest->answerOriginKind))
      return error;
  }
  if (interest->scope >= 0) {
    if (error = ndn_BinaryXMLEncoder_writeUnsignedDecimalIntDTagElement
        (encoder, ndn_BinaryXML_DTag_Scope, (unsigned int)interest->scope))
      return error;
  }
  
  if (interest->interestLifetime >= 0) {
    if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(encoder, ndn_BinaryXML_DTag_InterestLifetime))
      return error;
   
    unsigned int ticks = (unsigned int)(((double)interest->interestLifetime / 1000.0) * 4096.0);
    if (error = ndn_BinaryXMLEncoder_writeUnsignedIntBigEndianBlob(encoder, ticks))
      return error;
    
    if (error = ndn_BinaryXMLEncoder_writeElementClose(encoder))
      return error;
  }
  
  if (interest->nonce && interest->nonceLength > 0) {
    if (error = ndn_BinaryXMLEncoder_writeBlobDTagElement
        (encoder, ndn_BinaryXML_DTag_Nonce, interest->nonce, interest->nonceLength))
      return error;
  }
  
	if (error = ndn_BinaryXMLEncoder_writeElementClose(encoder))
    return error;
  
  return 0;  
}

ndn_Error ndn_decodeBinaryXMLInterest(struct ndn_Interest *interest, struct ndn_BinaryXMLDecoder *decoder)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLDecoder_readElementStartDTag(decoder, ndn_BinaryXML_DTag_Interest))
    return error;
    
  if (error = ndn_decodeBinaryXMLName(&interest->name, decoder))
    return error;
       
  if (error = ndn_BinaryXMLDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXML_DTag_MinSuffixComponents, &interest->minSuffixComponents))
    return error;
  if (error = ndn_BinaryXMLDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXML_DTag_MaxSuffixComponents, &interest->maxSuffixComponents))
    return error;
  
  int gotExpectedTag;
  if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_PublisherPublicKeyDigest, &gotExpectedTag))
    return error;
  if (gotExpectedTag) {
    if (error = ndn_BinaryXMLDecoder_readBinaryDTagElement
        (decoder, ndn_BinaryXML_DTag_PublisherPublicKeyDigest, 0, &interest->publisherPublicKeyDigest,
         &interest->publisherPublicKeyDigestLength))
      return error;
  }
  else {
    interest->publisherPublicKeyDigest = 0;
    interest->publisherPublicKeyDigestLength = 0;
  }
  
  // TODO: Implement exclude
  
  if (error = ndn_BinaryXMLDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXML_DTag_ChildSelector, &interest->childSelector))
    return error;
  if (error = ndn_BinaryXMLDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXML_DTag_AnswerOriginKind, &interest->answerOriginKind))
    return error;
  if (error = ndn_BinaryXMLDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXML_DTag_Scope, &interest->scope))
    return error;
  
  if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_InterestLifetime, &gotExpectedTag))
    return error;
  if (gotExpectedTag) {
    unsigned char *interestLifetime;
    unsigned int interestLifetimeLength;
    if (error = ndn_BinaryXMLDecoder_readBinaryDTagElement
        (decoder, ndn_BinaryXML_DTag_InterestLifetime, 0, &interestLifetime, &interestLifetimeLength))
      return error;
    
    interest->interestLifetime = (int)(1000.0 * 
      (double)ndn_BinaryXMLDecoder_bigEndianToUnsignedInt(interestLifetime, interestLifetimeLength) / 4096.0);
  }
  else
    interest->interestLifetime = -1;
  
  if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_Nonce, &gotExpectedTag))
    return error;
  if (gotExpectedTag) {
    if (error = ndn_BinaryXMLDecoder_readBinaryDTagElement
        (decoder, ndn_BinaryXML_DTag_Nonce, 0, &interest->nonce, &interest->nonceLength))
      return error;
  }
  else {
    interest->nonce = 0;
    interest->nonceLength = 0;
  }

  if (error = ndn_BinaryXMLDecoder_readElementClose(decoder))
    return error;
  
  return 0;
}

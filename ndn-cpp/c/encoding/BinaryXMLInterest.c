/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "BinaryXMLEncoder.h"
#include "BinaryXMLDecoder.h"
#include "BinaryXMLName.h"
#include "BinaryXMLInterest.h"

char *ndn_decodeBinaryXMLInterest(struct ndn_Interest *interest, struct ndn_BinaryXMLDecoder *decoder)
{
  char *error;
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
    
    interest->interestLifetime = 1000 * 
      ndn_BinaryXMLDecoder_bigEndianToUnsignedInt(interestLifetime, interestLifetimeLength) / 4096;
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
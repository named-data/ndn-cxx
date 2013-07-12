/**
 * @author: Jeff Thompson
 * Derived from Interest.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "BinaryXMLEncoder.h"
#include "BinaryXMLDecoder.h"
#include "BinaryXMLName.h"
#include "BinaryXMLPublisherPublicKeyDigest.h"
#include "BinaryXMLInterest.h"

static ndn_Error encodeExclude(struct ndn_Exclude *exclude, struct ndn_BinaryXMLEncoder *encoder)
{
  if (exclude->nEntries == 0)
    return 0;
  
  ndn_Error error;
  if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(encoder, ndn_BinaryXML_DTag_Exclude))
    return error;
  
  // TODO: Do we want to order the components (except for ANY)?
  unsigned int i;
  for (i = 0; i < exclude->nEntries; ++i) {
    struct ndn_ExcludeEntry *entry = &exclude->entries[i];
    
    if (entry->type == ndn_Exclude_COMPONENT) {
      if (error = ndn_BinaryXMLEncoder_writeBlobDTagElement
          (encoder, ndn_BinaryXML_DTag_Component, entry->component, entry->componentLength))
        return error;
    }
    else if (entry->type == ndn_Exclude_ANY) {
      if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(encoder, ndn_BinaryXML_DTag_Any))
        return error;
    	if (error = ndn_BinaryXMLEncoder_writeElementClose(encoder))
        return error;
    }
    else
      return NDN_ERROR_unrecognized_ndn_ExcludeType;
	}
  
	if (error = ndn_BinaryXMLEncoder_writeElementClose(encoder))
    return error;
  
  return 0;  
}

static ndn_Error decodeExclude(struct ndn_Exclude *exclude, struct ndn_BinaryXMLDecoder *decoder)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLDecoder_readElementStartDTag(decoder, ndn_BinaryXML_DTag_Exclude))
    return error;
    
  exclude->nEntries = 0;
  while (1) {
    int gotExpectedTag;
    
    if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_Component, &gotExpectedTag))
      return error;    
    if (gotExpectedTag) {
      // Component
      unsigned char *component;
      unsigned int componentLen;
      if (error = ndn_BinaryXMLDecoder_readBinaryDTagElement(decoder, ndn_BinaryXML_DTag_Component, 0, &component, &componentLen))
        return error;
    
      // Add the component entry.
      if (exclude->nEntries >= exclude->maxEntries)
        return NDN_ERROR_read_an_entry_past_the_maximum_number_of_entries_allowed_in_the_exclude;
      ndn_ExcludeEntry_init(exclude->entries + exclude->nEntries, ndn_Exclude_COMPONENT, component, componentLen);
      ++exclude->nEntries;

      continue;
    }
    
    if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_Any, &gotExpectedTag))
      return error;    
    if (gotExpectedTag) {
      // Any
      if (error = ndn_BinaryXMLDecoder_readElementStartDTag(decoder, ndn_BinaryXML_DTag_Any))
        return error;
      if (error = ndn_BinaryXMLDecoder_readElementClose(decoder))
        return error;
    
      // Add the any entry.
      if (exclude->nEntries >= exclude->maxEntries)
        return NDN_ERROR_read_an_entry_past_the_maximum_number_of_entries_allowed_in_the_exclude;
      ndn_ExcludeEntry_init(exclude->entries + exclude->nEntries, ndn_Exclude_ANY, 0, 0);
      ++exclude->nEntries;

      continue;
    }
    
    if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_Bloom, &gotExpectedTag))
      return error;    
    if (gotExpectedTag) {
      // Skip the Bloom and treat it as Any.
      unsigned char *value;
      unsigned int valueLen;
      if (error = ndn_BinaryXMLDecoder_readBinaryDTagElement(decoder, ndn_BinaryXML_DTag_Bloom, 0, &value, &valueLen))
        return error;
    
      // Add the any entry.
      if (exclude->nEntries >= exclude->maxEntries)
        return NDN_ERROR_read_an_entry_past_the_maximum_number_of_entries_allowed_in_the_exclude;
      ndn_ExcludeEntry_init(exclude->entries + exclude->nEntries, ndn_Exclude_ANY, 0, 0);
      ++exclude->nEntries;

      continue;
    }
    
    // Else no more entries.
    break;
  }
  
  if (error = ndn_BinaryXMLDecoder_readElementClose(decoder))
    return error;
  
  return 0;
}

ndn_Error ndn_encodeBinaryXMLInterest(struct ndn_Interest *interest, struct ndn_BinaryXMLEncoder *encoder)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(encoder, ndn_BinaryXML_DTag_Interest))
    return error;
    
  if (error = ndn_encodeBinaryXMLName(&interest->name, encoder))
    return error;
  
  if (error = ndn_BinaryXMLEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXML_DTag_MinSuffixComponents, interest->minSuffixComponents))
    return error;
  if (error = ndn_BinaryXMLEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXML_DTag_MaxSuffixComponents, interest->maxSuffixComponents))
    return error;
    
  // This will skip encoding if there is no publisherPublicKeyDigest.
  if (error = ndn_encodeBinaryXMLPublisherPublicKeyDigest(&interest->publisherPublicKeyDigest, encoder))
    return error;
  
  // This will skip encoding if there is no exclude.
  if (error = encodeExclude(&interest->exclude, encoder))
    return error;

  if (error = ndn_BinaryXMLEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXML_DTag_ChildSelector, interest->childSelector))
    return error;
  if (interest->answerOriginKind >= 0 && interest->answerOriginKind != ndn_Interest_DEFAULT_ANSWER_ORIGIN_KIND) {
    if (error = ndn_BinaryXMLEncoder_writeUnsignedDecimalIntDTagElement
        (encoder, ndn_BinaryXML_DTag_AnswerOriginKind, (unsigned int)interest->answerOriginKind))
      return error;
  }
  if (error = ndn_BinaryXMLEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXML_DTag_Scope, interest->scope))
    return error;
  
  if (error = ndn_BinaryXMLEncoder_writeOptionalTimeMillisecondsDTagElement
      (encoder, ndn_BinaryXML_DTag_InterestLifetime, interest->interestLifetimeMilliseconds))
    return error;
  
  if (error = ndn_BinaryXMLEncoder_writeOptionalBlobDTagElement
      (encoder, ndn_BinaryXML_DTag_Nonce, interest->nonce, interest->nonceLength))
    return error;
  
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
  
  if (error = ndn_decodeOptionalBinaryXMLPublisherPublicKeyDigest(&interest->publisherPublicKeyDigest, decoder))
    return error;
  
  int gotExpectedTag;
  if (error = ndn_BinaryXMLDecoder_peekDTag(decoder, ndn_BinaryXML_DTag_Exclude, &gotExpectedTag))
    return error;
  if (gotExpectedTag) {
    if (error = decodeExclude(&interest->exclude, decoder))
      return error;
  }
  else
    interest->exclude.nEntries = 0;
  
  if (error = ndn_BinaryXMLDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXML_DTag_ChildSelector, &interest->childSelector))
    return error;
  if (error = ndn_BinaryXMLDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXML_DTag_AnswerOriginKind, &interest->answerOriginKind))
    return error;
  if (error = ndn_BinaryXMLDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXML_DTag_Scope, &interest->scope))
    return error;
  
  if (error= ndn_BinaryXMLDecoder_readOptionalTimeMillisecondsDTagElement
      (decoder, ndn_BinaryXML_DTag_InterestLifetime, &interest->interestLifetimeMilliseconds))
    return error;
  
  if (error = ndn_BinaryXMLDecoder_readOptionalBinaryDTagElement
      (decoder, ndn_BinaryXML_DTag_Nonce, 0, &interest->nonce, &interest->nonceLength))
    return error;

  if (error = ndn_BinaryXMLDecoder_readElementClose(decoder))
    return error;
  
  return 0;
}

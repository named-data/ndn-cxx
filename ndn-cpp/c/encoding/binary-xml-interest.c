/**
 * @author: Jeff Thompson
 * Derived from Interest.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "binary-xml-encoder.h"
#include "binary-xml-decoder.h"
#include "binary-xml-name.h"
#include "binary-xml-publisher-public-key-digest.h"
#include "binary-xml-interest.h"

static ndn_Error encodeExclude(struct ndn_Exclude *exclude, struct ndn_BinaryXmlEncoder *encoder)
{
  if (exclude->nEntries == 0)
    return NDN_ERROR_success;
  
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_Exclude)))
    return error;
  
  // TODO: Do we want to order the components (except for ANY)?
  unsigned int i;
  for (i = 0; i < exclude->nEntries; ++i) {
    struct ndn_ExcludeEntry *entry = &exclude->entries[i];
    
    if (entry->type == ndn_Exclude_COMPONENT) {
      if ((error = ndn_BinaryXmlEncoder_writeBlobDTagElement
          (encoder, ndn_BinaryXml_DTag_Component, entry->component, entry->componentLength)))
        return error;
    }
    else if (entry->type == ndn_Exclude_ANY) {
      if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_Any)))
        return error;
      if ((error = ndn_BinaryXmlEncoder_writeElementClose(encoder)))
        return error;
    }
    else
      return NDN_ERROR_unrecognized_ndn_ExcludeType;
  }
  
  if ((error = ndn_BinaryXmlEncoder_writeElementClose(encoder)))
    return error;
  
  return NDN_ERROR_success;  
}

static ndn_Error decodeExclude(struct ndn_Exclude *exclude, struct ndn_BinaryXmlDecoder *decoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(decoder, ndn_BinaryXml_DTag_Exclude)))
    return error;
    
  exclude->nEntries = 0;
  while (1) {
    int gotExpectedTag;
    
    if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_Component, &gotExpectedTag)))
      return error;    
    if (gotExpectedTag) {
      // Component
      unsigned char *component;
      unsigned int componentLen;
      if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement(decoder, ndn_BinaryXml_DTag_Component, 0, &component, &componentLen)))
        return error;
    
      // Add the component entry.
      if (exclude->nEntries >= exclude->maxEntries)
        return NDN_ERROR_read_an_entry_past_the_maximum_number_of_entries_allowed_in_the_exclude;
      ndn_ExcludeEntry_init(exclude->entries + exclude->nEntries, ndn_Exclude_COMPONENT, component, componentLen);
      ++exclude->nEntries;

      continue;
    }
    
    if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_Any, &gotExpectedTag)))
      return error;    
    if (gotExpectedTag) {
      // Any
      if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(decoder, ndn_BinaryXml_DTag_Any)))
        return error;
      if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
        return error;
    
      // Add the any entry.
      if (exclude->nEntries >= exclude->maxEntries)
        return NDN_ERROR_read_an_entry_past_the_maximum_number_of_entries_allowed_in_the_exclude;
      ndn_ExcludeEntry_init(exclude->entries + exclude->nEntries, ndn_Exclude_ANY, 0, 0);
      ++exclude->nEntries;

      continue;
    }
    
    if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_Bloom, &gotExpectedTag)))
      return error;    
    if (gotExpectedTag) {
      // Skip the Bloom and treat it as Any.
      unsigned char *value;
      unsigned int valueLen;
      if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement(decoder, ndn_BinaryXml_DTag_Bloom, 0, &value, &valueLen)))
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
  
  if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_encodeBinaryXmlInterest(struct ndn_Interest *interest, struct ndn_BinaryXmlEncoder *encoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(encoder, ndn_BinaryXml_DTag_Interest)))
    return error;
    
  if ((error = ndn_encodeBinaryXmlName(&interest->name, encoder)))
    return error;
  
  if ((error = ndn_BinaryXmlEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXml_DTag_MinSuffixComponents, interest->minSuffixComponents)))
    return error;
  if ((error = ndn_BinaryXmlEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXml_DTag_MaxSuffixComponents, interest->maxSuffixComponents)))
    return error;
    
  // This will skip encoding if there is no publisherPublicKeyDigest.
  if ((error = ndn_encodeBinaryXmlPublisherPublicKeyDigest(&interest->publisherPublicKeyDigest, encoder)))
    return error;
  
  // This will skip encoding if there is no exclude.
  if ((error = encodeExclude(&interest->exclude, encoder)))
    return error;

  if ((error = ndn_BinaryXmlEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXml_DTag_ChildSelector, interest->childSelector)))
    return error;
  if (interest->answerOriginKind >= 0 && interest->answerOriginKind != ndn_Interest_DEFAULT_ANSWER_ORIGIN_KIND) {
    if ((error = ndn_BinaryXmlEncoder_writeUnsignedDecimalIntDTagElement
        (encoder, ndn_BinaryXml_DTag_AnswerOriginKind, (unsigned int)interest->answerOriginKind)))
      return error;
  }
  if ((error = ndn_BinaryXmlEncoder_writeOptionalUnsignedDecimalIntDTagElement
      (encoder, ndn_BinaryXml_DTag_Scope, interest->scope)))
    return error;
  
  if ((error = ndn_BinaryXmlEncoder_writeOptionalTimeMillisecondsDTagElement
      (encoder, ndn_BinaryXml_DTag_InterestLifetime, interest->interestLifetimeMilliseconds)))
    return error;
  
  if ((error = ndn_BinaryXmlEncoder_writeOptionalBlobDTagElement
      (encoder, ndn_BinaryXml_DTag_Nonce, interest->nonce, interest->nonceLength)))
    return error;
  
  if ((error = ndn_BinaryXmlEncoder_writeElementClose(encoder)))
    return error;
  
  return NDN_ERROR_success;  
}

ndn_Error ndn_decodeBinaryXmlInterest(struct ndn_Interest *interest, struct ndn_BinaryXmlDecoder *decoder)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(decoder, ndn_BinaryXml_DTag_Interest)))
    return error;
    
  if ((error = ndn_decodeBinaryXmlName(&interest->name, decoder)))
    return error;
       
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_MinSuffixComponents, &interest->minSuffixComponents)))
    return error;
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_MaxSuffixComponents, &interest->maxSuffixComponents)))
    return error;
  
  if ((error = ndn_decodeOptionalBinaryXmlPublisherPublicKeyDigest(&interest->publisherPublicKeyDigest, decoder)))
    return error;
  
  int gotExpectedTag;
  if ((error = ndn_BinaryXmlDecoder_peekDTag(decoder, ndn_BinaryXml_DTag_Exclude, &gotExpectedTag)))
    return error;
  if (gotExpectedTag) {
    if ((error = decodeExclude(&interest->exclude, decoder)))
      return error;
  }
  else
    interest->exclude.nEntries = 0;
  
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_ChildSelector, &interest->childSelector)))
    return error;
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_AnswerOriginKind, &interest->answerOriginKind)))
    return error;
  if ((error = ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
      (decoder, ndn_BinaryXml_DTag_Scope, &interest->scope)))
    return error;
  
  if (error= ndn_BinaryXmlDecoder_readOptionalTimeMillisecondsDTagElement
      (decoder, ndn_BinaryXml_DTag_InterestLifetime, &interest->interestLifetimeMilliseconds))
    return error;
  
  if ((error = ndn_BinaryXmlDecoder_readOptionalBinaryDTagElement
      (decoder, ndn_BinaryXml_DTag_Nonce, 0, &interest->nonce, &interest->nonceLength)))
    return error;

  if ((error = ndn_BinaryXmlDecoder_readElementClose(decoder)))
    return error;
  
  return NDN_ERROR_success;
}

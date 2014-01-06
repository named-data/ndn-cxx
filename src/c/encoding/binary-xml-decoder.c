/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * Derived from BinaryXMLDecoder.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include "binary-xml.h"
#include "binary-xml-decoder.h"

/**
 * Return the octet at self->offset, converting to unsigned int.  Increment self->offset.
 * This does not check for reading past the end of the input, so this is called "unsafe".
 */
static inline unsigned int unsafeReadOctet(struct ndn_BinaryXmlDecoder *self) 
{
  return (unsigned int)(self->input[self->offset++] & 0xff);  
}

/**
 * Return the octet at self->offset, converting to unsigned int.  Do not increment self->offset.
 * This does not check for reading past the end of the input, so this is called "unsafe".
 */
static inline unsigned int unsafeGetOctet(struct ndn_BinaryXmlDecoder *self) 
{
  return (unsigned int)(self->input[self->offset] & 0xff);  
}

/**
 * Parse the value as a decimal unsigned integer.  This does not check for whitespace or + sign.
 * If valueLength is 0, this succeeds with resultOut 0.
 * @param value
 * @param valueLength
 * @param resultOut output the parsed integer.
 * @return 0 for success, else an error code, including if an element of value is not a decimal digit.
 */
static ndn_Error parseUnsignedDecimalInt(uint8_t *value, size_t valueLength, unsigned int *resultOut)
{
  unsigned int result = 0;
  
  size_t i;
  for (i = 0; i < valueLength; ++i) {
    uint8_t digit = value[i];
    if (!(digit >= '0' && digit <= '9'))
      return NDN_ERROR_element_of_value_is_not_a_decimal_digit;

    result *= 10;
    result += (unsigned int)(digit - '0');
  }
    
  *resultOut = result;
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlDecoder_decodeTypeAndValue(struct ndn_BinaryXmlDecoder *self, unsigned int *type, unsigned int *valueOut) 
{
  unsigned int value = 0;
  int gotFirstOctet = 0;
  
  while (1) {
    if (self->offset >= self->inputLength)
      return NDN_ERROR_read_past_the_end_of_the_input;
    
    unsigned int octet = unsafeReadOctet(self);
    
    if (!gotFirstOctet) {
      if (octet == 0)
        return NDN_ERROR_the_first_header_octet_may_not_be_zero;
      
      gotFirstOctet = 1;
    }
    
    if (octet & ndn_BinaryXml_TT_FINAL) {
      // Finished.
      *type = octet & ndn_BinaryXml_TT_MASK;
      value = (value << ndn_BinaryXml_TT_VALUE_BITS) | ((octet >> ndn_BinaryXml_TT_BITS) & ndn_BinaryXml_TT_VALUE_MASK);
      break;
    }
    
    value = (value << ndn_BinaryXml_REGULAR_VALUE_BITS) | (octet & ndn_BinaryXml_REGULAR_VALUE_MASK);    
  }

  *valueOut = value;
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlDecoder_readElementStartDTag(struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag)
{
  if (self->offset == self->previouslyPeekedDTagStartOffset) {
    // peekDTag already decoded this DTag.
    if (self->previouslyPeekedDTag != expectedTag)
      return NDN_ERROR_did_not_get_the_expected_DTAG;

    // Fast forward past the header.
    self->offset = self->previouslyPeekedDTagEndOffset;
  }
  else {
    ndn_Error error;
    unsigned int type;
    unsigned int value;
    if ((error = ndn_BinaryXmlDecoder_decodeTypeAndValue(self, &type, &value)))
      return error;

    if (type != ndn_BinaryXml_DTAG)
      return NDN_ERROR_header_type_is_not_a_DTAG;

    if (value != expectedTag)
      return NDN_ERROR_did_not_get_the_expected_DTAG;
  }
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlDecoder_readElementClose(struct ndn_BinaryXmlDecoder *self)
{
  if (self->offset >= self->inputLength)
    return NDN_ERROR_read_past_the_end_of_the_input;
  
  if (unsafeReadOctet(self) != ndn_BinaryXml_CLOSE)
    return NDN_ERROR_did_not_get_the_expected_element_close;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlDecoder_peekDTag(struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, int *gotExpectedTag)
{
  if (self->offset == self->previouslyPeekedDTagStartOffset)
    // We already decoded this DTag.
    *gotExpectedTag = (self->previouslyPeekedDTag == expectedTag ? 1 : 0);
  else {
    // Default to 0.
    *gotExpectedTag = 0;

    // First check if it is an element close (which cannot be the expected tag).  
    if (self->offset >= self->inputLength)
      return NDN_ERROR_read_past_the_end_of_the_input;
    if (unsafeGetOctet(self) == ndn_BinaryXml_CLOSE)
      return NDN_ERROR_success;

    unsigned int type;
    unsigned int value;
    size_t saveOffset = self->offset;
    ndn_Error error = ndn_BinaryXmlDecoder_decodeTypeAndValue(self, &type, &value);
    // readElementStartDTag will use this to fast forward.
    self->previouslyPeekedDTagEndOffset = self->offset;
    // Restore offset.
    self->offset = saveOffset;

    if (error)
      return error;

    if (type == ndn_BinaryXml_DTAG) {
      self->previouslyPeekedDTagStartOffset = saveOffset;
      self->previouslyPeekedDTag = value;

      if (value == expectedTag)
        *gotExpectedTag = 1;
    }
  }
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlDecoder_readBinaryDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, int allowNull, struct ndn_Blob *value)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(self, expectedTag)))
    return error;
  
  if (allowNull) {
    if (self->offset >= self->inputLength)
      return NDN_ERROR_read_past_the_end_of_the_input;
  
    if (unsafeGetOctet(self) == ndn_BinaryXml_CLOSE) {
      // The binary item is missing, and this is allowed, so read the element close and return a null value.
      ++self->offset;
      value->value = 0;
      value->length = 0;
      return NDN_ERROR_success;
    }
  }
  
  unsigned int itemType;
  unsigned int uintValueLength;
  if ((error = ndn_BinaryXmlDecoder_decodeTypeAndValue(self, &itemType, &uintValueLength)))
    return error;
  // Ignore itemType.
  value->value = self->input + self->offset;
  value->length = (size_t)uintValueLength;
  self->offset += value->length;
  
  if ((error = ndn_BinaryXmlDecoder_readElementClose(self)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlDecoder_readOptionalBinaryDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, int allowNull, struct ndn_Blob *value)
{
  ndn_Error error;
  int gotExpectedTag;
  if ((error = ndn_BinaryXmlDecoder_peekDTag(self, expectedTag, &gotExpectedTag)))
    return error;
  if (gotExpectedTag) {
    if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement(self, expectedTag, allowNull, value)))
      return error;
  }
  else {
    value->value = 0;
    value->length = 0;
  }  
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlDecoder_readUDataDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, struct ndn_Blob *value)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_readElementStartDTag(self, expectedTag)))
    return error;
    
  unsigned int itemType;
  unsigned int uintValueLength;
  if ((error = ndn_BinaryXmlDecoder_decodeTypeAndValue(self, &itemType, &uintValueLength)))
    return error;
  if (itemType != ndn_BinaryXml_UDATA)
    return NDN_ERROR_item_is_not_UDATA;
  value->value = self->input + self->offset;
  value->length = uintValueLength;
  self->offset += value->length;
  
  if ((error = ndn_BinaryXmlDecoder_readElementClose(self)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlDecoder_readOptionalUDataDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, struct ndn_Blob *value)
{
  ndn_Error error;
  int gotExpectedTag;
  if ((error = ndn_BinaryXmlDecoder_peekDTag(self, expectedTag, &gotExpectedTag)))
    return error;
  if (gotExpectedTag) {
    if ((error = ndn_BinaryXmlDecoder_readUDataDTagElement(self, expectedTag, value)))
      return error;
  }
  else {
    value->value = 0;
    value->length = 0;
  }  
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlDecoder_readUnsignedIntegerDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, unsigned int *value)
{
  struct ndn_Blob udataValue;
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_readUDataDTagElement(self, expectedTag, &udataValue)))
    return error;
  
  if ((error = parseUnsignedDecimalInt(udataValue.value, udataValue.length, value)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, int *value)
{
  int gotExpectedTag;
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_peekDTag(self, expectedTag, &gotExpectedTag)))
    return error;
    
  if (!gotExpectedTag) {
    *value = -1;
    return NDN_ERROR_success;
  }

  unsigned int unsignedValue;
  if ((error = ndn_BinaryXmlDecoder_readUnsignedIntegerDTagElement(self, expectedTag, &unsignedValue)))
    return error;
  
  *value = (int)unsignedValue;
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlDecoder_readTimeMillisecondsDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, ndn_MillisecondsSince1970 *milliseconds)
{
  ndn_Error error;
  struct ndn_Blob bytes;
  if ((error = ndn_BinaryXmlDecoder_readBinaryDTagElement(self, expectedTag, 0, &bytes)))
    return error;
    
  *milliseconds = 1000.0 * ndn_BinaryXmlDecoder_unsignedBigEndianToDouble(bytes.value, bytes.length) / 4096.0;
  return NDN_ERROR_success;  
}

ndn_Error ndn_BinaryXmlDecoder_readOptionalTimeMillisecondsDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, ndn_MillisecondsSince1970 *milliseconds)
{
  int gotExpectedTag;
  ndn_Error error;
  if ((error = ndn_BinaryXmlDecoder_peekDTag(self, expectedTag, &gotExpectedTag)))
    return error;
    
  if (!gotExpectedTag) {
    *milliseconds = -1.0;
    return NDN_ERROR_success;
  }

  if ((error = ndn_BinaryXmlDecoder_readTimeMillisecondsDTagElement(self, expectedTag, milliseconds)))
    return error;
  
  return NDN_ERROR_success;
}

double ndn_BinaryXmlDecoder_unsignedBigEndianToDouble(uint8_t *bytes, size_t bytesLength) 
{
  double result = 0.0;
  size_t i;
  for (i = 0; i < bytesLength; ++i) {
    result *= 256.0;
    result += (double)bytes[i];
  }
  
  return result;
}

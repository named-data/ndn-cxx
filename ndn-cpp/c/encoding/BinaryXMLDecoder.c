/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include "BinaryXML.h"
#include "BinaryXMLDecoder.h"

/**
 * Return the octet at self->offset, converting to unsigned int.  Increment self->offset.
 * This does not check for reading past the end of the input, so this is called "unsafe".
 */
static inline unsigned int unsafeReadOctet(struct ndn_BinaryXMLDecoder *self) 
{
  return (unsigned int)(self->input[self->offset++] & 0xff);  
}

/**
 * Return the octet at self->offset, converting to unsigned int.  Do not increment self->offset.
 * This does not check for reading past the end of the input, so this is called "unsafe".
 */
static inline unsigned int unsafeGetOctet(struct ndn_BinaryXMLDecoder *self) 
{
  return (unsigned int)(self->input[self->offset] & 0xff);  
}

/**
 * Parse the value as a decimal unsigned integer.  This does not check for whitespace or + sign.
 * If valueLength is 0, this succeeds with resultOut 0.
 * @param value
 * @param valueLength
 * @param resultOut output the parsed integer.
 * @return 0 for success, else an error string, including if an element of value is not a decimal digit.
 */
static char *parseUnsignedDecimalInt(unsigned char *value, unsigned int valueLength, unsigned int *resultOut)
{
  unsigned int result = 0;
  
  unsigned int i;
  for (i = 0; i < valueLength; ++i) {
    unsigned char digit = value[i];
    if (!(digit >= '0' && digit <= '9'))
      return "parseUnsignedDecimalInt: element of value is not a decimal digit";

    result *= 10;
    result += (unsigned int)(digit - '0');
  }
    
  *resultOut = result;
  return 0;
}

char *ndn_BinaryXMLDecoder_decodeTypeAndValue(struct ndn_BinaryXMLDecoder *self, unsigned int *type, unsigned int *valueOut) 
{
  unsigned int value = 0;
  int gotFirstOctet = 0;
  
	while (1) {
    if (self->offset >= self->inputLength)
      return "ndn_BinaryXMLDecoder_decodeTypeAndVal: read past the end of the input";
    
		unsigned int octet = unsafeReadOctet(self);
		
    if (!gotFirstOctet) {
      if (octet == 0)
        return "ndn_BinaryXMLDecoder_decodeTypeAndVal: the first header octet may not be zero";
      
      gotFirstOctet = 1;
    }
    
		if (octet & ndn_BinaryXML_TT_FINAL) {
      // Finished.
			*type = octet & ndn_BinaryXML_TT_MASK;
			value = (value << ndn_BinaryXML_TT_VALUE_BITS) | ((octet >> ndn_BinaryXML_TT_BITS) & ndn_BinaryXML_TT_VALUE_MASK);
      break;
		}
		
    value = (value << ndn_BinaryXML_REGULAR_VALUE_BITS) | (octet & ndn_BinaryXML_REGULAR_VALUE_MASK);		
	}

	*valueOut = value;
  return 0;
}

char *ndn_BinaryXMLDecoder_readElementStartDTag(struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag)
{
  char *error;
  unsigned int type;
  unsigned int value;
  if (error = ndn_BinaryXMLDecoder_decodeTypeAndValue(self, &type, &value))
    return error;
  
  if (type != ndn_BinaryXML_DTAG)
    return "ndn_BinaryXMLDecoder_readElementStartDTag: header type is not a DTAG";
  
  if (value != expectedTag)
    return "ndn_BinaryXMLDecoder_readElementStartDTag: did not get the expected DTAG";
  
  return 0;
}

char *ndn_BinaryXMLDecoder_readElementClose(struct ndn_BinaryXMLDecoder *self)
{
  if (self->offset >= self->inputLength)
    return "ndn_BinaryXMLDecoder_readElementClose: read past the end of the input";
  
  if (unsafeReadOctet(self) != ndn_BinaryXML_CLOSE)
    return "ndn_BinaryXMLDecoder_readElementStartDTag: did not get the expected element close";
  
  return 0;
}

char *ndn_BinaryXMLDecoder_peekDTag(struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, int *gotExpectedTag)
{
  // Default to 0.
  *gotExpectedTag = 0;

  // First check if it is an element close (which cannot be the expected tag).  
  if (self->offset >= self->inputLength)
    return "ndn_BinaryXMLDecoder_readElementClose: read past the end of the input";
  if (unsafeGetOctet(self) == 0)
    return 0;

  unsigned int type;
  unsigned int value;
  unsigned int saveOffset = self->offset;
  char *error = ndn_BinaryXMLDecoder_decodeTypeAndValue(self, &type, &value);
  // Restore offset.
  self->offset = saveOffset;
  
  if (error)
    return error;
  
  if (type == ndn_BinaryXML_DTAG && value == expectedTag)
    *gotExpectedTag = 1;
  
  return 0;
}

char *ndn_BinaryXMLDecoder_readBinaryDTagElement
  (struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, int allowNull, unsigned char **value, unsigned int *valueLen)
{
  char *error;
  if (error = ndn_BinaryXMLDecoder_readElementStartDTag(self, expectedTag))
    return error;
  
  if (allowNull) {
    if (self->offset >= self->inputLength)
      return "ndn_BinaryXMLDecoder_readBinaryDTagElement: read past the end of the input";
  
    if (unsafeGetOctet(self) == ndn_BinaryXML_CLOSE) {
      // The binary item is missing, and this is allowed, so read the element close and return a null value.
      ++self->offset;
      *value = 0;
      *valueLen = 0;
      return 0;
    }
  }
  
  unsigned int itemType;
  if (error = ndn_BinaryXMLDecoder_decodeTypeAndValue(self, &itemType, valueLen))
    return error;
  // Ignore itemType.
  *value = self->input + self->offset;
  self->offset += *valueLen;
  
  if (error = ndn_BinaryXMLDecoder_readElementClose(self))
    return error;
  
  return 0;
}

char *ndn_BinaryXMLDecoder_readUDataDTagElement
  (struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, unsigned char **value, unsigned int *valueLen)
{
  char *error;
  if (error = ndn_BinaryXMLDecoder_readElementStartDTag(self, expectedTag))
    return error;
    
  unsigned int itemType;
  if (error = ndn_BinaryXMLDecoder_decodeTypeAndValue(self, &itemType, valueLen))
    return error;
  if (itemType != ndn_BinaryXML_UDATA)
    return "ndn_BinaryXMLDecoder_readUDataDTagElement: item is not UDATA";
  *value = self->input + self->offset;
  self->offset += *valueLen;
  
  if (error = ndn_BinaryXMLDecoder_readElementClose(self))
    return error;
  
  return 0;
}

char *ndn_BinaryXMLDecoder_readUnsignedIntegerDTagElement
  (struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, unsigned int *value)
{
  unsigned char *udataValue;
  unsigned int udataValueLength;
  char *error;
  if (error = ndn_BinaryXMLDecoder_readUDataDTagElement(self, expectedTag, &udataValue, &udataValueLength))
    return error;
  
  if (error = parseUnsignedDecimalInt(udataValue, udataValueLength, value))
    return error;
  
  return 0;
}

char *ndn_BinaryXMLDecoder_readOptionalUnsignedIntegerDTagElement
  (struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, int *value)
{
  int gotExpectedTag;
  char *error;
  if (error = ndn_BinaryXMLDecoder_peekDTag(self, expectedTag, &gotExpectedTag))
    return error;
    
  if (!gotExpectedTag) {
    value = -1;
    return 0;
  }

  unsigned int unsignedValue;
  if (error = ndn_BinaryXMLDecoder_readUnsignedIntegerDTagElement(self, expectedTag, &unsignedValue))
    return error;
  
  *value = (int)unsignedValue;
  return 0;
}

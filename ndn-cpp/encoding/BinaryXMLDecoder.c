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

char *ndn_BinaryXMLDecoder_decodeTypeAndValue(struct ndn_BinaryXMLDecoder *self, unsigned int *type, unsigned int *valueOut) 
{
  unsigned int value = 0;
  
	while (1) {
    if (self->offset >= self->inputLength)
      return "ndn_BinaryXMLDecoder_decodeTypeAndVal read past the end of the input";
    
		unsigned int octet = unsafeReadOctet(self);
		
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

char *ndn_BinaryXMLDecoder_readDTag(struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag)
{
  char *error;
  unsigned int type;
  unsigned int value;
  if (error = ndn_BinaryXMLDecoder_decodeTypeAndValue(self, &type, &value))
    return error;
  
  if (type != ndn_BinaryXML_DTAG)
    return "ndn_BinaryXMLDecoder_readDTag: header type is not a DTAG";
  
  if (value != expectedTag)
    return "ndn_BinaryXMLDecoder_readDTag: did not get the expected DTAG";
  
  return 0;
}

char *ndn_BinaryXMLDecoder_readElementClose(struct ndn_BinaryXMLDecoder *self)
{
  if (self->offset >= self->inputLength)
    return "ndn_BinaryXMLDecoder_readElementClose read past the end of the input";
  
  if (unsafeReadOctet(self) != ndn_BinaryXML_CLOSE)
    return "ndn_BinaryXMLDecoder_readDTag: did not get the expected element close";
  
  return 0;
}

char *ndn_BinaryXMLDecoder_peekDTag(struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, int *gotExpectedTag)
{
  // Default to 0.
  *gotExpectedTag = 0;
  
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
  if (error = ndn_BinaryXMLDecoder_readDTag(self, expectedTag))
    return error;
  
  if (allowNull) {
    if (self->offset >= self->inputLength)
      return "ndn_BinaryXMLDecoder_readBinaryDTagElement read past the end of the input";
  
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

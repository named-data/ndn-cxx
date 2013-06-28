/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include "BinaryXML.h"
#include "BinaryXMLDecoder.h"

char *ndn_BinaryXMLDecoder_decodeTypeAndValue(struct ndn_BinaryXMLDecoder *self, unsigned int *type, unsigned int *valueOut) 
{
  unsigned int value = 0;
  
	while (1) {
    if (self->offset >= self->inputLength)
      return "ndn_BinaryXMLDecoder_decodeTypeAndVal read past the end of the input";
    
		unsigned int octet = (unsigned int)(self->input[self->offset++] & 0xff);
		
		if (octet & ndn_BinaryXML_TT_FINAL) {
      // Finished.
			*type = octet & ndn_BinaryXML_TT_MASK;
			value = (value << ndn_BinaryXML_TT_VALUE_BITS) | ((octet >> ndn_BinaryXML_TT_BITS) & ndn_BinaryXML_TT_VALUE_MASK);
      break;
		}
		
    value = (value << ndn_BinaryXML_REGULAR_VALUE_BITS) | (octet & ndn_BinaryXML_REGULAR_VALUE_MASK);		
	}

	*valueOut = value;
  return (char *)0;
}

char *ndn_BinaryXMLDecoder_readDTag(struct ndn_BinaryXMLDecoder *self, unsigned int tag)
{
  char *error;
  unsigned int type;
  unsigned int value;
  if (error = ndn_BinaryXMLDecoder_decodeTypeAndValue(self, &type, &value))
    return error;
  
  if (type != ndn_BinaryXML_DTAG)
    return "ndn_BinaryXMLDecoder_readDTag: header type is not a DTAG";
  
  if (value != tag)
    return "ndn_BinaryXMLDecoder_readDTag: did not get the expected DTAG";
  
  return (char *)0;
}
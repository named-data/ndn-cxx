/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include "BinaryXML.h"
#include "BinaryXMLEncoder.h"

enum {
  ENCODING_LIMIT_1_BYTE = ((1 << ndn_BinaryXML_TT_VALUE_BITS) - 1),
  ENCODING_LIMIT_2_BYTES = ((1 << (ndn_BinaryXML_TT_VALUE_BITS + ndn_BinaryXML_REGULAR_VALUE_BITS)) - 1),
  ENCODING_LIMIT_3_BYTES = ((1 << (ndn_BinaryXML_TT_VALUE_BITS + 2 * ndn_BinaryXML_REGULAR_VALUE_BITS)) - 1)
};

/**
 * Return the number of bytes to encode a header of value x.
 */
static unsigned int getNEncodingBytes(unsigned int x) 
{
  // Do a quick check for pre-compiled results.
	if (x <= ENCODING_LIMIT_1_BYTE) 
    return 1;
	if (x <= ENCODING_LIMIT_2_BYTES) 
    return 2;
	if (x <= ENCODING_LIMIT_3_BYTES) 
    return 3;
	
	unsigned int nBytes = 1;
	
	// Last byte gives you TT_VALUE_BITS.
	// Remainder each gives you REGULAR_VALUE_BITS.
	x >>= ndn_BinaryXML_TT_VALUE_BITS;
	while (x != 0) {
    ++nBytes;
	  x >>= ndn_BinaryXML_REGULAR_VALUE_BITS;
	}
  
	return nBytes;
}

char *ndn_BinaryXMLEncoder_encodeTypeAndValue(struct ndn_BinaryXMLEncoder *self, unsigned int type, unsigned int value)
{
	if (type > ndn_BinaryXML_UDATA)
		return "ndn_BinaryXMLEncoder_encodeTypeAndValue: type is out of range";
  
	// Encode backwards. Calculate how many bytes we need.
	unsigned int nEncodingBytes = getNEncodingBytes(value);
  char *error;
  if (error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + nEncodingBytes))
    return error;

	// Bottom 4 bits of value go in last byte with tag.
	self->output.array[self->offset + nEncodingBytes - 1] = 
		(ndn_BinaryXML_TT_MASK & type | 
		((ndn_BinaryXML_TT_VALUE_MASK & value) << ndn_BinaryXML_TT_BITS)) |
		ndn_BinaryXML_TT_FINAL; // set top bit for last byte
	value >>= ndn_BinaryXML_TT_VALUE_BITS;
	
	// Rest of value goes into preceding bytes, 7 bits per byte. (Zero top bit is "more" flag.)
	unsigned int i = self->offset + nEncodingBytes - 2;
	while (value != 0 && i >= self->offset) {
		self->output.array[i] = (value & ndn_BinaryXML_REGULAR_VALUE_MASK);
		value >>= ndn_BinaryXML_REGULAR_VALUE_BITS;
		--i;
	}
	if (value != 0)
    // This should not happen if getNEncodingBytes is correct.
		return "ndn_BinaryXMLEncoder_encodeTypeAndValue: : miscalculated N encoding bytes";
	
	self->offset+= nEncodingBytes;
  
  return 0;
}

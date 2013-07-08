/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "../util/ndn_memory.h"
#include "BinaryXML.h"
#include "BinaryXMLEncoder.h"

enum {
  ENCODING_LIMIT_1_BYTE = ((1 << ndn_BinaryXML_TT_VALUE_BITS) - 1),
  ENCODING_LIMIT_2_BYTES = ((1 << (ndn_BinaryXML_TT_VALUE_BITS + ndn_BinaryXML_REGULAR_VALUE_BITS)) - 1),
  ENCODING_LIMIT_3_BYTES = ((1 << (ndn_BinaryXML_TT_VALUE_BITS + 2 * ndn_BinaryXML_REGULAR_VALUE_BITS)) - 1)
};

/**
 * Call ndn_DynamicUCharArray_ensureLength to ensure that there is enough room in the output, and copy
 * array to the output.  This does not write a header.
 * @param self pointer to the ndn_BinaryXMLEncoder struct
 * @param array the array to copy
 * @param arrayLength the length of the array
 * @return 0 for success, else an error string
 */
static char *writeArray(struct ndn_BinaryXMLEncoder *self, unsigned char *array, unsigned int arrayLength)
{
  char *error;
  if (error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + arrayLength))
    return error;
  
  ndn_memcpy(self->output.array + self->offset, array, arrayLength);
	self->offset += arrayLength;
  
  return 0;
}

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

char *ndn_BinaryXMLEncoder_writeElementClose(struct ndn_BinaryXMLEncoder *self)
{
  char *error;
  if (error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + 1))
    return error;
  
	self->output.array[self->offset] = ndn_BinaryXML_CLOSE;
	self->offset += 1;
  
  return 0;
}

char *ndn_BinaryXMLEncoder_writeBlob(struct ndn_BinaryXMLEncoder *self, unsigned char *value, unsigned int valueLength)
{
  char *error;
  if (error = ndn_BinaryXMLEncoder_encodeTypeAndValue(self, ndn_BinaryXML_BLOB, valueLength))
    return error;
  
  if (error = writeArray(self, value, valueLength))
    return error;
  
  return 0;
}

char *ndn_BinaryXMLEncoder_writeBlobDTagElement(struct ndn_BinaryXMLEncoder *self, unsigned int tag, unsigned char *value, unsigned int valueLength)
{
  char *error;
  if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(self, tag))
    return error;
  
  if (error = ndn_BinaryXMLEncoder_writeBlob(self, value, valueLength))
    return error;  
  
  if (error = ndn_BinaryXMLEncoder_writeElementClose(self))
    return error;
  
  return 0;
}

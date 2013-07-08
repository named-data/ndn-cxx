/**
 * @author: Jeff Thompson
 * Derived from BinaryXMLEncoder.js by Meki Cheraoui.
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
 * @return 0 for success, else an error code
 */
static ndn_Error writeArray(struct ndn_BinaryXMLEncoder *self, unsigned char *array, unsigned int arrayLength)
{
  ndn_Error error;
  if (error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + arrayLength))
    return error;
  
  ndn_memcpy(self->output.array + self->offset, array, arrayLength);
	self->offset += arrayLength;
  
  return 0;
}

/**
 * Return the number of bytes to encode a header of value x.
 */
static unsigned int getNHeaderEncodingBytes(unsigned int x) 
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

/**
 * Reverse the length bytes in array starting at offset.
 * @param array
 * @param offset
 * @param length
 */
static void reverse(unsigned char *array, unsigned int offset, unsigned int length) 
{
  if (length == 0)
    return;
  
  unsigned char *left = array + offset;
  unsigned char *right = array + offset + length - 1;
  while (left < right) {
    // Swap.
    unsigned char temp = *left;
    *left = *right;
    *right = temp;
    
    ++left;
    --right;
  }
}

/**
 * Write x as an unsigned decimal integer to the output, using ndn_DynamicUCharArray_ensureLength.
 * This does not write a header.
 * @param self pointer to the ndn_BinaryXMLEncoder struct
 * @param x the unsigned int to write
 * @return 0 for success, else an error code
 */
static ndn_Error encodeUnsignedDecimalInt(struct ndn_BinaryXMLEncoder *self, unsigned int x) 
{
  // We write the value backwards, then reverse it.
  unsigned int startOffset = self->offset;
  
  while (1) {
    ndn_Error error;
    if (error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + 1))
      return error;
    
    self->output.array[self->offset++] = (unsigned char)(x % 10 + '0');
    x /= 10;
    
    if (x == 0)
      break;
  }
  
  // Now reverse.
  reverse(self->output.array, startOffset, self->offset - startOffset);
  return 0;
}

ndn_Error ndn_BinaryXMLEncoder_encodeTypeAndValue(struct ndn_BinaryXMLEncoder *self, unsigned int type, unsigned int value)
{
	if (type > ndn_BinaryXML_UDATA)
		return NDN_ERROR_header_type_is_out_of_range;
  
	// Encode backwards. Calculate how many bytes we need.
	unsigned int nEncodingBytes = getNHeaderEncodingBytes(value);
  ndn_Error error;
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
    // This should not happen if getNHeaderEncodingBytes is correct.
		return NDN_ERROR_encodeTypeAndValue_miscalculated_N_encoding_bytes;
	
	self->offset+= nEncodingBytes;
  
  return 0;
}

ndn_Error ndn_BinaryXMLEncoder_writeElementClose(struct ndn_BinaryXMLEncoder *self)
{
  ndn_Error error;
  if (error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + 1))
    return error;
  
	self->output.array[self->offset] = ndn_BinaryXML_CLOSE;
	self->offset += 1;
  
  return 0;
}

ndn_Error ndn_BinaryXMLEncoder_writeBlob(struct ndn_BinaryXMLEncoder *self, unsigned char *value, unsigned int valueLength)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLEncoder_encodeTypeAndValue(self, ndn_BinaryXML_BLOB, valueLength))
    return error;
  
  if (error = writeArray(self, value, valueLength))
    return error;
  
  return 0;
}

ndn_Error ndn_BinaryXMLEncoder_writeBlobDTagElement(struct ndn_BinaryXMLEncoder *self, unsigned int tag, unsigned char *value, unsigned int valueLength)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(self, tag))
    return error;
  
  if (error = ndn_BinaryXMLEncoder_writeBlob(self, value, valueLength))
    return error;  
  
  if (error = ndn_BinaryXMLEncoder_writeElementClose(self))
    return error;
  
  return 0;
}

ndn_Error ndn_BinaryXMLEncoder_writeUnsignedDecimalInt(struct ndn_BinaryXMLEncoder *self, unsigned int value)
{
  // First write the decimal int (to find out how many bytes it is), then shift it forward to make room for the header.
  unsigned int startOffset = self->offset;
  
  ndn_Error error;
  if (error = encodeUnsignedDecimalInt(self, value))
    return error;
  
  unsigned int nIntegerBytes = self->offset - startOffset;
  unsigned int nHeaderBytes = getNHeaderEncodingBytes(nIntegerBytes);
  if (error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + nHeaderBytes))
    return error;
  
  // Don't use memcpy to shift because its behavior is not guaranteed when the buffers overlap.
  // We are shifting forward, so start from the end of the buffer.
  unsigned char *source = self->output.array + startOffset + nIntegerBytes - 1;
  unsigned char *dest = source + nHeaderBytes;
  unsigned char *sourceFinal = self->output.array + startOffset;
  while (source >= sourceFinal)
    *(dest--) = *(source--);
  
  // Override the offset to force encodeTypeAndValue to encode at startOffset, then fix the offset.
  self->offset = startOffset;
  if (error = ndn_BinaryXMLEncoder_encodeTypeAndValue(self, ndn_BinaryXML_UDATA, nIntegerBytes))
    // We don't really expect to get an error, since we have already ensured the length.
    return error;
  self->offset = startOffset + nHeaderBytes + nIntegerBytes;
  
  return 0;
}

ndn_Error ndn_BinaryXMLEncoder_writeUnsignedDecimalIntDTagElement(struct ndn_BinaryXMLEncoder *self, unsigned int tag, unsigned int value)
{
  ndn_Error error;
  if (error = ndn_BinaryXMLEncoder_writeElementStartDTag(self, tag))
    return error;
  
  if (error = ndn_BinaryXMLEncoder_writeUnsignedDecimalInt(self, value))
    return error;  
  
  if (error = ndn_BinaryXMLEncoder_writeElementClose(self))
    return error;
  
  return 0;
}

/**
 * @author: Jeff Thompson
 * Derived from BinaryXMLEncoder.js by Meki Cheraoui.
 * See COPYING for copyright and distribution information.
 */

#include <math.h>
#include "../util/ndn_memory.h"
#include "binary-xml.h"
#include "binary-xml-encoder.h"

enum {
  ENCODING_LIMIT_1_BYTE = ((1 << ndn_BinaryXml_TT_VALUE_BITS) - 1),
  ENCODING_LIMIT_2_BYTES = ((1 << (ndn_BinaryXml_TT_VALUE_BITS + ndn_BinaryXml_REGULAR_VALUE_BITS)) - 1),
  ENCODING_LIMIT_3_BYTES = ((1 << (ndn_BinaryXml_TT_VALUE_BITS + 2 * ndn_BinaryXml_REGULAR_VALUE_BITS)) - 1)
};

/**
 * Call ndn_DynamicUCharArray_ensureLength to ensure that there is enough room in the output, and copy
 * array to the output.  This does not write a header.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param array the array to copy
 * @param arrayLength the length of the array
 * @return 0 for success, else an error code
 */
static ndn_Error writeArray(struct ndn_BinaryXmlEncoder *self, unsigned char *array, unsigned int arrayLength)
{
  ndn_Error error;
  if ((error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + arrayLength)))
    return error;
  
  ndn_memcpy(self->output.array + self->offset, array, arrayLength);
	self->offset += arrayLength;
  
  return NDN_ERROR_success;
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
	x >>= ndn_BinaryXml_TT_VALUE_BITS;
	while (x != 0) {
    ++nBytes;
	  x >>= ndn_BinaryXml_REGULAR_VALUE_BITS;
	}
  
	return nBytes;
}

/**
 * Reverse the length bytes in array.
 * @param array
 * @param length
 */
static void reverse(unsigned char *array, unsigned int length) 
{
  if (length == 0)
    return;
  
  unsigned char *left = array;
  unsigned char *right = array + length - 1;
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
 * Write x as an unsigned decimal integer to the output with the digits in reverse order, using ndn_DynamicUCharArray_ensureLength.
 * This does not write a header.
 * We encode in reverse order, because this is the natural way to encode the digits, and the caller can reverse as needed.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param x the unsigned int to write
 * @return 0 for success, else an error code
 */
static ndn_Error encodeReversedUnsignedDecimalInt(struct ndn_BinaryXmlEncoder *self, unsigned int x) 
{
  while (1) {
    ndn_Error error;
    if ((error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + 1)))
      return error;
    
    self->output.array[self->offset++] = (unsigned char)(x % 10 + '0');
    x /= 10;
    
    if (x == 0)
      break;
  }
  
  return NDN_ERROR_success;
}

/**
 * Reverse the buffer in self->output.array, then shift it right by the amount needed to prefix a header with type, 
 * then encode the header at startOffset.
 * startOffser it the position in self-output.array of the first byte of the buffer and self->offset is the first byte past the end.
 * We reverse and shift in the same function to avoid unnecessary copying if we first reverse then shift.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param startOffset the offset in self->output.array of the start of the buffer to shift right
 * @param type the header type
 * @return 0 for success, else an error code
 */
static ndn_Error reverseBufferAndInsertHeader
  (struct ndn_BinaryXmlEncoder *self, unsigned int startOffset, unsigned int type)
{
  unsigned int nBufferBytes = self->offset - startOffset;
  unsigned int nHeaderBytes = getNHeaderEncodingBytes(nBufferBytes);
  ndn_Error error;
  if ((error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + nHeaderBytes)))
    return error;
  
  // To reverse and shift at the same time, we first shift nHeaderBytes to the destination while reversing,
  //   then reverse the remaining bytes in place.
  unsigned char *from = self->output.array + startOffset;
  unsigned char *fromEnd = from + nHeaderBytes;
  unsigned char *to = self->output.array + startOffset + nBufferBytes + nHeaderBytes - 1;
  while (from < fromEnd)
    *(to--) = *(from++);
  // Reverse the remaining bytes in place (if any).
  if (nBufferBytes > nHeaderBytes)
    reverse(self->output.array + startOffset + nHeaderBytes, nBufferBytes - nHeaderBytes);
  
  // Override the offset to force encodeTypeAndValue to encode at startOffset, then fix the offset.
  self->offset = startOffset;
  if ((error = ndn_BinaryXmlEncoder_encodeTypeAndValue(self, ndn_BinaryXml_UDATA, nBufferBytes)))
    // We don't really expect to get an error, since we have already ensured the length.
    return error;
  self->offset = startOffset + nHeaderBytes + nBufferBytes;
  
  return NDN_ERROR_success;
}

/**
 * Split the absolute value of x into 32 bit unsigned integers hi32 and lo32.
 * We need this because not all C compilers support 64 bit long long integers, so we carry around
 * a high precision value as a double, which we assume has more than 32 bits.
 * But we want to do bit-wise operations on integers.
 * @param x the double value
 * @param hi32 output the high 32 bits
 * @param lo32 output the low 32 bits
 */
static inline void splitAbsDouble(double x, unsigned long *hi32, unsigned long *lo32)
{
  if (x < 0)
    x = -x;
  x = round(x);
  
  double twoPower32 = 4294967296.0;
  double lo32Double = fmod(x, twoPower32);
  *lo32 = (unsigned long)lo32Double;
  *hi32 = (unsigned long)((x - lo32Double) / twoPower32);
}

ndn_Error ndn_BinaryXmlEncoder_encodeTypeAndValue(struct ndn_BinaryXmlEncoder *self, unsigned int type, unsigned int value)
{
	if (type > ndn_BinaryXml_UDATA)
		return NDN_ERROR_header_type_is_out_of_range;
  
	// Encode backwards. Calculate how many bytes we need.
	unsigned int nEncodingBytes = getNHeaderEncodingBytes(value);
  ndn_Error error;
  if ((error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + nEncodingBytes)))
    return error;

	// Bottom 4 bits of value go in last byte with tag.
	self->output.array[self->offset + nEncodingBytes - 1] = 
		(ndn_BinaryXml_TT_MASK & type | 
		((ndn_BinaryXml_TT_VALUE_MASK & value) << ndn_BinaryXml_TT_BITS)) |
		ndn_BinaryXml_TT_FINAL; // set top bit for last byte
	value >>= ndn_BinaryXml_TT_VALUE_BITS;
	
	// Rest of value goes into preceding bytes, 7 bits per byte. (Zero top bit is "more" flag.)
	unsigned int i = self->offset + nEncodingBytes - 2;
	while (value != 0 && i >= self->offset) {
		self->output.array[i] = (value & ndn_BinaryXml_REGULAR_VALUE_MASK);
		value >>= ndn_BinaryXml_REGULAR_VALUE_BITS;
		--i;
	}
	if (value != 0)
    // This should not happen if getNHeaderEncodingBytes is correct.
		return NDN_ERROR_encodeTypeAndValue_miscalculated_N_encoding_bytes;
	
	self->offset+= nEncodingBytes;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlEncoder_writeElementClose(struct ndn_BinaryXmlEncoder *self)
{
  ndn_Error error;
  if ((error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + 1)))
    return error;
  
	self->output.array[self->offset] = ndn_BinaryXml_CLOSE;
	self->offset += 1;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlEncoder_writeBlob(struct ndn_BinaryXmlEncoder *self, unsigned char *value, unsigned int valueLength)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_encodeTypeAndValue(self, ndn_BinaryXml_BLOB, valueLength)))
    return error;
  
  if ((error = writeArray(self, value, valueLength)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlEncoder_writeBlobDTagElement(struct ndn_BinaryXmlEncoder *self, unsigned int tag, unsigned char *value, unsigned int valueLength)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(self, tag)))
    return error;
  
  if ((error = ndn_BinaryXmlEncoder_writeBlob(self, value, valueLength)))
    return error;  
  
  if ((error = ndn_BinaryXmlEncoder_writeElementClose(self)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlEncoder_writeUnsignedDecimalInt(struct ndn_BinaryXmlEncoder *self, unsigned int value)
{
  // First write the decimal int (to find out how many bytes it is), then shift it forward to make room for the header.
  unsigned int startOffset = self->offset;
  
  ndn_Error error;
  if ((error = encodeReversedUnsignedDecimalInt(self, value)))
    return error;
  
  if ((error = reverseBufferAndInsertHeader(self, startOffset, ndn_BinaryXml_UDATA)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlEncoder_writeUnsignedDecimalIntDTagElement(struct ndn_BinaryXmlEncoder *self, unsigned int tag, unsigned int value)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(self, tag)))
    return error;
  
  if ((error = ndn_BinaryXmlEncoder_writeUnsignedDecimalInt(self, value)))
    return error;  
  
  if ((error = ndn_BinaryXmlEncoder_writeElementClose(self)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlEncoder_writeAbsDoubleBigEndianBlob(struct ndn_BinaryXmlEncoder *self, double value)
{
  unsigned long hi32, lo32;
  splitAbsDouble(value, &hi32, &lo32);
  
  // First encode the big endian backwards, then reverseBufferAndInsertHeader will reverse it.
  unsigned int startOffset = self->offset;
  
  ndn_Error error;
  while (lo32 != 0) {
    if ((error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + 1)))
      return error;
    
    self->output.array[self->offset++] = (unsigned char)(lo32 & 0xff);
    lo32 >>= 8;
  }
  
  if (hi32 != 0) {
    // Pad the lo values out to 4 bytes.
    while (self->offset - startOffset < 4) {
      if ((error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + 1)))
        return error;
    
      self->output.array[self->offset++] = 0;
    }
    
    // Encode hi32
    while (hi32 != 0) {
      if ((error = ndn_DynamicUCharArray_ensureLength(&self->output, self->offset + 1)))
        return error;
    
      self->output.array[self->offset++] = (unsigned char)(hi32 & 0xff);
      hi32 >>= 8;
    }
  }
  
  if ((error = reverseBufferAndInsertHeader(self, startOffset, ndn_BinaryXml_BLOB)))
    return error;
  
  return NDN_ERROR_success;
}

ndn_Error ndn_BinaryXmlEncoder_writeTimeMillisecondsDTagElement(struct ndn_BinaryXmlEncoder *self, unsigned int tag, double milliseconds)
{
  ndn_Error error;
  if ((error = ndn_BinaryXmlEncoder_writeElementStartDTag(self, tag)))
    return error;
   
  if ((error = ndn_BinaryXmlEncoder_writeAbsDoubleBigEndianBlob(self, (milliseconds / 1000.0) * 4096.0)))
    return error;
    
  if ((error = ndn_BinaryXmlEncoder_writeElementClose(self)))
    return error;
  
  return NDN_ERROR_success;
}

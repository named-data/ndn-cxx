/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLDECODER_H
#define NDN_BINARYXMLDECODER_H

#include <ndn-cpp/c/common.h>
#include "../errors.h"
#include "../util/blob.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ndn_BinaryXmlDecoder {
  uint8_t *input;
  size_t inputLength;
  size_t offset;
  // peekDTag sets and checks these, and readElementStartDTag uses them to avoid reading again.
  size_t previouslyPeekedDTagStartOffset;
  size_t previouslyPeekedDTagEndOffset;
  unsigned int previouslyPeekedDTag;
};

static inline void ndn_BinaryXmlDecoder_initialize(struct ndn_BinaryXmlDecoder *self, uint8_t *input, size_t inputLength) 
{
  self->input = input;
  self->inputLength = inputLength;
  self->offset = 0;
  self->previouslyPeekedDTagStartOffset = (size_t)-1;
}

/**
 * Decode the header's type and value from self's input starting at offset. Update offset.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param type output for the header type
 * @param value output for the header value
 * @return 0 for success, else an error code for read past the end of the input or if the initial byte is zero
 */
ndn_Error ndn_BinaryXmlDecoder_decodeTypeAndValue(struct ndn_BinaryXmlDecoder *self, unsigned int *type, unsigned int *value);

/**
 * Decode the header from self's input starting at offset, expecting the type to be DTAG and the value to be expectedTag.
 * Update offset.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param expectedTag the expected value for DTAG
 * @return 0 for success, else an error code, including an error if not the expected tag
 */
ndn_Error ndn_BinaryXmlDecoder_readElementStartDTag(struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag);

/**
 * Read one byte from self's input starting at offset, expecting it to be the element close.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @return 0 for success, else an error code, including an error if not the element close
 */
ndn_Error ndn_BinaryXmlDecoder_readElementClose(struct ndn_BinaryXmlDecoder *self);

/**
 * Decode the header from self's input starting at offset, and if it is a DTAG where the value is the expectedTag,
 * then set gotExpectedTag to 1, else 0.  Do not update offset, including if returning an error.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param gotExpectedTag output a 1 if got the expected tag, else 0
 * @return 0 for success, else an error code for read past the end of the input
 */
ndn_Error ndn_BinaryXmlDecoder_peekDTag(struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, int *gotExpectedTag);

/**
 * Decode the header from self's input starting at offset, expecting the type to be DTAG and the value to be expectedTag.
 * Then read one item of any type (presumably BLOB, UDATA, TAG or ATTR) and return the item's value and length.
 * However, if allowNull is 1, then the item may be absent.
 * Finally, read the element close.  Update offset.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param allowNull 1 if the binary item may be missing
 * @param value output a pointer to the binary data inside self's input buffer. However, if allowNull is 1 and the
 * binary data item is absent, then set value and length to 0.
 * @return 0 for success, else an error code, including an error if not the expected tag, or if allowNull is 0
 * and the binary data is absent
 */
ndn_Error ndn_BinaryXmlDecoder_readBinaryDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, int allowNull, struct ndn_Blob *value);

/**
 * Peek at the next element and if it is the expectedTag, call ndn_BinaryXmlDecoder_readBinaryDTagElement.
 * Otherwise, set value and valueLength to 0.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param allowNull 1 if the binary item may be missing
 * @param value output a pointer to the binary data inside self's input buffer. However, if allowNull is 1 and the
 * binary data item is absent, then set value and length to 0.
 * @return 0 for success, else an error code, including if allowNull is 0 and the binary data is absent
 */
ndn_Error ndn_BinaryXmlDecoder_readOptionalBinaryDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, int allowNull, struct ndn_Blob *value);

/**
 * Decode the header from self's input starting at offset, expecting the type to be DTAG and the value to be expectedTag.
 * Then read one item expecting it to be type UDATA, and return the item's value and length.
 * Finally, read the element close.  Update offset.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param value output a pointer to the binary data inside self's input buffer.
 * @return 0 for success, else an error code, including an error if not the expected tag, or if the item is not UDATA.
 */
ndn_Error ndn_BinaryXmlDecoder_readUDataDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, struct ndn_Blob *value);

/**
 * Peek at the next element and if it is the expectedTag, call ndn_BinaryXmlDecoder_readUDataDTagElement.
 * Otherwise, set value and valueLength to 0.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param value output a pointer to the binary data inside self's input buffer. However, if allowNull is 1 and the
 * binary data item is absent, then set value and length to 0.
 * @return 0 for success, else an error code.
 */
ndn_Error ndn_BinaryXmlDecoder_readOptionalUDataDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, struct ndn_Blob *value);

/**
 * Decode the header from self's input starting at offset, expecting the type to be DTAG and the value to be expectedTag.
 * Then read one item expecting it to be type UDATA, parse it as an unsigned decimal integer and return the integer.
 * Finally, read the element close.  Update offset.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param value output the unsigned integer
 * @return 0 for success, else an error code, including an error if not the expected tag, or if the item is not UDATA,
 * or can't parse the integer
 */
ndn_Error ndn_BinaryXmlDecoder_readUnsignedIntegerDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, unsigned int *value);

/**
 * Peek at the next element, and if it has the expectedTag then call ndn_BinaryXmlDecoder_readUnsignedIntegerDTagElement.
 * Otherwise, set value to -1.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param value output the unsigned integer cast to int, or -1 if the next element doesn't have expectedTag.
 * @return 0 for success, else an error code, including an error if the item is not UDATA,
 * or can't parse the integer
 */
ndn_Error ndn_BinaryXmlDecoder_readOptionalUnsignedIntegerDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, int *value);

/**
 * Decode the header from self's input starting at offset, expecting the type to be DTAG and the value to be expectedTag.
 * Then read one item, parse it as an unsigned big endian integer in 4096 ticks per second, and convert it to milliseconds.
 * Finally, read the element close.  Update offset.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param milliseconds output the number of milliseconds
 * @return 0 for success, else an error code, including an error if not the expected tag
 */
ndn_Error ndn_BinaryXmlDecoder_readTimeMillisecondsDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, ndn_MillisecondsSince1970 *milliseconds);

/**
 * Peek at the next element, and if it has the expectedTag then call ndn_BinaryXmlDecoder_readTimeMillisecondsDTagElement.
 * Otherwise, set value to -1.0 .
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param milliseconds output the number of milliseconds, or -1.0 if the next element doesn't have expectedTag.
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlDecoder_readOptionalTimeMillisecondsDTagElement
  (struct ndn_BinaryXmlDecoder *self, unsigned int expectedTag, ndn_MillisecondsSince1970 *milliseconds);

/**
 * Interpret the bytes as an unsigned big endian integer and convert to a double. Don't check for overflow.
 * We use a double because it is large enough to represent NDN time (4096 ticks per second since 1970).
 * @param bytes pointer to the array of bytes
 * @param bytesLength the length of bytes
 * @return the result
 */
double ndn_BinaryXmlDecoder_unsignedBigEndianToDouble(uint8_t *bytes, size_t bytesLength); 

/**
 * Set the offset into the input, used for the next read.
 * @param self pointer to the ndn_BinaryXmlDecoder struct
 * @param offset the new offset
 */
static inline void ndn_BinaryXmlDecoder_seek(struct ndn_BinaryXmlDecoder *self, size_t offset) 
{
  self->offset = offset;
}

#ifdef __cplusplus
}
#endif

#endif

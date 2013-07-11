/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLDECODER_H
#define	NDN_BINARYXMLDECODER_H

#include "../errors.h"

#ifdef	__cplusplus
extern "C" {
#endif

struct ndn_BinaryXMLDecoder {
  unsigned char *input;
  unsigned int inputLength;
  unsigned int offset;
};

static inline void ndn_BinaryXMLDecoder_init(struct ndn_BinaryXMLDecoder *self, unsigned char *input, unsigned int inputLength) 
{
  self->input = input;
  self->inputLength = inputLength;
  self->offset = 0;
}

/**
 * Decode the header's type and value from self's input starting at offset. Update offset.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @param type output for the header type
 * @param value output for the header value
 * @return 0 for success, else an error code for read past the end of the input or if the initial byte is zero
 */
ndn_Error ndn_BinaryXMLDecoder_decodeTypeAndValue(struct ndn_BinaryXMLDecoder *self, unsigned int *type, unsigned int *value);

/**
 * Decode the header from self's input starting at offset, expecting the type to be DTAG and the value to be expectedTag.
 * Update offset.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @param expectedTag the expected value for DTAG
 * @return 0 for success, else an error code, including an error if not the expected tag
 */
ndn_Error ndn_BinaryXMLDecoder_readElementStartDTag(struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag);

/**
 * Read one byte from self's input starting at offset, expecting it to be the element close.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @return 0 for success, else an error code, including an error if not the element close
 */
ndn_Error ndn_BinaryXMLDecoder_readElementClose(struct ndn_BinaryXMLDecoder *self);

/**
 * Decode the header from self's input starting at offset, and if it is a DTAG where the value is the expectedTag,
 * then set gotExpectedTag to 1, else 0.  Do not update offset, including if returning an error.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param gotExpectedTag output a 1 if got the expected tag, else 0
 * @return 0 for success, else an error code for read past the end of the input
 */
ndn_Error ndn_BinaryXMLDecoder_peekDTag(struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, int *gotExpectedTag);

/**
 * Decode the header from self's input starting at offset, expecting the type to be DTAG and the value to be expectedTag.
 * Then read one item of any type (presumably BLOB, UDATA, TAG or ATTR) and return the item's value and length.
 * However, if allowNull is 1, then the item may be absent.
 * Finally, read the element close.  Update offset.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param allowNull 1 if the binary item may be missing
 * @param value output a pointer to the binary data inside self's input buffer. However, if allowNull is 1 and the
 * binary data item is absent, then return 0.
 * @param valueLength output the length of the binary data. However, if allowNull is 1 and the
 * binary data item is absent, then return 0.
 * @return 0 for success, else an error code, including an error if not the expected tag, or if allowNull is 0
 * and the binary data is absent
 */
ndn_Error ndn_BinaryXMLDecoder_readBinaryDTagElement
  (struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, int allowNull, unsigned char **value, unsigned int *valueLength);

/**
 * Peek at the next element and if it is the expectedTag, call ndn_BinaryXMLDecoder_readBinaryDTagElement.
 * Otherwise, set value and valueLength to 0.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param allowNull 1 if the binary item may be missing
 * @param value output a pointer to the binary data inside self's input buffer. However, if allowNull is 1 and the
 * binary data item is absent, then return 0.
 * @param valueLength output the length of the binary data. However, if allowNull is 1 and the
 * binary data item is absent, then return 0.
 * @return 0 for success, else an error code, including if allowNull is 0 and the binary data is absent
 */
ndn_Error ndn_BinaryXMLDecoder_readOptionalBinaryDTagElement
  (struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, int allowNull, unsigned char **value, unsigned int *valueLength);

/**
 * Decode the header from self's input starting at offset, expecting the type to be DTAG and the value to be expectedTag.
 * Then read one item expecting it to be type UDATA, and return the item's value and length.
 * Finally, read the element close.  Update offset.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param value output a pointer to the binary data inside self's input buffer.
 * @param valueLength output the length of the binary data.
 * @return 0 for success, else an error code, including an error if not the expected tag, or if the item is not UDATA.
 */
ndn_Error ndn_BinaryXMLDecoder_readUDataDTagElement
  (struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, unsigned char **value, unsigned int *valueLength);

/**
 * Decode the header from self's input starting at offset, expecting the type to be DTAG and the value to be expectedTag.
 * Then read one item expecting it to be type UDATA, parse it as an unsigned decimal integer and return the integer.
 * Finally, read the element close.  Update offset.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param value output the unsigned integer
 * @return 0 for success, else an error code, including an error if not the expected tag, or if the item is not UDATA,
 * or can't parse the integer
 */
ndn_Error ndn_BinaryXMLDecoder_readUnsignedIntegerDTagElement
  (struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, unsigned int *value);

/**
 * Peek at the next element, and if it has the expectedTag then call ndn_BinaryXMLDecoder_readUnsignedIntegerDTagElement.
 * Otherwise, set value to -1.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param value output the unsigned integer cast to int, or -1 if the next element doesn't have expectedTag.
 * @return 0 for success, else an error code, including an error if the item is not UDATA,
 * or can't parse the integer
 */
ndn_Error ndn_BinaryXMLDecoder_readOptionalUnsignedIntegerDTagElement
  (struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, int *value);

/**
 * Convert the big endian bytes to an unsigned int. Don't check for overflow.
 * @param bytes pointer to the array of bytes
 * @param bytesLength the length of bytes
 * @return the result unsigned int
 */
unsigned int ndn_BinaryXMLDecoder_bigEndianToUnsignedInt(unsigned char *bytes, unsigned int bytesLength); 

/**
 * Set the offset into the input, used for the next read.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @param offset the new offset
 */
static inline void ndn_BinaryXMLDecoder_seek(struct ndn_BinaryXMLDecoder *self, unsigned int offset) 
{
  self->offset = offset;
}

#ifdef	__cplusplus
}
#endif

#endif

/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef NDN_BINARYXMLDECODER_H
#define	NDN_BINARYXMLDECODER_H

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
 * @return 0 for success, else an error string for read past the end of the input or if the initial byte is zero
 */
char *ndn_BinaryXMLDecoder_decodeTypeAndValue(struct ndn_BinaryXMLDecoder *self, unsigned int *type, unsigned int *value);

/**
 * Decode the header from self's input starting at offset, expecting the type to be DTAG and the value to be expectedTag.
 * Update offset.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @param expectedTag the expected value for DTAG
 * @return 0 for success, else an error string, including an error if not the expected tag
 */
char *ndn_BinaryXMLDecoder_readDTag(struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag);

/**
 * Read one byte from self's input starting at offset, expecting it to be the element close.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @return 0 for success, else an error string, including an error if not the element close
 */
char *ndn_BinaryXMLDecoder_readElementClose(struct ndn_BinaryXMLDecoder *self);

/**
 * Decode the header from self's input starting at offset, and if it is a DTAG where the value is the expectedTag,
 * then set gotExpectedTag to 1, else 0.  Do not update offset, including if returning an error.
 * @param self pointer to the ndn_BinaryXMLDecoder struct
 * @param expectedTag the expected value for DTAG
 * @param gotExpectedTag output a 1 if got the expected tag, else 0
 * @return 0 for success, else an error string for read past the end of the input
 */
char *ndn_BinaryXMLDecoder_peekDTag(struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, int *gotExpectedTag);

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
 * @param valueLen output the length of the binary data. However, if allowNull is 1 and the
 * binary data item is absent, then return 0.
 * @return 0 for success, else an error string, including an error if not the expected tag, or if allowNull is 0
 * and the binary data is absent
 */
char *ndn_BinaryXMLDecoder_readBinaryDTagElement
  (struct ndn_BinaryXMLDecoder *self, unsigned int expectedTag, int allowNull, unsigned char **value, unsigned int *valueLen);

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

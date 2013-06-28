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

// Even though the first byte should not be zero, this silently ignores initial zeros.
char *ndn_BinaryXMLDecoder_decodeTypeAndValue(struct ndn_BinaryXMLDecoder *self, unsigned int *type, unsigned int *value);

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

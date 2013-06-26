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
  const unsigned char *input;
  unsigned int inputLen;
  unsigned int offset;
};

static void ndn_BinaryXMLDecoder_init(struct ndn_BinaryXMLDecoder *self, const unsigned char *input, unsigned int inputLen) {
  self->input = input;
  self->inputLen = inputLen;
  self->offset = 0;
}

// Even though the first byte should not be zero, this silently ignores initial zeros.
const char *ndn_BinaryXMLDecoder_decodeTypeAndValue(struct ndn_BinaryXMLDecoder *self, unsigned int *type, unsigned int *value);

#ifdef	__cplusplus
}
#endif

#endif

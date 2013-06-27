/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef NDN_BINARYXMLSTRUCTUREDECODER_H
#define	NDN_BINARYXMLSTRUCTUREDECODER_H

#ifdef	__cplusplus
extern "C" {
#endif

struct ndn_BinaryXMLStructureDecoder {
  int gotElementEnd; /**< boolean */
  unsigned int offset;
  int level;
  int state;
  unsigned int headerLength;
  int useHeaderBuffer; /**< boolean */
  // 10 bytes is enough to hold an encoded header with a type and a 64 bit value.
  unsigned char headerBuffer[10];
  int nBytesToRead;
};

enum {
  ndn_BinaryXMLStructureDecoder_READ_HEADER_OR_CLOSE,
  ndn_BinaryXMLStructureDecoder_READ_BYTES
};

void ndn_BinaryXMLStructureDecoder_init(struct ndn_BinaryXMLStructureDecoder *self);
        
/**
 * Continue scanning input starting from self->offset to find the element end.  On return, you must check
 *   self->gotElementEnd: If the end of the element which started at offset 0 is found, 
 *   then self->gotElementEnd is 1 and self->offset is the length of the element.  Otherwise, self-forElementEnd is 0 
 *   which means you should read more into input and call again.
 * @param self pointer to the ndn_BinaryXMLStructureDecoder struct
 * @param input the input buffer. You have to pass in input each time because the buffer could be reallocated.
 * @param inputLength the number of bytes in input.
 * @return 0 for success, else an error string
 */
const char *ndn_BinaryXMLStructureDecoder_findElementEnd
  (struct ndn_BinaryXMLStructureDecoder *self, const unsigned char *input, unsigned int inputLength);

#ifdef	__cplusplus
}
#endif

#endif

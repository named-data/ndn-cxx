/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLSTRUCTUREDECODER_H
#define NDN_BINARYXMLSTRUCTUREDECODER_H

#include "../errors.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ndn_BinaryXmlStructureDecoder {
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
  ndn_BinaryXmlStructureDecoder_READ_HEADER_OR_CLOSE,
  ndn_BinaryXmlStructureDecoder_READ_BYTES
};

void ndn_BinaryXmlStructureDecoder_initialize(struct ndn_BinaryXmlStructureDecoder *self);
        
/**
 * Continue scanning input starting from self->offset to find the element end.  On return, you must check
 *   self->gotElementEnd: If the end of the element which started at offset 0 is found, 
 *   then self->gotElementEnd is 1 and self->offset is the length of the element.  Otherwise, self-forElementEnd is 0 
 *   which means you should read more into input and call again.
 * @param self pointer to the ndn_BinaryXmlStructureDecoder struct
 * @param input the input buffer. You have to pass in input each time because the buffer could be reallocated.
 * @param inputLength the number of bytes in input.
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlStructureDecoder_findElementEnd
  (struct ndn_BinaryXmlStructureDecoder *self, unsigned char *input, unsigned int inputLength);

/**
 * Set the offset into the input, used for the next read.
 * @param self pointer to the ndn_BinaryXmlStructureDecoder struct
 * @param offset the new offset
 */
static inline void ndn_BinaryXmlStructureDecoder_seek(struct ndn_BinaryXmlStructureDecoder *self, unsigned int offset) 
{
  self->offset = offset;
}

#ifdef __cplusplus
}
#endif

#endif

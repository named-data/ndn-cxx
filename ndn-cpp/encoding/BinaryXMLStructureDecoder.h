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
  int gotElementEnd; // boolean
  unsigned int offset;
  int level;
  int state;
  unsigned int headerLength;
  int useHeaderBuffer; // boolean
  // 10 bytes is enough to hold an encoded header with a type and a 64 bit value.
  unsigned char headerBuffer[10];
  int nBytesToRead;
};

const int ndn_BinaryXMLStructureDecoder_READ_HEADER_OR_CLOSE = 0;
const int ndn_BinaryXMLStructureDecoder_READ_BYTES = 1;

void ndn_BinaryXMLStructureDecoder_init(struct ndn_BinaryXMLStructureDecoder *self);
        
#ifdef	__cplusplus
}
#endif

#endif

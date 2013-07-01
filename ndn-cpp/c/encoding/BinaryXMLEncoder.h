/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef NDN_BINARYXMLENCODER_H
#define	NDN_BINARYXMLENCODER_H

#include "../util/DynamicUCharArray.h"

#ifdef	__cplusplus
extern "C" {
#endif

struct ndn_BinaryXMLEncoder {
  struct ndn_DynamicUCharArray output; /**< receives the encoded output */
  unsigned int offset;             /**< the offset into output.array for the next encoding */
};

/**
 * Initialize an ndn_BinaryXMLEncoder_init struct with the arguments for initializing the ndn_DynamicUCharArray.
 * @param self pointer to the ndn_BinaryXMLEncoder struct
 * @param outputArray the allocated array buffer to receive the encoding
 * @param outputArrayLength the length of outputArray
 * @param reallocFunction the realloc function used by ndn_DynamicUCharArray_ensureLength.  If outputArrayLength
 * is large enough to receive the entire encoding, this can be 0.
 */
static inline void ndn_BinaryXMLEncoder_init
  (struct ndn_BinaryXMLEncoder *self, unsigned char *outputArray, unsigned int outputArrayLength, 
   unsigned char (*reallocFunction)(unsigned char *, unsigned int)) 
{
  ndn_DynamicUCharArray_init(&self->output, outputArray, outputArrayLength, reallocFunction);
  self->offset = 0;
}

char *ndn_BinaryXMLEncoder_encodeTypeAndValue(struct ndn_BinaryXMLEncoder *self, unsigned int type, unsigned int value);

#ifdef	__cplusplus
}
#endif

#endif


/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLENCODER_H
#define	NDN_BINARYXMLENCODER_H

#include "../errors.h"
#include "../util/DynamicUCharArray.h"
#include "BinaryXML.h"

#ifdef	__cplusplus
extern "C" {
#endif

/** An ndn_BinaryXMLEncoder struct is used by all the encoding functions.  You should initialize it with
 *  ndn_BinaryXMLEncoder_init.
 */
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
   unsigned char * (*reallocFunction)(unsigned char *, unsigned int)) 
{
  ndn_DynamicUCharArray_init(&self->output, outputArray, outputArrayLength, reallocFunction);
  self->offset = 0;
}

/**
 * Encode a header with the type and value and write it to self->output.
 * @param self pointer to the ndn_BinaryXMLEncoder struct
 * @param type the header type
 * @param value the header value
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXMLEncoder_encodeTypeAndValue(struct ndn_BinaryXMLEncoder *self, unsigned int type, unsigned int value);

/**
 * Write an element start header using DTAG with the tag to self->output.
 * @param self pointer to the ndn_BinaryXMLEncoder struct
 * @param tag the DTAG tag
 * @return 0 for success, else an error code
 */
static inline ndn_Error ndn_BinaryXMLEncoder_writeElementStartDTag(struct ndn_BinaryXMLEncoder *self, unsigned int tag) 
{
  return ndn_BinaryXMLEncoder_encodeTypeAndValue(self, ndn_BinaryXML_DTAG, tag);
}

/**
 * Write an element close to self->output.
 * @param self pointer to the ndn_BinaryXMLEncoder struct
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXMLEncoder_writeElementClose(struct ndn_BinaryXMLEncoder *self);

/**
 * Write a BLOB header, then the bytes of the blob value to self->output.
 * @param self pointer to the ndn_BinaryXMLEncoder struct
 * @param value an array of bytes for the blob value
 * @param valueLength the length of the array
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXMLEncoder_writeBlob(struct ndn_BinaryXMLEncoder *self, unsigned char *value, unsigned int valueLength);

/**
 * Write an element start header using DTAG with the tag to self->output, then the blob, then an element close.
 * (If you want to just write the blob, use ndn_BinaryXMLEncoder_writeBlob .)
 * @param self pointer to the ndn_BinaryXMLEncoder struct
 * @param tag the DTAG tag
 * @param value an array of bytes for the blob value
 * @param valueLength the length of the array
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXMLEncoder_writeBlobDTagElement(struct ndn_BinaryXMLEncoder *self, unsigned int tag, unsigned char *value, unsigned int valueLength);

#ifdef	__cplusplus
}
#endif

#endif


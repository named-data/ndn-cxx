/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLENCODER_H
#define NDN_BINARYXMLENCODER_H

#include "../errors.h"
#include "../util/dynamic-uchar-array.h"
#include "binary-xml.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An ndn_BinaryXmlEncoder struct is used by all the encoding functions.  You should initialize it with
 *  ndn_BinaryXmlEncoder_initialize.
 */
struct ndn_BinaryXmlEncoder {
  struct ndn_DynamicUCharArray *output; /**< A pointer to a ndn_DynamicUCharArray which receives the encoded output */
  unsigned int offset;             /**< the offset into output.array for the next encoding */
};

/**
 * Initialize an ndn_BinaryXmlEncoder_initialize struct with the arguments for initializing the ndn_DynamicUCharArray.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param output A pointer to a ndn_DynamicUCharArray struct which receives the encoded output.  The struct must
 * remain valid during the entire life of this ndn_BinaryXmlEncoder. If the output->realloc
 * function pointer is null, its array must be large enough to receive the entire encoding.
 */
static inline void ndn_BinaryXmlEncoder_initialize(struct ndn_BinaryXmlEncoder *self, struct ndn_DynamicUCharArray *output) 
{
  self->output = output;
  self->offset = 0;
}

/**
 * Encode a header with the type and value and write it to self->output.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param type the header type
 * @param value the header value
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlEncoder_encodeTypeAndValue(struct ndn_BinaryXmlEncoder *self, unsigned int type, unsigned int value);

/**
 * Write an element start header using DTAG with the tag to self->output.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param tag the DTAG tag
 * @return 0 for success, else an error code
 */
static inline ndn_Error ndn_BinaryXmlEncoder_writeElementStartDTag(struct ndn_BinaryXmlEncoder *self, unsigned int tag) 
{
  return ndn_BinaryXmlEncoder_encodeTypeAndValue(self, ndn_BinaryXml_DTAG, tag);
}

/**
 * Write an element close to self->output.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlEncoder_writeElementClose(struct ndn_BinaryXmlEncoder *self);

/**
 * Write a BLOB header, then the bytes of the blob value to self->output.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param value an array of bytes for the blob value
 * @param valueLength the length of the array
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlEncoder_writeBlob(struct ndn_BinaryXmlEncoder *self, unsigned char *value, unsigned int valueLength);

/**
 * Write an element start header using DTAG with the tag to self->output, then the blob, then an element close.
 * (If you want to just write the blob, use ndn_BinaryXmlEncoder_writeBlob .)
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param tag the DTAG tag
 * @param value an array of bytes for the blob value
 * @param valueLength the length of the array
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlEncoder_writeBlobDTagElement(struct ndn_BinaryXmlEncoder *self, unsigned int tag, unsigned char *value, unsigned int valueLength);

/**
 * If value or valueLen is 0 then do nothing, otherwise call ndn_BinaryXmlEncoder_writeBlobDTagElement.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param tag the DTAG tag
 * @param value an array of bytes for the blob value
 * @param valueLength the length of the array
 * @return 0 for success, else an error code
 */
static inline ndn_Error ndn_BinaryXmlEncoder_writeOptionalBlobDTagElement
  (struct ndn_BinaryXmlEncoder *self, unsigned int tag, unsigned char *value, unsigned int valueLength)
{
  if (value && valueLength > 0)
    return ndn_BinaryXmlEncoder_writeBlobDTagElement(self, tag, value, valueLength);
  else
    return NDN_ERROR_success;
}

/**
 * Write a UDATA header, then the bytes of the UDATA value to self->output.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param value an array of bytes for the value
 * @param valueLength the length of the array
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlEncoder_writeUData(struct ndn_BinaryXmlEncoder *self, unsigned char *value, unsigned int valueLength);

/**
 * Write an element start header using DTAG with the tag to self->output, then the UDATA value, then an element close.
 * (If you want to just write the UDATA value, use ndn_BinaryXmlEncoder_writeUData .)
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param tag the DTAG tag
 * @param value an array of bytes for the value
 * @param valueLength the length of the array
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlEncoder_writeUDataDTagElement(struct ndn_BinaryXmlEncoder *self, unsigned int tag, unsigned char *value, unsigned int valueLength);

/**
 * If value or valueLen is 0 then do nothing, otherwise call ndn_BinaryXmlEncoder_writeUDataDTagElement.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param tag the DTAG tag
 * @param value an array of bytes for the value
 * @param valueLength the length of the array
 * @return 0 for success, else an error code
 */
static inline ndn_Error ndn_BinaryXmlEncoder_writeOptionalUDataDTagElement
  (struct ndn_BinaryXmlEncoder *self, unsigned int tag, unsigned char *value, unsigned int valueLength)
{
  if (value && valueLength > 0)
    return ndn_BinaryXmlEncoder_writeUDataDTagElement(self, tag, value, valueLength);
  else
    return NDN_ERROR_success;
}

/**
 * Write a UDATA header, then the value as an unsigned decimal integer.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param value the unsigned int
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlEncoder_writeUnsignedDecimalInt(struct ndn_BinaryXmlEncoder *self, unsigned int value);

/**
 * Write an element start header using DTAG with the tag to self->output, then the value as an unsigned decimal integer, 
 * then an element close.
 * (If you want to just write the integer, use ndn_BinaryXmlEncoder_writeUnsignedDecimalInt .)
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param tag the DTAG tag
 * @param value the unsigned int
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlEncoder_writeUnsignedDecimalIntDTagElement(struct ndn_BinaryXmlEncoder *self, unsigned int tag, unsigned int value);

/**
 * If value is negative then do nothing, otherwise call ndn_BinaryXmlEncoder_writeUnsignedDecimalIntDTagElement.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param tag the DTAG tag
 * @param value negative for none, otherwise use (unsigned int)value
 * @return 0 for success, else an error code
 */
static inline ndn_Error ndn_BinaryXmlEncoder_writeOptionalUnsignedDecimalIntDTagElement(struct ndn_BinaryXmlEncoder *self, unsigned int tag, int value)
{
  if (value >= 0)
    return ndn_BinaryXmlEncoder_writeUnsignedDecimalIntDTagElement(self, tag, (unsigned int)value);
  else
    return NDN_ERROR_success;
}

/**
 * Write a BLOB header, then the absolute value of value, rounded to an integer, to self->output encoded as big endian.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param value the double to encode as big endian.  If value is 0, the big endian encoding has zero bytes.
 * The value is converted to absolute value.
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlEncoder_writeAbsDoubleBigEndianBlob(struct ndn_BinaryXmlEncoder *self, double value);

/**
 * Write an element start header using DTAG with the tag to self->output, then the absolute value of milliseconds
 * as a big endian BLOB converted to 4096 ticks per second, then an element close.
 * (If you want to just write the integer, use ndn_BinaryXmlEncoder_writeUnsignedDecimalInt .)
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param tag the DTAG tag
 * @param milliseconds the the number of milliseconds
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlEncoder_writeTimeMillisecondsDTagElement(struct ndn_BinaryXmlEncoder *self, unsigned int tag, double milliseconds);

/**
 * If milliseconds is negative then do nothing, otherwise call ndn_BinaryXmlEncoder_writeTimeMillisecondsDTagElement.
 * @param self pointer to the ndn_BinaryXmlEncoder struct
 * @param tag the DTAG tag
 * @param milliseconds negative for none, otherwise the number of milliseconds
 * @return 0 for success, else an error code
 */
static inline ndn_Error ndn_BinaryXmlEncoder_writeOptionalTimeMillisecondsDTagElement
  (struct ndn_BinaryXmlEncoder *self, unsigned int tag, double milliseconds)
{
  if (milliseconds >= 0)
    return ndn_BinaryXmlEncoder_writeTimeMillisecondsDTagElement(self, tag, milliseconds);
  else
    return NDN_ERROR_success;
}

#ifdef __cplusplus
}
#endif

#endif


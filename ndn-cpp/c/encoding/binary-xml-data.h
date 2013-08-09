/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARY_XML_DATA_H
#define NDN_BINARY_XML_DATA_H

#include "../errors.h"
#include "../data.h"
#include "binary-xml-encoder.h"
#include "binary-xml-decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encode the data as binary XML.
 * @param data Pointer to the data object the encode.
 * @param signedFieldsBeginOffset Return the offset in the encoding of the beginning of the fields which are signed.
 * If you are not encoding in order to sign, you can ignore this.
 * @param signedFieldsEndOffset Return the offset in the encoding of the end of the fields which are signed.
 * If you are not encoding in order to sign, you can ignore this.
 * @param encoder Pointer to the encoder object which receives the encoding. 
 * @return 
 */
ndn_Error ndn_encodeBinaryXmlData
  (struct ndn_Data *data, unsigned int *signedFieldsBeginOffset, unsigned int *signedFieldsEndOffset, struct ndn_BinaryXmlEncoder *encoder);

ndn_Error ndn_decodeBinaryXmlData(struct ndn_Data *data, struct ndn_BinaryXmlDecoder *decoder);

#ifdef __cplusplus
}
#endif

#endif

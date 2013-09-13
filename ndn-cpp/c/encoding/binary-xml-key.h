/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLKEY_H
#define NDN_BINARYXMLKEY_H

#include "../errors.h"
#include "../key.h"
#include "binary-xml-encoder.h"
#include "binary-xml-decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encode the ndn_KeyLocator struct using Binary XML.  If keyLocator->type is -1, then do nothing. 
 * @param keyLocator pointer to the ndn_KeyLocator struct
 * @param encoder pointer to the ndn_BinaryXmlEncoder struct
 * @return 0 for success, else an error code
 */
ndn_Error ndn_encodeBinaryXmlKeyLocator(struct ndn_KeyLocator *keyLocator, struct ndn_BinaryXmlEncoder *encoder);

/**
 * Expect the next element to be a Binary XML KeyLocator and decode into the ndn_KeyLocator struct.
 * @param keyLocator pointer to the ndn_KeyLocator struct
 * @param decoder pointer to the ndn_BinaryXmlDecoder struct
 * @return 0 for success, else an error code, including if the next element is not KeyLocator.
 */
ndn_Error ndn_decodeBinaryXmlKeyLocator(struct ndn_KeyLocator *keyLocator, struct ndn_BinaryXmlDecoder *decoder);

/**
 * Peek the next element and if it is a Binary XML KeyLocator and decode into the ndn_KeyLocator struct.
 * Otherwise, set the ndn_KeyLocator struct to none.
 * @param keyLocator pointer to the ndn_KeyLocator struct
 * @param decoder pointer to the ndn_BinaryXmlDecoder struct
 * @return 0 for success, else an error code, including if the next element is not KeyLocator.
 */
ndn_Error ndn_decodeOptionalBinaryXmlKeyLocator(struct ndn_KeyLocator *keyLocator, struct ndn_BinaryXmlDecoder *decoder);

#ifdef __cplusplus
}
#endif

#endif

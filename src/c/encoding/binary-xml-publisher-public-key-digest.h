/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLPUBLISHERPUBLICKEYDIGEST_H
#define NDN_BINARYXMLPUBLISHERPUBLICKEYDIGEST_H

#include "../errors.h"
#include "../publisher-public-key-digest.h"
#include "binary-xml-encoder.h"
#include "binary-xml-decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encode the ndn_PublisherPublicKeyDigest struct using Binary XML.  If publisherPublicKeyDigest->publisherPublicKeyDigest or
 * publisherPublicKeyDigestLength is 0, then do nothing. 
 * @param publisherPublicKeyDigest pointer to the ndn_PublisherPublicKeyDigest struct
 * @param encoder pointer to the ndn_BinaryXmlEncoder struct
 * @return 0 for success, else an error code
 */
ndn_Error ndn_encodeBinaryXmlPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXmlEncoder *encoder);

/**
 * Expect the next element to be a Binary XML PublisherPublicKeyDigest and decode into the ndn_PublisherPublicKeyDigest struct.
 * @param publisherPublicKeyDigest pointer to the ndn_PublisherPublicKeyDigest struct
 * @param decoder pointer to the ndn_BinaryXmlDecoder struct
 * @return 0 for success, else an error code, including if the next element is not PublisherPublicKeyDigest.
 */
ndn_Error ndn_decodeBinaryXmlPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXmlDecoder *decoder);

/**
 * Peek the next element and if it is a Binary XML PublisherPublicKeyDigest and decode into the ndn_PublisherPublicKeyDigest struct.
 * Otherwise, set the ndn_PublisherPublicKeyDigest struct to none.
 * @param publisherPublicKeyDigest pointer to the ndn_PublisherPublicKeyDigest struct
 * @param decoder pointer to the ndn_BinaryXmlDecoder struct
 * @return 0 for success, else an error code, including if the next element is not PublisherPublicKeyDigest.
 */
ndn_Error ndn_decodeOptionalBinaryXmlPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXmlDecoder *decoder);

#ifdef __cplusplus
}
#endif

#endif

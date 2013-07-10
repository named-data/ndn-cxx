/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLPUBLISHERPUBLICKEYDIGEST_H
#define	NDN_BINARYXMLPUBLISHERPUBLICKEYDIGEST_H

#include "../errors.h"
#include "../PublisherPublicKeyDigest.h"
#include "BinaryXMLEncoder.h"
#include "BinaryXMLDecoder.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Encode the ndn_PublisherPublicKeyDigest struct using Binary XML.  If publisherPublicKeyDigest->publisherPublicKeyDigest or
 * publisherPublicKeyDigestLength is 0, then do nothing. 
 * @param publisherPublicKeyDigest pointer to the ndn_PublisherPublicKeyDigest struct
 * @param encoder pointer to the ndn_BinaryXMLEncoder struct
 * @return 0 for success, else an error code
 */
ndn_Error ndn_encodeBinaryXMLPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXMLEncoder *encoder);

/**
 * Expect the next element to be a Binary XML PublisherPublicKeyDigest and decode into the ndn_PublisherPublicKeyDigest struct.
 * @param publisherPublicKeyDigest pointer to the ndn_PublisherPublicKeyDigest struct
 * @param decoder pointer to the ndn_BinaryXMLDecoder struct
 * @return 0 for success, else an error code, including if the next element is not PublisherPublicKeyDigest.
 */
ndn_Error ndn_decodeBinaryXMLPublisherPublicKeyDigest
  (struct ndn_PublisherPublicKeyDigest *publisherPublicKeyDigest, struct ndn_BinaryXMLDecoder *decoder);

#ifdef	__cplusplus
}
#endif

#endif

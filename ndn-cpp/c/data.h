/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DATA_H
#define NDN_DATA_H

#include "name.h"
#include "publisher-public-key-digest.h"
#include "key.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ndn_Signature {
  unsigned char *digestAlgorithm;      /**< pointer to pre-allocated buffer.  0 for none.
                                        *   If none, default is 2.16.840.1.101.3.4.2.1 (sha-256). */
  unsigned int digestAlgorithmLength;  /**< length of digestAlgorithm.  0 for none */
  unsigned char *witness;              /**< pointer to pre-allocated buffer.  0 for none. */
  unsigned int witnessLength;          /**< length of witness.  0 for none */
  unsigned char *signature;
  unsigned int signatureLength;
};

static inline void ndn_Signature_init(struct ndn_Signature *self) {
  self->digestAlgorithm = 0;
  self->digestAlgorithmLength = 0;
  self->witness = 0;
  self->witnessLength = 0;
  self->signature = 0;
  self->signatureLength = 0;
}

enum {
  ndn_ContentType_DATA = 0,
  ndn_ContentType_ENCR = 1,
  ndn_ContentType_GONE = 2,
  ndn_ContentType_KEY =  3,
  ndn_ContentType_LINK = 4,
  ndn_ContentType_NACK = 5
};

struct ndn_SignedInfo {
  struct ndn_PublisherPublicKeyDigest publisherPublicKeyDigest;
  double timestampMilliseconds;    /**< milliseconds since 1/1/1970. -1 for none */
  int type;                        /**< default is ndn_ContentType_DATA. -1 for none */
  int freshnessSeconds;            /**< -1 for none */
  unsigned char *finalBlockID;     /**< pointer to pre-allocated buffer.  0 for none */
  unsigned int finalBlockIDLength; /**< length of finalBlockID.  0 for none */
  struct ndn_KeyLocator keyLocator;
};

/**
 * Initialize the ndn_SignedInfo struct with values for none and the type to the default ndn_ContentType_DATA.
 * @param self pointer to the ndn_SignedInfo struct.
 */
static inline void ndn_SignedInfo_init(struct ndn_SignedInfo *self) {
  ndn_PublisherPublicKeyDigest_init(&self->publisherPublicKeyDigest);
  self->type = ndn_ContentType_DATA;
  self->freshnessSeconds = -1;
  self->finalBlockID = 0;
  self->finalBlockIDLength = 0;
  ndn_KeyLocator_init(&self->keyLocator);
}

struct ndn_Data {
  struct ndn_Signature signature;
  struct ndn_Name name;
  struct ndn_SignedInfo signedInfo;
  unsigned char *content;     /**< pointer to the content */
  unsigned int contentLength; /**< length of content */
};

/**
 * Initialize an ndn_Data struct with the pre-allocated nameComponents,
 * and defaults for all the values.
 * @param self pointer to the ndn_Data struct
 * @param nameComponents the pre-allocated array of ndn_NameComponent
 * @param maxNameComponents the number of elements in the allocated nameComponents array
 */
static inline void ndn_Data_init(struct ndn_Data *self, struct ndn_NameComponent *nameComponents, unsigned int maxNameComponents) 
{
  ndn_Signature_init(&self->signature);
  ndn_Name_init(&self->name, nameComponents, maxNameComponents);
  ndn_SignedInfo_init(&self->signedInfo);
  self->content = 0;
  self->contentLength = 0;
}

#ifdef __cplusplus
}
#endif

#endif

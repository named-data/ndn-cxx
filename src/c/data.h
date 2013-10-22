/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DATA_H
#define NDN_DATA_H

#include <ndn-cpp/c/data-types.h>
#include "name.h"
#include "publisher-public-key-digest.h"
#include "key.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An ndn_Signature struct holds the signature bits and other info representing the signature in a data packet.
 */
struct ndn_Signature {
  struct ndn_Blob digestAlgorithm; /**< A Blob whose value is a pointer to a pre-allocated buffer.  0 for none.
                                    *   If none, default is 2.16.840.1.101.3.4.2.1 (sha-256). */
  struct ndn_Blob witness;         /**< A Blob whose value is a pointer to pre-allocated buffer.  0 for none. */
  struct ndn_Blob signature;
  struct ndn_PublisherPublicKeyDigest publisherPublicKeyDigest;
  struct ndn_KeyLocator keyLocator;
};

/**
 * Initialize the ndn_Signature struct with values for none and the default digestAlgorithm.
 * @param self A pointer to the ndn_MetaInfo struct.
 * @param keyNameComponents The pre-allocated array of ndn_NameComponent for the keyLocator.
 * @param maxKeyNameComponents The number of elements in the allocated keyNameComponents array.
 */
static inline void ndn_Signature_initialize(struct ndn_Signature *self, struct ndn_NameComponent *keyNameComponents, size_t maxKeyNameComponents) {
  ndn_Blob_initialize(&self->digestAlgorithm, 0, 0);
  ndn_Blob_initialize(&self->witness, 0, 0);
  ndn_Blob_initialize(&self->signature, 0, 0);
  ndn_PublisherPublicKeyDigest_initialize(&self->publisherPublicKeyDigest);
  ndn_KeyLocator_initialize(&self->keyLocator, keyNameComponents, maxKeyNameComponents);
}

/**
 * An ndn_MetaInfo struct holds the meta info which is signed inside the data packet.
 */
struct ndn_MetaInfo {
  ndn_MillisecondsSince1970 timestampMilliseconds; /**< milliseconds since 1/1/1970. -1 for none */
  ndn_ContentType type;                  /**< default is ndn_ContentType_DATA. -1 for none */
  int freshnessSeconds;                  /**< -1 for none */
  struct ndn_NameComponent finalBlockID; /**< has a pointer to a pre-allocated buffer.  0 for none */
};

/**
 * Initialize the ndn_MetaInfo struct with values for none and the type to the default ndn_ContentType_DATA.
 * @param self A pointer to the ndn_MetaInfo struct.
 */
static inline void ndn_MetaInfo_initialize
  (struct ndn_MetaInfo *self) {
  self->type = ndn_ContentType_DATA;
  self->freshnessSeconds = -1;
  ndn_NameComponent_initialize(&self->finalBlockID, 0, 0);
}

struct ndn_Data {
  struct ndn_Signature signature;
  struct ndn_Name name;
  struct ndn_MetaInfo metaInfo;
  struct ndn_Blob content;     /**< A Blob with a pointer to the content. */
};

/**
 * Initialize an ndn_Data struct with the pre-allocated nameComponents and keyNameComponents,
 * and defaults for all the values.
 * @param self A pointer to the ndn_Data struct.
 * @param nameComponents The pre-allocated array of ndn_NameComponent.
 * @param maxNameComponents The number of elements in the allocated nameComponents array.
 * @param keyNameComponents The pre-allocated array of ndn_NameComponent for the signature.keyLocator.
 * @param maxKeyNameComponents The number of elements in the allocated keyNameComponents array.
 */
static inline void ndn_Data_initialize
  (struct ndn_Data *self, struct ndn_NameComponent *nameComponents, size_t maxNameComponents, 
   struct ndn_NameComponent *keyNameComponents, size_t maxKeyNameComponents) 
{
  ndn_Signature_initialize(&self->signature, keyNameComponents, maxKeyNameComponents);
  ndn_Name_initialize(&self->name, nameComponents, maxNameComponents);
  ndn_MetaInfo_initialize(&self->metaInfo);
  ndn_Blob_initialize(&self->content, 0, 0);
}

#ifdef __cplusplus
}
#endif

#endif

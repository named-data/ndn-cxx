/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FORWARDING_ENTRY_H
#define NDN_FORWARDING_ENTRY_H

#include "name.h"
#include "publisher-public-key-digest.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An ndn_ForwardingEntry holds fields for a ForwardingEntry which is used to register a prefix with a hub.
 */
struct ndn_ForwardingEntry {
  unsigned char *action;     /**< pointer to pre-allocated buffer.  0 for none. */
  unsigned int actionLength; /**< length of action.  0 for none. */
  struct ndn_Name prefix;
  struct ndn_PublisherPublicKeyDigest publisherPublicKeyDigest;
  int faceId;               /**< -1 for none. */
  int forwardingFlags;      /**< -1 for none. */
  int freshnessSeconds;     /**< -1 for none. */
};

/**
 * Initialize an ndn_ForwardingEntry struct with the pre-allocated prefixNameComponents,
 * and defaults for all the values.
 * @param self pointer to the ndn_Interest struct
 * @param prefixNameComponents the pre-allocated array of ndn_NameComponent
 * @param maxPrefixNameComponents the number of elements in the allocated prefixNameComponents array
 */
static inline void ndn_ForwardingEntry_initialize
  (struct ndn_ForwardingEntry *self, struct ndn_NameComponent *prefixNameComponents, unsigned int maxPrefixNameComponents) 
{
  self->action = 0;
  self->actionLength = 0;
  ndn_Name_initialize(&self->prefix, prefixNameComponents, maxPrefixNameComponents);
  ndn_PublisherPublicKeyDigest_initialize(&self->publisherPublicKeyDigest);
  self->faceId = -1;
  self->forwardingFlags = -1;
  self->freshnessSeconds = -1;
}

#ifdef __cplusplus
}
#endif

#endif

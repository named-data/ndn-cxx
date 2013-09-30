/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FORWARDING_ENTRY_H
#define NDN_FORWARDING_ENTRY_H

#include "common.h"
#include "name.h"
#include "publisher-public-key-digest.h"
#include "forwarding-flags.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ndn_ForwardingEntryFlags_ACTIVE         = 1,
  ndn_ForwardingEntryFlags_CHILD_INHERIT  = 2,
  ndn_ForwardingEntryFlags_ADVERTISE      = 4,
  ndn_ForwardingEntryFlags_LAST           = 8,
  ndn_ForwardingEntryFlags_CAPTURE       = 16,
  ndn_ForwardingEntryFlags_LOCAL         = 32,
  ndn_ForwardingEntryFlags_TAP           = 64,
  ndn_ForwardingEntryFlags_CAPTURE_OK   = 128
} ndn_ForwardingEntryFlags;

/**
 * An ndn_ForwardingEntry holds fields for a ForwardingEntry which is used to register a prefix with a hub.
 */
struct ndn_ForwardingEntry {
  uint8_t *action;     /**< pointer to pre-allocated buffer.  0 for none. */
  size_t actionLength; /**< length of action.  0 for none. */
  struct ndn_Name prefix;
  struct ndn_PublisherPublicKeyDigest publisherPublicKeyDigest;
  int faceId;               /**< -1 for none. */
  struct ndn_ForwardingFlags forwardingFlags;
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
  (struct ndn_ForwardingEntry *self, struct ndn_NameComponent *prefixNameComponents, size_t maxPrefixNameComponents) 
{
  self->action = 0;
  self->actionLength = 0;
  ndn_Name_initialize(&self->prefix, prefixNameComponents, maxPrefixNameComponents);
  ndn_PublisherPublicKeyDigest_initialize(&self->publisherPublicKeyDigest);
  self->faceId = -1;
  ndn_ForwardingFlags_initialize(&self->forwardingFlags);
  self->freshnessSeconds = -1;
}

#ifdef __cplusplus
}
#endif

#endif

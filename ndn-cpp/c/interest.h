/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_INTEREST_H
#define NDN_INTEREST_H

#include "name.h"
#include "publisher-public-key-digest.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ndn_Exclude_COMPONENT = 0,
  ndn_Exclude_ANY = 1  
} ndn_ExcludeType;

/**
 * An ndn_ExcludeEntry holds an ndn_ExcludeType, and if it is a COMPONENT, it holds a pointer to the component value.
 */
struct ndn_ExcludeEntry {
  ndn_ExcludeType type;
  struct ndn_NameComponent component;
};

/**
 * 
 * @param self pointer to the ndn_NameComponent struct
 * @param type one of the ndn_ExcludeType enum
 * @param component the pre-allocated buffer for the component value, only used if type is ndn_Exclude_COMPONENT
 * @param componentLength the number of bytes in value, only used if type is ndn_Exclude_COMPONENT
 */
static inline void ndn_ExcludeEntry_initialize(struct ndn_ExcludeEntry *self, ndn_ExcludeType type, uint8_t *component, size_t componentLength) 
{
  self->type = type;
  ndn_NameComponent_initialize(&self->component, component, componentLength);
}

/**
 * An ndn_Exclude holds an array of ndn_ExcludeEntry.
 */
struct ndn_Exclude {
  struct ndn_ExcludeEntry *entries;  /**< pointer to the array of entries. */
  size_t maxEntries;                 /**< the number of elements in the allocated entries array */
  size_t nEntries;                   /**< the number of entries in the exclude, 0 for no exclude */
};
/**
 * Initialize an ndn_Exclude struct with the entries array.
 * @param self A pointer to the ndn_Exclude struct.
 * @param entries the pre-allocated array of ndn_ExcludeEntry
 * @param maxEntries the number of elements in the allocated entries array
 */
static inline void ndn_Exclude_initialize(struct ndn_Exclude *self, struct ndn_ExcludeEntry *entries, size_t maxEntries) 
{
  self->entries = entries;
  self->maxEntries = maxEntries;
  self->nEntries = 0;
}

/**
 * Compare the components using NDN component ordering.
 * A component is less if it is shorter, otherwise if equal length do a byte comparison.
 * @param component1 A pointer to the first name component.
 * @param component2 A pointer to the second name component.
 * @return -1 if component1 is less than component2, 1 if greater or 0 if equal.
 */
int ndn_Exclude_compareComponents(struct ndn_NameComponent *component1, struct ndn_NameComponent *component2);

/**
 * Check if the component matches any of the exclude criteria.
 * @param self A pointer to the ndn_Exclude struct.
 * @param component A pointer to the name component to check.
 * @return 1 if the component matches any of the exclude criteria, otherwise 0.
 */
int ndn_Exclude_matches(struct ndn_Exclude *self, struct ndn_NameComponent *component);

enum {
  ndn_Interest_CHILD_SELECTOR_LEFT = 0,
  ndn_Interest_CHILD_SELECTOR_RIGHT = 1,

  ndn_Interest_ANSWER_NO_CONTENT_STORE = 0,
  ndn_Interest_ANSWER_CONTENT_STORE = 1,
  ndn_Interest_ANSWER_GENERATED = 2,
  ndn_Interest_ANSWER_STALE = 4,    // Stale answer OK
  ndn_Interest_MARK_STALE = 16,      // Must have scope 0.  Michael calls this a "hack"

  ndn_Interest_DEFAULT_ANSWER_ORIGIN_KIND = ndn_Interest_ANSWER_CONTENT_STORE | ndn_Interest_ANSWER_GENERATED
};

/**
 * An ndn_Interest holds an ndn_Name and other fields for an interest.
 */
struct ndn_Interest {
  struct ndn_Name name;
  int minSuffixComponents;  /**< -1 for none */
  int maxSuffixComponents;  /**< -1 for none */
  struct ndn_PublisherPublicKeyDigest publisherPublicKeyDigest;
  struct ndn_Exclude exclude;
  int childSelector;        /**< -1 for none */
  int answerOriginKind;     /**< -1 for none */
  int scope;                /**< -1 for none */
  double interestLifetimeMilliseconds; /**< milliseconds. -1.0 for none */
  uint8_t *nonce;           /**< pointer to pre-allocated buffer.  0 for none */
  size_t nonceLength;       /**< length of nonce.  0 for none */
};

/**
 * Initialize an ndn_Interest struct with the pre-allocated nameComponents and excludeEntries,
 * and defaults for all the values.
 * @param self pointer to the ndn_Interest struct
 * @param nameComponents the pre-allocated array of ndn_NameComponent
 * @param maxNameComponents the number of elements in the allocated nameComponents array
 * @param excludeEntries the pre-allocated array of ndn_ExcludeEntry
 * @param maxExcludeEntries the number of elements in the allocated excludeEntries array
 */
static inline void ndn_Interest_initialize
  (struct ndn_Interest *self, struct ndn_NameComponent *nameComponents, size_t maxNameComponents,
   struct ndn_ExcludeEntry *excludeEntries, size_t maxExcludeEntries) 
{
  ndn_Name_initialize(&self->name, nameComponents, maxNameComponents);
  self->minSuffixComponents = -1;
  self->maxSuffixComponents = -1;
  ndn_PublisherPublicKeyDigest_initialize(&self->publisherPublicKeyDigest);
  ndn_Exclude_initialize(&self->exclude, excludeEntries, maxExcludeEntries);
  self->childSelector = -1;
  self->answerOriginKind = -1;
  self->scope = -1;
  self->interestLifetimeMilliseconds = -1.0;
  self->nonce = 0;
  self->nonceLength = 0;
}

/**
 * Check if self's name matches the given name (using ndn_Name_match) and the given name also conforms to the 
 * interest selectors.
 * @param self A pointer to the ndn_Interest struct.
 * @param name A pointer to the name to check.
 * @return 1 if the name and interest selectors match, 0 otherwise.
 */
int ndn_Interest_matchesName(struct ndn_Interest *self, struct ndn_Name *name);

#ifdef __cplusplus
}
#endif

#endif


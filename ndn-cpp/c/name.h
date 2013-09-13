/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NAME_H
#define NDN_NAME_H

#ifdef __cplusplus
extern "C" {
#endif
  
/**
 * An ndn_NameComponent holds a pointer to the component value.
 */
struct ndn_NameComponent {
  unsigned char *value;     /**< pointer to the pre-allocated buffer for the component value */
  unsigned int valueLength; /**< the number of bytes in value */
};

/**
 * 
 * @param self pointer to the ndn_NameComponent struct
 * @param value the pre-allocated buffer for the component value
 * @param valueLength the number of bytes in value
 */
static inline void ndn_NameComponent_initialize(struct ndn_NameComponent *self, unsigned char *value, unsigned int valueLength) 
{
  self->value = value;
  self->valueLength = valueLength;
}
  
/**
 * An ndn_Name holds an array of ndn_NameComponent.
 */
struct ndn_Name {
  struct ndn_NameComponent *components; /**< pointer to the array of components. */
  unsigned int maxComponents;           /**< the number of elements in the allocated components array */
  unsigned int nComponents;             /**< the number of components in the name */
};

/**
 * Initialize an ndn_Name struct with the components array.
 * @param self pointer to the ndn_Name struct
 * @param components the pre-allocated array of ndn_NameComponent
 * @param maxComponents the number of elements in the allocated components array
 */
static inline void ndn_Name_initialize(struct ndn_Name *self, struct ndn_NameComponent *components, unsigned int maxComponents) 
{
  self->components = components;
  self->maxComponents = maxComponents;
  self->nComponents = 0;
}

/**
 * Return true if the N components of this name are the same as the first N components of the given name.
 * @param self A pointer to the ndn_Name struct.
 * @param name A pointer to the other name to match.
 * @return 1 if this matches the given name, 0 otherwise.  This always returns 1 if this name is empty.
 */
int ndn_Name_match(struct ndn_Name *self, struct ndn_Name *name);

#ifdef __cplusplus
}
#endif

#endif


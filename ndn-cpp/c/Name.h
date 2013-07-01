/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef NDN_NAME_H
#define	NDN_NAME_H

#ifdef	__cplusplus
extern "C" {
#endif
  
struct ndn_NameComponent {
  unsigned char *value;     /**< pointer to the component value */
  unsigned int valueLength; /**< the number of bytes in value */
};

static inline void ndn_NameComponent_init(struct ndn_NameComponent *self, unsigned char *value, unsigned int valueLength) 
{
  self->value = value;
  self->valueLength = valueLength;
}
  
struct ndn_Name {
  struct ndn_NameComponent *components; /**< pointer to the array of components. */
  unsigned int maxComponents;           /**< the number of elements in the allocated components array */
  unsigned int nComponents;             /**< the number of components in the name */
};

/**
 * Initialize an ndn_Name struct with the components array.
 * @param self pointer to the ndn_Name struct
 * @param components the array of ndn_NameComponent already allocated
 * @param maxComponents the number of elements in the allocated components array
 */
static inline void ndn_Name_init(struct ndn_Name *self, struct ndn_NameComponent *components, unsigned int maxComponents) 
{
  self->components = components;
  self->maxComponents = maxComponents;
  self->nComponents = 0;
}

#ifdef	__cplusplus
}
#endif

#endif


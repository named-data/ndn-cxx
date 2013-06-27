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
  unsigned char *value;
  unsigned int valueLength;
};

static inline void ndn_NameComponent_init(struct ndn_NameComponent *self, unsigned char *value, unsigned int valueLength) 
{
  self->value = value;
  self->valueLength = valueLength;
}

enum {
  ndn_Name_MAX_COMPONENTS = 100  
};
  
struct ndn_Name {
  struct ndn_NameComponent components[ndn_Name_MAX_COMPONENTS];
  unsigned int nComponents;
};

static inline void ndn_Name_init(struct ndn_Name *self) 
{
  self->nComponents = 0;
}

#ifdef	__cplusplus
}
#endif

#endif	/* NAME_H */


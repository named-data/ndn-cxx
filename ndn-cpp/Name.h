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
  
struct ndn_Component {
  unsigned char *value;
  unsigned int valueLen;
};

enum {
  ndn_Name_MAX_COMPONENTS = 100  
};
  
struct ndn_Name {
  struct ndn_Component components[ndn_Name_MAX_COMPONENTS];
  unsigned int nComponents;
};

void ndn_Name_init(struct ndn_Name *self);

#ifdef	__cplusplus
}
#endif

#endif	/* NAME_H */


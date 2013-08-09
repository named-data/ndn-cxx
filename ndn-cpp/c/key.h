/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_H
#define NDN_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ndn_KeyLocatorType_KEY = 1,
  ndn_KeyLocatorType_CERTIFICATE = 2,
  ndn_KeyLocatorType_KEYNAME = 3
} ndn_KeyLocatorType;
  
struct ndn_KeyLocator {
  ndn_KeyLocatorType type;         /**< -1 for none */
  unsigned char *keyOrCertificate; /**< if type is ndn_KeyLocatorType_KEY, pointer to the pre-allocated buffer for the key value.
                                        if type is ndn_KeyLocatorType_CERTIFICATE, pointer to the pre-allocated buffer for the cetrificate value. */
  unsigned int keyOrCertificateLength;
  // TODO: Implement keyName.
};

static inline void ndn_KeyLocator_init(struct ndn_KeyLocator *self) {
  self->type = (ndn_KeyLocatorType)-1;
  self->keyOrCertificate = 0;
  self->keyOrCertificateLength = 0;
  // TODO: Implement keyName.
}

#ifdef __cplusplus
}
#endif

#endif

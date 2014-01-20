/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_TYPES_H
#define NDN_KEY_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ndn_KeyLocatorType_KEY = 1,
  ndn_KeyLocatorType_CERTIFICATE = 2,
  ndn_KeyLocatorType_KEYNAME = 3
} ndn_KeyLocatorType;

typedef enum {
  ndn_KeyNameType_PUBLISHER_PUBLIC_KEY_DIGEST = 1,
  ndn_KeyNameType_PUBLISHER_CERTIFICATE_DIGEST = 2,
  ndn_KeyNameType_PUBLISHER_ISSUER_KEY_DIGEST = 3,
  ndn_KeyNameType_PUBLISHER_ISSUER_CERTIFICATE_DIGEST = 4
} ndn_KeyNameType;

#ifdef __cplusplus
}
#endif

#endif

/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DATA_TYPES_H
#define NDN_DATA_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ndn_ContentType_DATA = 0,
  ndn_ContentType_ENCR = 1,
  ndn_ContentType_GONE = 2,
  ndn_ContentType_KEY =  3,
  ndn_ContentType_LINK = 4,
  ndn_ContentType_NACK = 5
} ndn_ContentType;

#ifdef __cplusplus
}
#endif

#endif

/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_COMMON_H
#define NDN_COMMON_H

#include <ndn-cpp/ndn-cpp-config.h>
#include <stdint.h>
// TODO: Is stddef.h portable?
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A time interval represented as the number of milliseconds.
 */
typedef int64_t ndn_Milliseconds;
   
/**
 * The calendar time represented as the number of milliseconds since 1/1/1970.
 */
typedef int64_t ndn_MillisecondsSince1970;

#ifdef __cplusplus
}
#endif

#endif

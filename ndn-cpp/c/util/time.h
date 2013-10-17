/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TIME_H
#define NDN_TIME_H

#include <ndn-cpp/c/common.h>

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * Use gettimeofday to return the current time in milliseconds.
 * @return The current time in milliseconds since 1/1/1970, including fractions of a millisecond according to timeval.tv_usec.
 */
ndn_MillisecondsSince1970 
ndn_getNowMilliseconds();

#ifdef  __cplusplus
}
#endif

#endif

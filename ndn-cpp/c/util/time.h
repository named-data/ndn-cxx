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

/**
 * Convert the time from milliseconds to ISO time, for example "20131018T184138.423355".
 * @param milliseconds The time in milliseconds since 1/1/1970, including fractions of a millisecond.
 * @param isoString A buffer of at least 23 bytes to receive the null-terminated ISO time string.
 */
void
ndn_toIsoString(ndn_MillisecondsSince1970 time, char *isoString);

#ifdef  __cplusplus
}
#endif

#endif

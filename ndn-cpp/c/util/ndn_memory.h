/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

/*
 * Based on HAVE_MEMCPY and HAVE_MEMSET in config.h, use the library version or a local implementation of memcmp, memcpy and memset.
 */

#ifndef NDN_MEMORY_H
#define NDN_MEMORY_H

#include "../../../config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if HAVE_MEMCMP
#include <memory.h>
/**
 * Use the library version of memcmp.
 */
static inline int ndn_memcmp(unsigned char *buf1, unsigned char *buf2, unsigned int len) { return memcmp(buf1, buf2, len); }
#else
/**
 * Use a local implementation of memcmp instead of the library version.
 */
int ndn_memcmp(unsigned char *buf1, unsigned char *buf2, unsigned int len);
#endif

#if HAVE_MEMCPY
#include <memory.h>
/**
 * Use the library version of memcpy.
 */
static inline void ndn_memcpy(unsigned char *dest, unsigned char *src, unsigned int len) { memcpy(dest, src, len); }
#else
/**
 * Use a local implementation of memcpy instead of the library version.
 */
void ndn_memcpy(unsigned char *dest, unsigned char *src, unsigned int len);
#endif

#if HAVE_MEMSET
#include <memory.h>
/**
 * Use the library version of memset.
 */
static inline void ndn_memset(unsigned char *dest, int val, unsigned int len) { memset(dest, val, len); }
#else
/**
 * Use a local implementation of memset instead of the library version.
 */
void ndn_memset(unsigned char *dest, int val, unsigned int len);
#endif

#ifdef __cplusplus
}
#endif

#endif


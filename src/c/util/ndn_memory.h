/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

/*
 * Based on NDN_CPP_HAVE_MEMCPY and NDN_CPP_HAVE_MEMSET in ndn-cpp-config.h, use the library version or a local implementation of memcmp, memcpy and memset.
 */

#ifndef NDN_MEMORY_H
#define NDN_MEMORY_H

#include <ndn-cpp/c/common.h>

#ifdef __cplusplus
extern "C" {
#endif

#if NDN_CPP_HAVE_MEMCMP
#include <memory.h>
/**
 * Use the library version of memcmp.
 */
static inline int ndn_memcmp(const uint8_t *buf1, const uint8_t *buf2, size_t len) { return memcmp(buf1, buf2, len); }
#else
/**
 * Use a local implementation of memcmp instead of the library version.
 */
int ndn_memcmp(uint8_t *buf1, uint8_t *buf2, size_t len);
#endif

#if NDN_CPP_HAVE_MEMCPY
#include <memory.h>
/**
 * Use the library version of memcpy.
 */
static inline void ndn_memcpy(uint8_t *dest, const uint8_t *src, size_t len) { memcpy(dest, src, len); }
#else
/**
 * Use a local implementation of memcpy instead of the library version.
 */
void ndn_memcpy(uint8_t *dest, const uint8_t *src, size_t len);
#endif

#if NDN_CPP_HAVE_MEMSET
#include <memory.h>
/**
 * Use the library version of memset.
 */
static inline void ndn_memset(uint8_t *dest, int val, size_t len) { memset(dest, val, len); }
#else
/**
 * Use a local implementation of memset instead of the library version.
 */
void ndn_memset(uint8_t *dest, int val, size_t len);
#endif

#ifdef __cplusplus
}
#endif

#endif


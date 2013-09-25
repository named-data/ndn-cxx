/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_COMMON_H
#define	NDN_COMMON_H

#include "../../config.h"

// Need to define uint8_t.  There may be no stdint.h because it is only part of the C standard since 1999.
#if HAVE_STDINT
#include <stdint.h>
#else
#endif
#ifndef _UINT8_T
#define _UINT8_T
typedef unsigned char uint8_t;
#endif

#if HAVE_MEMCMP || HAVE_MEMCPY || HAVE_MEMSET
// size_t is defined in memory.h, or something it includes.
#include <memory.h>
#else
// Need to define size_t.
#ifndef	_SIZE_T
#define	_SIZE_T
typedef	unsigned int size_t;
#endif
#endif

#ifdef	__cplusplus
extern "C" {
#endif



#ifdef	__cplusplus
}
#endif

#endif

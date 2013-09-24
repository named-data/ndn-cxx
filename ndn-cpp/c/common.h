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

#ifdef	__cplusplus
extern "C" {
#endif



#ifdef	__cplusplus
}
#endif

#endif

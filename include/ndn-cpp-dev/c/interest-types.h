/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_INTEREST_TYPES_H
#define NDN_INTEREST_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ndn_Exclude_COMPONENT = 0,
  ndn_Exclude_ANY = 1  
} ndn_ExcludeType;

enum {
  ndn_Interest_CHILD_SELECTOR_LEFT = 0,
  ndn_Interest_CHILD_SELECTOR_RIGHT = 1,

  ndn_Interest_ANSWER_NO_CONTENT_STORE = 0,
  ndn_Interest_ANSWER_CONTENT_STORE = 1,
  ndn_Interest_ANSWER_GENERATED = 2,
  ndn_Interest_ANSWER_STALE = 4,    // Stale answer OK
  ndn_Interest_MARK_STALE = 16,      // Must have scope 0.  Michael calls this a "hack"

  ndn_Interest_DEFAULT_ANSWER_ORIGIN_KIND = ndn_Interest_ANSWER_CONTENT_STORE | ndn_Interest_ANSWER_GENERATED
};

#ifdef __cplusplus
}
#endif

#endif

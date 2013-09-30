/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FORWARDING_FLAGS_H
#define	NDN_FORWARDING_FLAGS_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * An ndn_ForwardingFlags object holds the flags which specify how the forwarding daemon should forward an interest for
 * a registered prefix.  We use a separate ForwardingFlags object to retain future compatibility if the daemon forwarding
 * bits are changed, amended or deprecated.
 */
struct ndn_ForwardingFlags {
  int active; /**< 1 if the flag is set, 0 if cleared. */
  int childInherit;
  int advertise;
  int last;
  int capture;
  int local;
  int tap;
  int captureOk;  
};

/**
 * Initialize an ndn_ForwardingFlags struct with the default with "active" and "childInherit" set and all other flags cleared.
 * @param self A pointer to the ndn_ForwardingFlags struct.
 */
void ndn_ForwardingFlags_initialize(struct ndn_ForwardingFlags *self);

/**
 * Get an integer with the bits set according to the flags as used by the ForwardingEntry message.
 * @param self A pointer to the ndn_ForwardingFlags struct.
 * @return An integer with the bits set.
 */
int ndn_ForwardingFlags_getForwardingEntryFlags(struct ndn_ForwardingFlags *self);

/**
 * Set the flags according to the bits in forwardingEntryFlags as used by the ForwardingEntry message.
 * @param self A pointer to the ndn_ForwardingFlags struct.
 * @param flags An integer with the bits set.
 */
void ndn_ForwardingFlags_setForwardingEntryFlags(struct ndn_ForwardingFlags *self, int forwardingEntryFlags);


#ifdef	__cplusplus
}
#endif

#endif

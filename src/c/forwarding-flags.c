/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "forwarding-entry.h"

void ndn_ForwardingFlags_initialize(struct ndn_ForwardingFlags *self)
{
  self->active = 1;
  self->childInherit = 1;
  self->advertise = 0;
  self->last = 0;
  self->capture = 0;
  self->local = 0;
  self->tap = 0;
  self->captureOk = 0;
}

int ndn_ForwardingFlags_getForwardingEntryFlags(struct ndn_ForwardingFlags *self)
{
  int result = 0;
  
  if (self->active)
    result |= ndn_ForwardingEntryFlags_ACTIVE;
  else if (self->childInherit)
    result |= ndn_ForwardingEntryFlags_CHILD_INHERIT;
  else if (self->advertise)
    result |= ndn_ForwardingEntryFlags_ADVERTISE;
  else if (self->last)
    result |= ndn_ForwardingEntryFlags_LAST;
  else if (self->capture)
    result |= ndn_ForwardingEntryFlags_CAPTURE;
  else if (self->local)
    result |= ndn_ForwardingEntryFlags_LOCAL;
  else if (self->tap)
    result |= ndn_ForwardingEntryFlags_TAP;
  else if (self->captureOk)
    result |= ndn_ForwardingEntryFlags_CAPTURE_OK;
  
  return result;
}

void ndn_ForwardingFlags_setForwardingEntryFlags(struct ndn_ForwardingFlags *self, int forwardingEntryFlags)
{
  self->active = (forwardingEntryFlags & ndn_ForwardingEntryFlags_ACTIVE) ? 1 : 0;
  self->childInherit = (forwardingEntryFlags & ndn_ForwardingEntryFlags_CHILD_INHERIT) ? 1 : 0;
  self->advertise = (forwardingEntryFlags & ndn_ForwardingEntryFlags_ADVERTISE) ? 1 : 0;
  self->last = (forwardingEntryFlags & ndn_ForwardingEntryFlags_LAST) ? 1 : 0;
  self->capture = (forwardingEntryFlags & ndn_ForwardingEntryFlags_CAPTURE) ? 1 : 0;
  self->local = (forwardingEntryFlags & ndn_ForwardingEntryFlags_LOCAL) ? 1 : 0;
  self->tap = (forwardingEntryFlags & ndn_ForwardingEntryFlags_TAP) ? 1 : 0;
  self->captureOk = (forwardingEntryFlags & ndn_ForwardingEntryFlags_CAPTURE_OK) ? 1 : 0;
}

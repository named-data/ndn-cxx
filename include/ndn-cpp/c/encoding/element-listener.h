/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_ELEMENT_LISTENER_H
#define NDN_ELEMENT_LISTENER_H

#include "../common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An ndn_ElementListener struct holds a function pointer onReceivedElement.  You can extend this struct with data that
 * will be passed to onReceivedElement.
 */
struct ndn_ElementListener {
  void (*onReceivedElement)(struct ndn_ElementListener *self, uint8_t *element, size_t elementLength); /**< see ndn_ElementListener_initialize */
};

/**
 * Initialize an ndn_ElementListener struct to use the onReceivedElement function pointer.
 * @param self pointer to the ndn_ElementListener struct
 * @param onReceivedElement pointer to a function which is called when an entire binary XML element is received.
 * self is the pointer to this ndn_ElementListener struct.  See ndn_BinaryXmlElementReader_onReceivedData.
 */
static inline void ndn_ElementListener_initialize
  (struct ndn_ElementListener *self, void (*onReceivedElement)(struct ndn_ElementListener *self, uint8_t *element, size_t elementLength))
{
  self->onReceivedElement = onReceivedElement;
}

#ifdef __cplusplus
}
#endif

#endif

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp-dev/encoding/element-listener.hpp>

namespace ndn {

void 
ElementListener::staticOnReceivedElement(struct ndn_ElementListener *self, uint8_t *element, size_t elementLength)
{
  ((ElementListener *)self)->onReceivedElement(element, elementLength);
}

}

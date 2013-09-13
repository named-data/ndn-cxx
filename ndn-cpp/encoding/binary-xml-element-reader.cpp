/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "binary-xml-element-reader.hpp"

namespace ndn {

void ElementListener::staticOnReceivedElement(struct ndn_ElementListener *self, unsigned char *element, unsigned int elementLength)
{
  ((ElementListener *)self)->onReceivedElement(element, elementLength);
}

}

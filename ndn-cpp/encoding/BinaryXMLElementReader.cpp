/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "BinaryXMLElementReader.hpp"

namespace ndn {

void ElementListener::staticOnReceivedElement(struct ndn_ElementListener *self, unsigned char *element, unsigned int elementLength)
{
  ((ElementListener *)self)->onReceivedElement(element, elementLength);
}

}

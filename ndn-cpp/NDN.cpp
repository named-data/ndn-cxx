/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "encoding/BinaryXMLDecoder.hpp"
#include "c/encoding/BinaryXML.h"
#include "ContentObject.hpp"
#include "NDN.hpp"

namespace ndn {

void NDN::onReceivedElement(unsigned char *element, unsigned int elementLength)
{
  BinaryXMLDecoder decoder(element, elementLength);
  
  if (decoder.peekDTag(ndn_BinaryXML_DTag_ContentObject)) {
    ContentObject contentObject;
    contentObject.decode(element, elementLength);

#if 0
    cout << "Got content with name " << contentObject.getName().to_uri() << endl;
    for (unsigned int i = 0; i < contentObject.getContent().size(); ++i)
      cout << contentObject.getContent()[i];
    cout << endl;
#endif
  }
}

}

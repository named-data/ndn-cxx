/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "encoding/BinaryXMLDecoder.hpp"
#include "c/encoding/BinaryXML.h"
#include "ContentObject.hpp"
#include "NDN.hpp"

using namespace std;

namespace ndn {

void NDN::onReceivedElement(unsigned char *element, unsigned int elementLength)
{
  BinaryXMLDecoder decoder(element, elementLength);
  
  if (decoder.peekDTag(ndn_BinaryXML_DTag_ContentObject)) {
    ptr_lib::shared_ptr<ContentObject> contentObject(new ContentObject());
    contentObject->decode(element, elementLength);
    
    ptr_lib::shared_ptr<Interest> dummyInterest;
    UpcallInfo upcallInfo(this, dummyInterest, 0, contentObject);
    closure_->upcall(UPCALL_CONTENT, upcallInfo);
  }
}

}

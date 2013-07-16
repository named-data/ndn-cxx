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

void NDN::expressInterest(const Name &name, const ptr_lib::shared_ptr<Closure> &closure, const Interest *interestTemplate)
{
  Interest interest(name);
  vector<unsigned char> encoding;
  interest.encode(encoding);  

  transport_->connect(*this);
  transport_->send(&encoding[0], encoding.size());
}
    
void NDN::onReceivedElement(unsigned char *element, unsigned int elementLength)
{
  BinaryXMLDecoder decoder(element, elementLength);
  
  if (decoder.peekDTag(ndn_BinaryXML_DTag_ContentObject)) {
    ptr_lib::shared_ptr<ContentObject> contentObject(new ContentObject());
    contentObject->decode(element, elementLength);
    
    ptr_lib::shared_ptr<Interest> dummyInterest;
    UpcallInfo upcallInfo(this, dummyInterest, 0, contentObject);
    tempClosure_->upcall(UPCALL_CONTENT, upcallInfo);
  }
}

}

/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "encoding/BinaryXMLDecoder.hpp"
#include "c/encoding/BinaryXML.h"
#include "ContentObject.hpp"
#include "NDN.hpp"

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

void NDN::expressInterest(const Name &name, const shared_ptr<Closure> &closure, const Interest *interestTemplate)
{
  Interest interest(name);
  shared_ptr<vector<unsigned char> > encoding = interest.encode();  

  // TODO: This should go in the PIT.
  tempClosure_ = closure;
  
  transport_->connect(*this);
  transport_->send(*encoding);
}
    
void NDN::onReceivedElement(unsigned char *element, unsigned int elementLength)
{
  BinaryXMLDecoder decoder(element, elementLength);
  
  if (decoder.peekDTag(ndn_BinaryXML_DTag_ContentObject)) {
    shared_ptr<ContentObject> contentObject(new ContentObject());
    contentObject->decode(element, elementLength);
    
    shared_ptr<Interest> dummyInterest;
    UpcallInfo upcallInfo(this, dummyInterest, 0, contentObject);
    tempClosure_->upcall(UPCALL_CONTENT, upcallInfo);
  }
}

}

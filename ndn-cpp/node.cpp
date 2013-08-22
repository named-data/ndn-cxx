/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <sys/time.h>
#include "encoding/binary-xml-decoder.hpp"
#include "c/encoding/binary-xml.h"
#include "data.hpp"
#include "node.hpp"

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

// Use gettimeofday to return the current time in milliseconds.
static inline double getNowMilliseconds()
{
  timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}
  
Node::PitEntry::PitEntry(const ptr_lib::shared_ptr<const Interest> &interest, Closure *closure)
: interest_(interest), closure_(closure)
{
  // Set up timeoutTime_.
  if (interest_->getInterestLifetimeMilliseconds() >= 0.0)
    timeoutTimeMilliseconds_ = getNowMilliseconds() + interest_->getInterestLifetimeMilliseconds();
  else
    // No timeout.
    timeoutTimeMilliseconds_ = -1.0;
  
  // Set up interestStruct_.
  // TODO: Doesn't this belong in the Interest class?
  nameComponents_.reserve(interest_->getName().getComponentCount());
  excludeEntries_.reserve(interest_->getExclude().getEntryCount());
  ndn_Interest_init
    (&interestStruct_, &nameComponents_[0], nameComponents_.capacity(), &excludeEntries_[0], excludeEntries_.capacity());
  interest_->get(interestStruct_);  
}

bool Node::PitEntry::checkTimeout(Node *parent, double nowMilliseconds)
{
  if (timeoutTimeMilliseconds_ >= 0.0 && nowMilliseconds >= timeoutTimeMilliseconds_) {
    shared_ptr<Data> dummyData;
    UpcallInfo upcallInfo(parent, interest_, 0, dummyData);
    
    // Ignore all exceptions.
    try {
      closure_->upcall(UPCALL_INTEREST_TIMED_OUT, upcallInfo);
    }
    catch (...) { }
    
    return true;
  }
  else
    return false;
}

void Node::expressInterest(const Name &name, Closure *closure, const Interest *interestTemplate)
{
  shared_ptr<const Interest> interest;
  if (interestTemplate)
    interest.reset(new Interest
      (name, interestTemplate->getMinSuffixComponents(), interestTemplate->getMaxSuffixComponents(),
       interestTemplate->getPublisherPublicKeyDigest(), interestTemplate->getExclude(),
       interestTemplate->getChildSelector(), interestTemplate->getAnswerOriginKind(),
       interestTemplate->getScope(), interestTemplate->getInterestLifetimeMilliseconds()));
  else
    interest.reset(new Interest(name, 4000.0));
  
  shared_ptr<PitEntry> pitEntry(new PitEntry(interest, closure));
  pit_.push_back(pitEntry);
  
  shared_ptr<vector<unsigned char> > encoding = pitEntry->getInterest()->wireEncode();  
  
  // TODO: Properly check if we are already connected to the expected host.
  if (!transport_->getIsConnected())
    transport_->connect(*this);
  
  transport_->send(*encoding);
}

void Node::processEvents()
{
  transport_->processEvents();
  
  // Check for PIT entry timeouts.  Go backwards through the list so we can erase entries.
  double nowMilliseconds = getNowMilliseconds();
  for (int i = (int)pit_.size() - 1; i >= 0; --i) {
    if (pit_[i]->checkTimeout(this, nowMilliseconds)) {
      pit_.erase(pit_.begin() + i);
      
      // Refresh now since the timeout callback might have delayed.
      nowMilliseconds = getNowMilliseconds();
    }
  }
}

void Node::onReceivedElement(const unsigned char *element, unsigned int elementLength)
{
  BinaryXmlDecoder decoder(element, elementLength);
  
  if (decoder.peekDTag(ndn_BinaryXml_DTag_ContentObject)) {
    shared_ptr<Data> data(new Data());
    data->wireDecode(element, elementLength);
    
    int iPitEntry = getEntryIndexForExpressedInterest(data->getName());
    if (iPitEntry >= 0) {
      UpcallInfo upcallInfo(this, pit_[iPitEntry]->getInterest(), 0, data);
      
      // Remove the PIT entry before the calling the callback.
      Closure *closure = pit_[iPitEntry]->getClosure();
      pit_.erase(pit_.begin() + iPitEntry);
      closure->upcall(UPCALL_DATA, upcallInfo);
    }
  }
}

void Node::shutdown()
{
  transport_->close();
}

int Node::getEntryIndexForExpressedInterest(const Name &name)
{
  // TODO: Doesn't this belong in the Name class?
  vector<struct ndn_NameComponent> nameComponents;
  nameComponents.reserve(name.getComponentCount());
  struct ndn_Name nameStruct;
  ndn_Name_init(&nameStruct, &nameComponents[0], nameComponents.capacity());
  name.get(nameStruct);
  
  int iResult = -1;
    
	for (unsigned int i = 0; i < pit_.size(); ++i) {
		if (ndn_Interest_matchesName((struct ndn_Interest *)&pit_[i]->getInterestStruct(), &nameStruct)) {
      if (iResult < 0 || 
          pit_[i]->getInterestStruct().name.nComponents > pit_[iResult]->getInterestStruct().name.nComponents)
        // Update to the longer match.
        iResult = i;
    }
	}
    
	return iResult;
}

}

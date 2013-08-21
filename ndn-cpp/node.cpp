/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "encoding/binary-xml-decoder.hpp"
#include "c/encoding/binary-xml.h"
#include "data.hpp"
#include "Node.hpp"

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

Node::PitEntry::PitEntry(const Name &name, Closure *closure)
: interest_(name), interestStructIsStale_(true), closure_(closure)
{
  if (interest_.getInterestLifetimeMilliseconds() >= 0.0)
    timeoutTime_ = ::clock() + (clock_t)((interest_.getInterestLifetimeMilliseconds() / 1000.0) * (double)CLOCKS_PER_SEC);
  else
    // No timeout.
    timeoutTime_ = 0;
}

const struct ndn_Interest &Node::PitEntry::getInterestStruct()
{
  if (interestStructIsStale_) {
    nameComponents_.reserve(interest_.getName().getComponentCount());
    excludeEntries_.reserve(interest_.getExclude().getEntryCount());
    ndn_Interest_init
      (&interestStruct_, &nameComponents_[0], nameComponents_.capacity(), &excludeEntries_[0], excludeEntries_.capacity());
    interest_.get(interestStruct_);
    
    interestStructIsStale_ = false;
  }
  
  return interestStruct_;
}

void Node::expressInterest(const Name &name, Closure *closure, const Interest *interestTemplate)
{
  shared_ptr<PitEntry> pitEntry(new PitEntry(name, closure));
  if (interestTemplate) {
		pitEntry->getInterest().setMinSuffixComponents(interestTemplate->getMinSuffixComponents());
		pitEntry->getInterest().setMaxSuffixComponents(interestTemplate->getMaxSuffixComponents());
		pitEntry->getInterest().getPublisherPublicKeyDigest() = interestTemplate->getPublisherPublicKeyDigest();
		pitEntry->getInterest().getExclude() = interestTemplate->getExclude();
		pitEntry->getInterest().setChildSelector(interestTemplate->getChildSelector());
		pitEntry->getInterest().setAnswerOriginKind(interestTemplate->getAnswerOriginKind());
		pitEntry->getInterest().setScope(interestTemplate->getScope());
		pitEntry->getInterest().setInterestLifetimeMilliseconds(interestTemplate->getInterestLifetimeMilliseconds());
  }
  else
    pitEntry->getInterest().setInterestLifetimeMilliseconds(4000.0);   // default interest timeout value.
  
  pit_.push_back(pitEntry);
  
  shared_ptr<vector<unsigned char> > encoding = pitEntry->getInterest().wireEncode();  
  
  // TODO: Check if we are already connected.
  transport_->connect(*this);
  transport_->send(*encoding);
}

void Node::processEvents()
{
  transport_->processEvents();
}

void Node::onReceivedElement(unsigned char *element, unsigned int elementLength)
{
  BinaryXmlDecoder decoder(element, elementLength);
  
  if (decoder.peekDTag(ndn_BinaryXml_DTag_ContentObject)) {
    shared_ptr<Data> data(new Data());
    data->wireDecode(element, elementLength);
    
    int iPitEntry = getEntryIndexForExpressedInterest(data->getName());
    if (iPitEntry >= 0) {
      shared_ptr<Interest> interestCopy(new Interest(pit_[iPitEntry]->getInterest()));
      UpcallInfo upcallInfo(this, interestCopy, 0, data);
      
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
          pit_[i]->getInterest().getName().getComponentCount() > pit_[iResult]->getInterest().getName().getComponentCount())
        iResult = i;
    }
	}
    
	return iResult;
}

}

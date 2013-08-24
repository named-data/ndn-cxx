/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <sys/time.h>
#include "encoding/binary-xml-decoder.hpp"
#include "c/encoding/binary-xml.h"
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

void Node::construct()
{
  ndndIdFetcherInterest_ = Interest(Name("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY"), 4000.0);
}

void Node::expressInterest(const Interest &interest, const OnData &onData, const OnTimeout &onTimeout)
{
  // TODO: Properly check if we are already connected to the expected host.
  if (!transport_->getIsConnected())
    transport_->connect(*connectionInfo_, *this);
  
  shared_ptr<PitEntry> pitEntry(new PitEntry(shared_ptr<const Interest>(new Interest(interest)), onData, onTimeout));
  pit_.push_back(pitEntry);
  
  shared_ptr<vector<unsigned char> > encoding = pitEntry->getInterest()->wireEncode();  
  
  transport_->send(*encoding);
}

void Node::expressInterest(const Name &name, const Interest *interestTemplate, const OnData &onData, const OnTimeout &onTimeout)
{
  if (interestTemplate)
    expressInterest(Interest
      (name, interestTemplate->getMinSuffixComponents(), interestTemplate->getMaxSuffixComponents(),
       interestTemplate->getPublisherPublicKeyDigest(), interestTemplate->getExclude(),
       interestTemplate->getChildSelector(), interestTemplate->getAnswerOriginKind(),
       interestTemplate->getScope(), interestTemplate->getInterestLifetimeMilliseconds()), onData, onTimeout);
  else
    expressInterest(Interest(name, 4000.0), onData, onTimeout);
}

void Node::registerPrefix(const Name &prefix, const OnInterest &onInterest, int flags)
{
  if (ndndId_.size() == 0) {
    // First fetch the ndndId of the connected hub.
    NdndIdFetcher fetcher(make_shared<NdndIdFetcher::Info>(this, prefix, onInterest, flags));
    // It is OK for func_lib::function make a copy of the function object because the Info is in a shared_ptr.
    expressInterest(ndndIdFetcherInterest_, fetcher, fetcher);
  }
  else
    registerPrefixHelper(prefix, onInterest, flags);
}

void Node::NdndIdFetcher::operator()(const ptr_lib::shared_ptr<const Interest> &interest, const ptr_lib::shared_ptr<Data> &ndndIdData)
{
  if (ndndIdData->getSignedInfo().getPublisherPublicKeyDigest().getPublisherPublicKeyDigest().size() > 0) {
    // Set the ndndId_ and continue.
    info_->node_.ndndId_ = ndndIdData->getSignedInfo().getPublisherPublicKeyDigest().getPublisherPublicKeyDigest();
    info_->node_.registerPrefixHelper(info_->prefix_, info_->onInterest_, info_->flags_);
  }
  // TODO: else need to log not getting the ndndId.
}

void Node::NdndIdFetcher::operator()(const ptr_lib::shared_ptr<const Interest> &timedOutInterest)
{
  // TODO: Log the timeout.
}

void Node::registerPrefixHelper(const Name &prefix, const OnInterest &onInterest, int flags)
{
  throw logic_error("need to finish implementing registerPrefix");
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
      // Copy pointers to the needed objects and remove the PIT entry before the calling the callback.
      const OnData onData = pit_[iPitEntry]->getOnData();
      const ptr_lib::shared_ptr<const Interest> interest = pit_[iPitEntry]->getInterest();
      pit_.erase(pit_.begin() + iPitEntry);
      onData(interest, data);
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
  
Node::PitEntry::PitEntry(const ptr_lib::shared_ptr<const Interest> &interest, const OnData &onData, const OnTimeout &onTimeout)
: interest_(interest), onData_(onData), onTimeout_(onTimeout)
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
    if (onTimeout_) {
      // Ignore all exceptions.
      try {
        onTimeout_(interest_);
      }
      catch (...) { }
    }
    
    return true;
  }
  else
    return false;
}

}

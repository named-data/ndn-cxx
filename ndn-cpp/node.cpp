/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <sys/time.h>
#include "encoding/binary-xml-decoder.hpp"
#include "c/encoding/binary-xml.h"
#include "forwarding-entry.hpp"
#include "security/key-chain.hpp"
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

Node::Node(const ptr_lib::shared_ptr<Transport>& transport, const ptr_lib::shared_ptr<const Transport::ConnectionInfo>& connectionInfo)
: transport_(transport), connectionInfo_(connectionInfo),
  ndndIdFetcherInterest_(Name("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY"), 4000.0)
{
}

void Node::expressInterest(const Interest& interest, const OnData& onData, const OnTimeout& onTimeout)
{
  // TODO: Properly check if we are already connected to the expected host.
  if (!transport_->getIsConnected())
    transport_->connect(*connectionInfo_, *this);
  
  pit_.push_back(shared_ptr<PitEntry>(new PitEntry(shared_ptr<const Interest>(new Interest(interest)), onData, onTimeout)));
  
  shared_ptr<vector<unsigned char> > encoding = interest.wireEncode();  
  transport_->send(*encoding);
}

void Node::registerPrefix(const Name& prefix, const OnInterest& onInterest, int flags)
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

void Node::NdndIdFetcher::operator()(const ptr_lib::shared_ptr<const Interest>& interest, const ptr_lib::shared_ptr<Data>& ndndIdData)
{
  if (ndndIdData->getMetaInfo().getPublisherPublicKeyDigest().getPublisherPublicKeyDigest().size() > 0) {
    // Set the ndndId_ and continue.
    // TODO: If there are multiple connected hubs, the NDN ID is really stored per connected hub.
    info_->node_.ndndId_ = ndndIdData->getMetaInfo().getPublisherPublicKeyDigest().getPublisherPublicKeyDigest();
    info_->node_.registerPrefixHelper(info_->prefix_, info_->onInterest_, info_->flags_);
  }
  // TODO: else need to log not getting the ndndId.
}

void Node::NdndIdFetcher::operator()(const ptr_lib::shared_ptr<const Interest>& timedOutInterest)
{
  // TODO: Log the timeout.
}

void Node::registerPrefixHelper(const Name& prefix, const OnInterest& onInterest, int flags)
{
  // Create a ForwardingEntry.
  ForwardingEntry forwardingEntry("selfreg", prefix, PublisherPublicKeyDigest(), -1, 3, 2147483647);
  ptr_lib::shared_ptr<vector<unsigned char> > content = forwardingEntry.wireEncode();

  // Set the ForwardingEntry as the content of a Data packet and sign.
  Data data;
  data.setContent(*content);
  data.getMetaInfo().setTimestampMilliseconds(time(NULL) * 1000.0);
  // TODO: Should we sign with a different key?
  KeyChain::defaultSign(data);
  ptr_lib::shared_ptr<vector<unsigned char> > encodedData = data.wireEncode();
  
  // Create an interest where the name has the encoded Data packet.
  Name interestName;
  const unsigned char component0[] = "ndnx";
  const unsigned char component2[] = "selfreg";
  interestName.addComponent(component0, sizeof(component0) - 1);
  interestName.addComponent(ndndId_);
  interestName.addComponent(component2, sizeof(component2) - 1);
  interestName.addComponent(*encodedData);
  
  Interest interest(interestName);
  interest.setScope(1);
  ptr_lib::shared_ptr<vector<unsigned char> > encodedInterest = interest.wireEncode();
  
  // Save the onInterest callback and send the registration interest.
  registeredPrefixTable_.push_back(shared_ptr<PrefixEntry>(new PrefixEntry(shared_ptr<const Name>(new Name(prefix)), onInterest)));
  
  transport_->send(*encodedInterest);
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
  
  if (decoder.peekDTag(ndn_BinaryXml_DTag_Interest)) {
    shared_ptr<Interest> interest(new Interest());
    interest->wireDecode(element, elementLength);
    
    PrefixEntry *entry = getEntryForRegisteredPrefix(interest->getName());
    if (entry)
      entry->getOnInterest()(entry->getPrefix(), interest, *transport_);
  }
  else if (decoder.peekDTag(ndn_BinaryXml_DTag_ContentObject)) {
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

int Node::getEntryIndexForExpressedInterest(const Name& name)
{
  // TODO: Doesn't this belong in the Name class?
  vector<struct ndn_NameComponent> nameComponents;
  nameComponents.reserve(name.getComponentCount());
  struct ndn_Name nameStruct;
  ndn_Name_initialize(&nameStruct, &nameComponents[0], nameComponents.capacity());
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
  
Node::PrefixEntry *Node::getEntryForRegisteredPrefix(const Name& name)
{
  int iResult = -1;
    
	for (unsigned int i = 0; i < registeredPrefixTable_.size(); ++i) {
		if (registeredPrefixTable_[i]->getPrefix()->match(name)) {
      if (iResult < 0 || 
          registeredPrefixTable_[i]->getPrefix()->getComponentCount() > registeredPrefixTable_[iResult]->getPrefix()->getComponentCount())
        // Update to the longer match.
        iResult = i;
    }
	}
    
  if (iResult >= 0)
  	return registeredPrefixTable_[iResult].get();
  else
    return 0;
}

Node::PitEntry::PitEntry(const ptr_lib::shared_ptr<const Interest>& interest, const OnData& onData, const OnTimeout& onTimeout)
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
  ndn_Interest_initialize
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

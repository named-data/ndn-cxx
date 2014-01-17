/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "c/util/time.h"

#include <ndn-cpp/forwarding-entry.hpp>
#include <ndn-cpp/face-instance.hpp>
#include <ndn-cpp/node.hpp>

#include "util/ndnd-id-fetcher.hpp"

#include <ndn-cpp/security/signature-sha256-with-rsa.hpp>
#include <ndn-cpp/status-response.hpp>

using namespace std;
#if NDN_CPP_HAVE_CXX11
// In the std library, the placeholders are in a different namespace than boost.
using namespace ndn::func_lib::placeholders;
#endif

namespace ndn {

uint64_t Node::PendingInterest::lastPendingInterestId_ = 0;
uint64_t Node::RegisteredPrefix::lastRegisteredPrefixId_ = 0;

Node::Node(const ptr_lib::shared_ptr<Transport>& transport)
  : pitTimeoutCheckTimerActive_(false)
  , transport_(transport)
  , ndndIdFetcherInterest_(Name("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY"), 4000.0)
{
  ioService_ = ptr_lib::make_shared<boost::asio::io_service>();      
  pitTimeoutCheckTimer_      = ptr_lib::make_shared<boost::asio::deadline_timer>(boost::ref(*ioService_));
  processEventsTimeoutTimer_ = ptr_lib::make_shared<boost::asio::deadline_timer>(boost::ref(*ioService_));
}

Node::Node(const ptr_lib::shared_ptr<Transport>& transport, const ptr_lib::shared_ptr<boost::asio::io_service> &ioService)
  : ioService_(ioService)
  , pitTimeoutCheckTimerActive_(false)
  , transport_(transport)
  , ndndIdFetcherInterest_(Name("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY"), 4000.0)
{
  pitTimeoutCheckTimer_      = ptr_lib::make_shared<boost::asio::deadline_timer>(boost::ref(*ioService_));
  processEventsTimeoutTimer_ = ptr_lib::make_shared<boost::asio::deadline_timer>(boost::ref(*ioService_));
}

uint64_t 
Node::expressInterest(const Interest& interest, const OnData& onData, const OnTimeout& onTimeout)
{
  if (!transport_->isConnected())
    transport_->connect(*ioService_,
                        ptr_lib::bind(&Node::onReceiveElement, this, _1));
  
  uint64_t pendingInterestId = PendingInterest::getNextPendingInterestId();
  pendingInterestTable_.push_back(ptr_lib::shared_ptr<PendingInterest>(new PendingInterest
    (pendingInterestId, ptr_lib::shared_ptr<const Interest>(new Interest(interest)), onData, onTimeout)));

  transport_->send(interest.wireEncode());

  if (!pitTimeoutCheckTimerActive_) {
    pitTimeoutCheckTimerActive_ = true;
    pitTimeoutCheckTimer_->expires_from_now(boost::posix_time::milliseconds(100));
    pitTimeoutCheckTimer_->async_wait(func_lib::bind(&Node::checkPitExpire, this));
  }
  
  return pendingInterestId;
}

void
Node::put(const Data &data)
{
  if (!transport_->isConnected())
    transport_->connect(*ioService_,
                        ptr_lib::bind(&Node::onReceiveElement, this, _1));

  transport_->send(data.wireEncode());
}


void
Node::removePendingInterest(uint64_t pendingInterestId)
{
  // Go backwards through the list so we can erase entries.
  // Remove all entries even though pendingInterestId should be unique.
  for (int i = (int)pendingInterestTable_.size() - 1; i >= 0; --i) {
    if (pendingInterestTable_[i]->getPendingInterestId() == pendingInterestId)
      pendingInterestTable_.erase(pendingInterestTable_.begin() + i);
  }
}

uint64_t 
Node::registerPrefix
  (const Name& prefix, const OnInterest& onInterest, const OnRegisterFailed& onRegisterFailed, const ForwardingFlags& flags)
{
  // Get the registeredPrefixId now so we can return it to the caller.
  uint64_t registeredPrefixId = RegisteredPrefix::getNextRegisteredPrefixId();
  ptr_lib::shared_ptr<const Name> prefixPtr = ptr_lib::make_shared<const Name>(prefix);
  
  if (ndndId_.size() == 0) {
    // First fetch the ndndId of the connected hub.
    NdndIdFetcher fetcher(ndndId_,
                          func_lib::bind(&Node::registerPrefixHelper, this,
                                         registeredPrefixId, prefixPtr, onInterest, onRegisterFailed, flags),
                          func_lib::bind(onRegisterFailed, prefixPtr));

    // @todo: Check if this crash
    // It is OK for func_lib::function make a copy of the function object because the Info is in a ptr_lib::shared_ptr.
    expressInterest(ndndIdFetcherInterest_, fetcher, fetcher);
  }
  else
    registerPrefixHelper(registeredPrefixId, prefixPtr, onInterest, onRegisterFailed, flags);
  
  return registeredPrefixId;
}

void
Node::removeRegisteredPrefix(uint64_t registeredPrefixId)
{
  // Go backwards through the list so we can erase entries.
  // Remove all entries even though pendingInterestId should be unique.

  for (RegisteredPrefixTable::iterator i = registeredPrefixTable_.begin();
       i != registeredPrefixTable_.end();
       ++i)
    {
      if ((*i)->getRegisteredPrefixId() == registeredPrefixId) {
        ForwardingEntry forwardingEntry("unreg", *(*i)->getPrefix(), faceId_);
        Data data;
        data.setContent(forwardingEntry.wireEncode());
        
        SignatureSha256WithRsa signature;
        signature.setValue(Block(Tlv::SignatureValue, ptr_lib::make_shared<Buffer>()));
        data.setSignature(signature);

        // Create an interest where the name has the encoded Data packet.
        Name interestName;
        interestName.append("ndnx");
        interestName.append(ndndId_);
        interestName.append("unreg");
        interestName.append(data.wireEncode());

        Interest interest(interestName);
        interest.setScope(1);
        interest.setInterestLifetime(1000);

        expressInterest(interest, OnData(), OnTimeout());
        
        registeredPrefixTable_.erase(i);
        break;
      }
    }
}

void 
Node::registerPrefixHelper(uint64_t registeredPrefixId,
                           const ptr_lib::shared_ptr<const Name>& prefix,
                           const OnInterest& onInterest,
                           const OnRegisterFailed& onRegisterFailed, 
                           const ForwardingFlags& flags)
{
  // Create a ForwardingEntry.

  // AlexA: ndnd ignores any freshness that is larger than 3600 sec and sets 300 sec instead
  //        to register "forever" (=2000000000 sec), freshnessPeriod must be omitted
  ForwardingEntry forwardingEntry("selfreg", *prefix, -1, flags, -1);
  Block content = forwardingEntry.wireEncode();

  // Set the ForwardingEntry as the content of a Data packet and sign.
  Data data;
  data.setContent(content);
  
  // Create an empty signature, since nobody going to verify it for now
  // @todo In the future, we may require real signatures to do the registration
  SignatureSha256WithRsa signature;
  signature.setValue(Block(Tlv::SignatureValue, ptr_lib::make_shared<Buffer>()));
  data.setSignature(signature);

  // Create an interest where the name has the encoded Data packet.
  Name interestName;
  interestName.append("ndnx");
  interestName.append(ndndId_);
  interestName.append("selfreg");
  interestName.append(data.wireEncode());

  Interest interest(interestName);
  interest.setScope(1);
  interest.setInterestLifetime(1000);

  expressInterest(interest,
                  func_lib::bind(&Node::registerPrefixFinal, this,
                                 registeredPrefixId, prefix, onInterest, onRegisterFailed, _1, _2),
                  func_lib::bind(onRegisterFailed, prefix));
}

void
Node::registerPrefixFinal(uint64_t registeredPrefixId,
                          const ptr_lib::shared_ptr<const Name>& prefix,
                          const OnInterest& onInterest,
                          const OnRegisterFailed& onRegisterFailed,
                          const ptr_lib::shared_ptr<const Interest>&, const ptr_lib::shared_ptr<Data>&data)
{
  Block content = data->getContent();
  content.parse();

  if (content.getAll().empty())
    {
      onRegisterFailed(prefix);
      return;
    }

  Block::element_iterator val = content.getAll().begin();
  
  switch(val->type())
    {
    case Tlv::FaceManagement::ForwardingEntry:
      {
        ForwardingEntry entry;
        entry.wireDecode(*val);

        // Save the onInterest callback and send the registration interest.
        registeredPrefixTable_.push_back(ptr_lib::make_shared<RegisteredPrefix>(registeredPrefixId, prefix, onInterest));

        /// @todo Notify user about successful registration
        
        // succeeded
        return;
      }
    case Tlv::FaceManagement::StatusResponse:
      {
        // failed :(
        StatusResponse resp;
        resp.wireDecode(*val);

        // std::cerr << "StatusReponse: " << resp << std::endl;
      
        onRegisterFailed(prefix);
        return;
      }
    default:
      {
        // failed :(
      
        onRegisterFailed(prefix);
        return;
      }
    }
}

void 
Node::processEvents(Milliseconds timeout/* = 0 */, bool keepThread/* = false*/)
{
  try
    {
      if (timeout < 0)
        {
          // do not block if timeout is negative, but process pending events
          ioService_->poll();
          return;
        }

      if (timeout > 0)
        {
          processEventsTimeoutTimer_->expires_from_now(boost::posix_time::milliseconds(timeout));
          processEventsTimeoutTimer_->async_wait(fireProcessEventsTimeout);
        }
      
      if (keepThread) {
        // work will ensure that ioService_ is running until work object exists
        ioServiceWork_ = ptr_lib::make_shared<boost::asio::io_service::work>(boost::ref(*ioService_));
      }
          
      ioService_->run();
      ioService_->reset(); // so it is possible to run processEvents again (if necessary)
    }
  catch(Node::ProcessEventsTimeout &)
    {
      // break
      ioService_->reset();
    }
  catch(const std::exception &)
    {
      ioService_->reset();
      pendingInterestTable_.clear();
      registeredPrefixTable_.clear();
      throw;
    }
}

void
Node::fireProcessEventsTimeout(const boost::system::error_code& error)
{
  if (!error) // can fire for some other reason, e.g., cancelled
    throw Node::ProcessEventsTimeout();
}

void
Node::checkPitExpire()
{
  // Check for PIT entry timeouts.  Go backwards through the list so we can erase entries.
  MillisecondsSince1970 nowMilliseconds = ndn_getNowMilliseconds();
  for (int i = (int)pendingInterestTable_.size() - 1; i >= 0; --i) {
    if (pendingInterestTable_[i]->isTimedOut(nowMilliseconds)) {
      // Save the PendingInterest and remove it from the PIT.  Then call the callback.
      ptr_lib::shared_ptr<PendingInterest> pendingInterest = pendingInterestTable_[i];
      pendingInterestTable_.erase(pendingInterestTable_.begin() + i);
      pendingInterest->callTimeout();
      
      // Refresh now since the timeout callback might have delayed.
      nowMilliseconds = ndn_getNowMilliseconds();
    }
  }

  if (!pendingInterestTable_.empty()) {
    pitTimeoutCheckTimerActive_ = true;
    
    pitTimeoutCheckTimer_->expires_from_now(boost::posix_time::milliseconds(100));
    pitTimeoutCheckTimer_->async_wait(func_lib::bind(&Node::checkPitExpire, this));
  }
  else {
    pitTimeoutCheckTimerActive_ = false;

    if (registeredPrefixTable_.empty()) {
      transport_->close();
      if (!ioServiceWork_) {
        processEventsTimeoutTimer_->cancel();
      }
    }
  }
}


void 
Node::onReceiveElement(const Block &block)
{
  if (block.type() == Tlv::Interest)
    {
      ptr_lib::shared_ptr<Interest> interest(new Interest());
      interest->wireDecode(block);
    
      RegisteredPrefixTable::iterator entry = getEntryForRegisteredPrefix(interest->getName());
      if (entry != registeredPrefixTable_.end()) {
        (*entry)->getOnInterest()((*entry)->getPrefix(), interest, *transport_, (*entry)->getRegisteredPrefixId());
      }
    }
  else if (block.type() == Tlv::Data)
    {
      ptr_lib::shared_ptr<Data> data(new Data());
      data->wireDecode(block);

      PendingInterestTable::iterator entry = getEntryIndexForExpressedInterest(data->getName());
      if (entry != pendingInterestTable_.end()) {
        // Copy pointers to the needed objects and remove the PIT entry before the calling the callback.
        const OnData onData = (*entry)->getOnData();
        const ptr_lib::shared_ptr<const Interest> interest = (*entry)->getInterest();
        pendingInterestTable_.erase(entry);

        if (onData) {
          onData(interest, data);
        }

        if (pendingInterestTable_.empty()) {
          pitTimeoutCheckTimer_->cancel(); // this will cause checkPitExpire invocation
        }
      }
    }
}

void 
Node::shutdown()
{
  pendingInterestTable_.clear();
  registeredPrefixTable_.clear();

  transport_->close();
  pitTimeoutCheckTimer_->cancel();
  processEventsTimeoutTimer_->cancel();
  pitTimeoutCheckTimerActive_ = false;
}

Node::PendingInterestTable::iterator 
Node::getEntryIndexForExpressedInterest(const Name& name)
{
  for (PendingInterestTable::iterator i = pendingInterestTable_.begin ();
       i != pendingInterestTable_.end(); ++i)
    {
      if ((*i)->getInterest()->matchesName(name))
        {
          return i;
        }
    }

  return pendingInterestTable_.end();
}
  
Node::RegisteredPrefixTable::iterator
Node::getEntryForRegisteredPrefix(const Name& name)
{
  RegisteredPrefixTable::iterator longestPrefix = registeredPrefixTable_.end();

  for (RegisteredPrefixTable::iterator i = registeredPrefixTable_.begin();
       i != registeredPrefixTable_.end();
       ++i)
    {
      if (longestPrefix == registeredPrefixTable_.end() ||
          (*i)->getPrefix()->size() > (*longestPrefix)->getPrefix()->size())
        {
          longestPrefix = i;
        }
    }
  return longestPrefix;
}

Node::PendingInterest::PendingInterest(uint64_t pendingInterestId,
                                       const ptr_lib::shared_ptr<const Interest>& interest,
                                       const OnData& onData, const OnTimeout& onTimeout)
: pendingInterestId_(pendingInterestId),
  interest_(interest),
  onData_(onData), onTimeout_(onTimeout)
{
  // Set up timeoutTime_.
  if (interest_->getInterestLifetime() >= 0)
    timeoutTimeMilliseconds_ = ndn_getNowMilliseconds() + interest_->getInterestLifetime();
  else
    // No timeout.
    /**
     * @todo Set more meaningful default timeout.  This timeout MUST exist.
     */
    timeoutTimeMilliseconds_ = ndn_getNowMilliseconds() + 4000;
}

void 
Node::PendingInterest::callTimeout()
{
  if (onTimeout_) {
    onTimeout_(interest_);
  }
}

}

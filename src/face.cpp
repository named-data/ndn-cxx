/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"

#include "face.hpp"

#include "security/signature-sha256-with-rsa.hpp"

#include "util/time.hpp"
#include "util/random.hpp"

#include "management/ndnd-controller.hpp"
#include "management/nfd-controller.hpp"

namespace ndn {

Face::Face(bool nfdMode/* = false*/)
{
  construct(shared_ptr<Transport>(new UnixTransport()),
            make_shared<boost::asio::io_service>(), nfdMode);
}

Face::Face(const shared_ptr<boost::asio::io_service> &ioService, bool nfdMode/* = false*/)
{
  construct(shared_ptr<Transport>(new UnixTransport()),
            ioService, nfdMode);
}

Face::Face(const std::string &host, const std::string &port/* = "6363"*/, bool nfdMode/* = false*/)
{
  construct(shared_ptr<Transport>(new TcpTransport(host, port)),
            make_shared<boost::asio::io_service>(), nfdMode);
}

Face::Face(const shared_ptr<Transport>& transport, bool nfdMode/* = false*/)
{
  construct(transport,
            make_shared<boost::asio::io_service>(), nfdMode);
}

Face::Face(const shared_ptr<Transport>& transport,
           const shared_ptr<boost::asio::io_service> &ioService,
           bool nfdMode/* = false*/)
{
  construct(transport, ioService, nfdMode);
}

void
Face::construct(const shared_ptr<Transport>& transport,
                const shared_ptr<boost::asio::io_service> &ioService,
                bool nfdMode)
{
  pitTimeoutCheckTimerActive_ = false;
  transport_ = transport;
  ioService_ = ioService;

  pitTimeoutCheckTimer_      = make_shared<boost::asio::deadline_timer>(boost::ref(*ioService_));
  processEventsTimeoutTimer_ = make_shared<boost::asio::deadline_timer>(boost::ref(*ioService_));
  
  if (nfdMode)
      m_fwController = make_shared<nfd::Controller>(boost::ref(*this));
  else
      m_fwController = make_shared<ndnd::Controller>(boost::ref(*this));
}


const PendingInterestId*
Face::expressInterest(const Interest& interest, const OnData& onData, const OnTimeout& onTimeout)
{
  if (!transport_->isConnected())
    transport_->connect(*ioService_,
                        bind(&Face::onReceiveElement, this, _1));

  shared_ptr<const Interest> interestToExpress(new Interest(interest));
  
  ioService_->post(bind(&Face::asyncExpressInterest, this, interestToExpress, onData, onTimeout));
  
  return reinterpret_cast<const PendingInterestId*>(interestToExpress.get());
}

const PendingInterestId*
Face::expressInterest(const Name& name,
                      const Interest &tmpl,
                      const OnData& onData, const OnTimeout& onTimeout/* = OnTimeout()*/)
{
  return expressInterest(Interest(name,
                                  tmpl.getMinSuffixComponents(),
                                  tmpl.getMaxSuffixComponents(),
                                  tmpl.getExclude(),
                                  tmpl.getChildSelector(),
                                  tmpl.getMustBeFresh(),
                                  tmpl.getScope(),
                                  tmpl.getInterestLifetime()),
                         onData, onTimeout);
}

void
Face::asyncExpressInterest(const shared_ptr<const Interest> &interest,
                           const OnData& onData, const OnTimeout& onTimeout)
{
  pendingInterestTable_.push_back(shared_ptr<PendingInterest>(new PendingInterest
                                                              (interest, onData, onTimeout)));

  transport_->send(interest->wireEncode());

  if (!pitTimeoutCheckTimerActive_) {
    pitTimeoutCheckTimerActive_ = true;
    pitTimeoutCheckTimer_->expires_from_now(boost::posix_time::milliseconds(100));
    pitTimeoutCheckTimer_->async_wait(bind(&Face::checkPitExpire, this));
  }
}
    
void
Face::put(const Data &data)
{
  if (!transport_->isConnected())
    transport_->connect(*ioService_,
                        bind(&Face::onReceiveElement, this, _1));

  transport_->send(data.wireEncode());
}


void
Face::removePendingInterest(const PendingInterestId *pendingInterestId)
{
  ioService_->post(bind(&Face::asyncRemovePendingInterest, this, pendingInterestId));
}


void
Face::asyncRemovePendingInterest(const PendingInterestId *pendingInterestId)
{
  pendingInterestTable_.remove_if(MatchPendingInterestId(pendingInterestId));
}

const RegisteredPrefixId*
Face::setInterestFilter(const Name& prefix,
                        const OnInterest& onInterest,
                        const OnSetInterestFilterFailed& onSetInterestFilterFailed)
{
  shared_ptr<RegisteredPrefix> prefixToRegister(new RegisteredPrefix(prefix, onInterest));

  m_fwController->selfRegisterPrefix(prefixToRegister->getPrefix(),
                                     bind(&RegisteredPrefixTable::push_back, &registeredPrefixTable_, prefixToRegister),
                                     bind(onSetInterestFilterFailed, prefixToRegister->getPrefix(), _1));
  
  return reinterpret_cast<const RegisteredPrefixId*>(prefixToRegister.get());
}

void
Face::unsetInterestFilter(const RegisteredPrefixId *registeredPrefixId)
{
  ioService_->post(bind(&Face::asyncUnsetInterestFilter, this, registeredPrefixId));
}

void
Face::asyncUnsetInterestFilter(const RegisteredPrefixId *registeredPrefixId)
{
  RegisteredPrefixTable::iterator i = std::find_if(registeredPrefixTable_.begin(), registeredPrefixTable_.end(),
                                                   MatchRegisteredPrefixId(registeredPrefixId));  
  if (i != registeredPrefixTable_.end())
    {
      m_fwController->selfDeregisterPrefix((*i)->getPrefix(),
                                           bind(&Face::finalizeUnsertInterestFilter, this, i),
                                           Controller::FailCallback());
    }

  // there cannot be two registered prefixes with the same id. if there are, then something is broken
}

void
Face::finalizeUnsertInterestFilter(RegisteredPrefixTable::iterator item)
{
  registeredPrefixTable_.erase(item);

  if (!pitTimeoutCheckTimerActive_ && registeredPrefixTable_.empty())
    {
      transport_->close();
      if (!ioServiceWork_) {
        processEventsTimeoutTimer_->cancel();
      }
    }
}

void 
Face::processEvents(Milliseconds timeout/* = 0 */, bool keepThread/* = false*/)
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
          processEventsTimeoutTimer_->async_wait(&fireProcessEventsTimeout);
        }
      
      if (keepThread) {
        // work will ensure that ioService_ is running until work object exists
        ioServiceWork_ = make_shared<boost::asio::io_service::work>(boost::ref(*ioService_));
      }
          
      ioService_->run();
      ioService_->reset(); // so it is possible to run processEvents again (if necessary)
    }
  catch(Face::ProcessEventsTimeout &)
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
Face::shutdown()
{
  pendingInterestTable_.clear();
  registeredPrefixTable_.clear();

  transport_->close();
  pitTimeoutCheckTimer_->cancel();
  processEventsTimeoutTimer_->cancel();
  pitTimeoutCheckTimerActive_ = false;
}

void
Face::fireProcessEventsTimeout(const boost::system::error_code& error)
{
  if (!error) // can fire for some other reason, e.g., cancelled
    throw Face::ProcessEventsTimeout();
}

void
Face::checkPitExpire()
{
  // Check for PIT entry timeouts.  Go backwards through the list so we can erase entries.
  MillisecondsSince1970 nowMilliseconds = getNowMilliseconds();

  PendingInterestTable::iterator i = pendingInterestTable_.begin();
  while (i != pendingInterestTable_.end())
    {
      if ((*i)->isTimedOut(nowMilliseconds))
        {
          // Save the PendingInterest and remove it from the PIT.  Then call the callback.
          shared_ptr<PendingInterest> pendingInterest = *i;

          i = pendingInterestTable_.erase(i);

          pendingInterest->callTimeout();
        }
      else
        ++i;
    }

  if (!pendingInterestTable_.empty()) {
    pitTimeoutCheckTimerActive_ = true;
    
    pitTimeoutCheckTimer_->expires_from_now(boost::posix_time::milliseconds(100));
    pitTimeoutCheckTimer_->async_wait(bind(&Face::checkPitExpire, this));
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
Face::onReceiveElement(const Block &block)
{
  if (block.type() == Tlv::Interest)
    {
      shared_ptr<Interest> interest(new Interest());
      interest->wireDecode(block);
    
      RegisteredPrefixTable::iterator entry = getEntryForRegisteredPrefix(interest->getName());
      if (entry != registeredPrefixTable_.end()) {
        (*entry)->getOnInterest()((*entry)->getPrefix(), *interest);
      }
    }
  else if (block.type() == Tlv::Data)
    {
      shared_ptr<Data> data(new Data());
      data->wireDecode(block);

      PendingInterestTable::iterator entry = getEntryIndexForExpressedInterest(data->getName());
      if (entry != pendingInterestTable_.end()) {
        // Copy pointers to the needed objects and remove the PIT entry before the calling the callback.
        const OnData onData = (*entry)->getOnData();
        const shared_ptr<const Interest> interest = (*entry)->getInterest();
        pendingInterestTable_.erase(entry);

        if (onData) {
          onData(*interest, *data);
        }

        if (pendingInterestTable_.empty()) {
          pitTimeoutCheckTimer_->cancel(); // this will cause checkPitExpire invocation
        }
      }
    }
}

Face::PendingInterestTable::iterator 
Face::getEntryIndexForExpressedInterest(const Name& name)
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
  
Face::RegisteredPrefixTable::iterator
Face::getEntryForRegisteredPrefix(const Name& name)
{
  RegisteredPrefixTable::iterator longestPrefix = registeredPrefixTable_.end();

  for (RegisteredPrefixTable::iterator i = registeredPrefixTable_.begin();
       i != registeredPrefixTable_.end();
       ++i)
    {
      if (longestPrefix == registeredPrefixTable_.end() ||
          (*i)->getPrefix().size() > (*longestPrefix)->getPrefix().size())
        {
          longestPrefix = i;
        }
    }
  return longestPrefix;
}

} // namespace ndn

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "dns.hpp"

#include "scheduler.hpp"

namespace ndn {
namespace dns {

typedef boost::asio::ip::udp::endpoint EndPoint;
typedef boost::asio::ip::basic_resolver<boost::asio::ip::udp> BoostResolver;

class Resolver : noncopyable
{
public:
  Resolver(const SuccessCallback& onSuccess,
           const ErrorCallback& onError,
           const ndn::dns::AddressSelector& addressSelector,
           boost::asio::io_service& ioService)
    : m_resolver(ioService)
    , m_addressSelector(addressSelector)
    , m_onSuccess(onSuccess)
    , m_onError(onError)
    , m_scheduler(ioService)
  {
  }

  void
  asyncResolve(const std::string& host,
               const time::nanoseconds& timeout,
               const shared_ptr<Resolver>& self)
  {
    BoostResolver::query query(host, NULL_PORT);

    m_resolver.async_resolve(query, bind(&Resolver::onResolveSuccess, this, _1, _2, self));

    m_resolveTimeout = m_scheduler.scheduleEvent(timeout,
                                                 bind(&Resolver::onResolveError, this,
                                                 "Timeout", self));
  }

  BoostResolver::iterator
  syncResolve(BoostResolver::query query)
  {
    return m_resolver.resolve(query);
  }

  void
  onResolveSuccess(const boost::system::error_code& error,
                   BoostResolver::iterator remoteEndpoint,
                   const shared_ptr<Resolver>& self)
  {
    m_scheduler.cancelEvent(m_resolveTimeout);

    if (error)
      {
        if (error == boost::system::errc::operation_canceled)
          {
            return;
          }

        return m_onError("Remote endpoint hostname or port cannot be resolved: " +
                         error.category().message(error.value()));
      }

    BoostResolver::iterator end;
    for (; remoteEndpoint != end; ++remoteEndpoint)
      {
        IpAddress address(EndPoint(*remoteEndpoint).address());

        if (m_addressSelector(address))
          {
            return m_onSuccess(address);
          }
      }

    m_onError("No endpoint matching the specified address selector found");
  }

  void
  onResolveError(const std::string& errorInfo, const shared_ptr<Resolver>& self)
  {
    m_resolver.cancel();
    m_onError(errorInfo);
  }

public:
  static const std::string NULL_PORT;

private:
  BoostResolver m_resolver;
  EventId m_resolveTimeout;

  ndn::dns::AddressSelector m_addressSelector;
  SuccessCallback m_onSuccess;
  ErrorCallback m_onError;

  Scheduler m_scheduler;
};

const std::string Resolver::NULL_PORT = "";

void
asyncResolve(const std::string& host,
             const SuccessCallback& onSuccess,
             const ErrorCallback& onError,
             boost::asio::io_service& ioService,
             const ndn::dns::AddressSelector& addressSelector,
             const time::nanoseconds& timeout)
{
  shared_ptr<Resolver> resolver = make_shared<Resolver>(onSuccess, onError,
                                                        addressSelector, ndn::ref(ioService));
  resolver->asyncResolve(host, timeout, resolver);
  // resolver will be destroyed when async operation finishes or global IO service stops
}

IpAddress
syncResolve(const std::string& host, boost::asio::io_service& ioService,
            const ndn::dns::AddressSelector& addressSelector)
{
  Resolver resolver(SuccessCallback(), ErrorCallback(), addressSelector, ioService);

  BoostResolver::query query(host, Resolver::NULL_PORT);

  BoostResolver::iterator remoteEndpoint = resolver.syncResolve(query);

  BoostResolver::iterator end;
  for (; remoteEndpoint != end; ++remoteEndpoint)
    {
      if (addressSelector(EndPoint(*remoteEndpoint).address()))
        {
          return EndPoint(*remoteEndpoint).address();
        }
    }
  BOOST_THROW_EXCEPTION(Error("No endpoint matching the specified address selector found"));
}

} // namespace dns
} // namespace ndn

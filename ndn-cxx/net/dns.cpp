/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "ndn-cxx/net/dns.hpp"
#include "ndn-cxx/util/scheduler.hpp"

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/post.hpp>

namespace ndn::dns {

class Resolver : noncopyable
{
public:
  Resolver(boost::asio::io_context& ioCtx,
           const AddressSelector& addressSelector)
    : m_resolver(ioCtx)
    , m_addressSelector(addressSelector)
    , m_scheduler(ioCtx)
  {
    BOOST_ASSERT(m_addressSelector != nullptr);
  }

  void
  asyncResolve(const std::string& host,
               const SuccessCallback& onSuccess,
               const ErrorCallback& onError,
               time::nanoseconds timeout,
               const shared_ptr<Resolver>& self)
  {
    m_onSuccess = onSuccess;
    m_onError = onError;

    m_resolver.async_resolve(host, "", [=] (auto&&... args) {
      onResolveResult(std::forward<decltype(args)>(args)..., self);
    });

    m_resolveTimeout = m_scheduler.schedule(timeout, [=] { onResolveTimeout(self); });
  }

private:
  void
  onResolveResult(const boost::system::error_code& error,
                  const boost::asio::ip::udp::resolver::results_type& results,
                  const shared_ptr<Resolver>& self)
  {
    m_resolveTimeout.cancel();

    // ensure the Resolver isn't destructed while callbacks are still pending, see #2653
    boost::asio::post(m_resolver.get_executor(), [self] {});

    if (error) {
      if (m_onError && error != boost::asio::error::operation_aborted) {
        m_onError("Hostname could not be resolved: " + error.message());
      }
      return;
    }

    for (const auto& entry : results) {
      if (m_addressSelector(entry.endpoint().address())) {
        if (m_onSuccess) {
          m_onSuccess(entry.endpoint().address());
        }
        return;
      }
    }

    if (m_onError) {
      m_onError("No endpoints match the specified address selector");
    }
  }

  void
  onResolveTimeout(const shared_ptr<Resolver>& self)
  {
    m_resolver.cancel();

    // ensure the Resolver isn't destructed while callbacks are still pending, see #2653
    boost::asio::post(m_resolver.get_executor(), [self] {});

    if (m_onError) {
      m_onError("Hostname resolution timed out");
    }
  }

private:
  boost::asio::ip::udp::resolver m_resolver;

  AddressSelector m_addressSelector;
  SuccessCallback m_onSuccess;
  ErrorCallback m_onError;

  Scheduler m_scheduler;
  scheduler::EventId m_resolveTimeout;
};

void
asyncResolve(const std::string& host,
             const SuccessCallback& onSuccess,
             const ErrorCallback& onError,
             boost::asio::io_context& ioCtx,
             const AddressSelector& addressSelector,
             time::nanoseconds timeout)
{
  auto resolver = make_shared<Resolver>(ioCtx, addressSelector);
  resolver->asyncResolve(host, onSuccess, onError, timeout, resolver);
  // resolver will be destroyed when async operation finishes or ioCtx stops
}

} // namespace ndn::dns

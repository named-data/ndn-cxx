/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifndef NDN_TRANSPORT_STREAM_TRANSPORT_WITH_RESOLVER_IMPL_HPP
#define NDN_TRANSPORT_STREAM_TRANSPORT_WITH_RESOLVER_IMPL_HPP

#include "stream-transport-impl.hpp"

namespace ndn {

/** \brief implementation detail of a Boost.Asio-based stream-oriented transport
 *         with resolver support
 */
template<typename BaseTransport, typename Protocol>
class StreamTransportWithResolverImpl : public StreamTransportImpl<BaseTransport, Protocol>
{
public:
  typedef StreamTransportWithResolverImpl<BaseTransport,Protocol> Impl;

  StreamTransportWithResolverImpl(BaseTransport& transport, boost::asio::io_service& ioService)
    : StreamTransportImpl<BaseTransport, Protocol>(transport, ioService)
  {
  }

  void
  connect(const typename Protocol::resolver::query& query)
  {
    if (this->m_isConnecting) {
      return;
    }

    this->m_isConnecting = true;

    // Wait at most 4 seconds to connect
    /// @todo Decide whether this number should be configurable
    this->m_connectTimer.expires_from_now(boost::posix_time::seconds(4));
    this->m_connectTimer.async_wait(bind(&Impl::connectTimeoutHandler, this->shared_from_this(), _1));

    // typename boost::asio::ip::basic_resolver< Protocol > resolver;
    auto resolver = make_shared<typename Protocol::resolver>(ref(this->m_socket.get_io_service()));
    resolver->async_resolve(query, bind(&Impl::resolveHandler, this->shared_from_base(), _1, _2, resolver));
  }

protected:
  void
  resolveHandler(const boost::system::error_code& error,
                 typename Protocol::resolver::iterator endpoint,
                 const shared_ptr<typename Protocol::resolver>&)
  {
    if (error) {
      if (error == boost::system::errc::operation_canceled)
        return;

      BOOST_THROW_EXCEPTION(Transport::Error(error, "Error during resolution of host or port"));
    }

    typename Protocol::resolver::iterator end;
    if (endpoint == end) {
      this->m_transport.close();
      BOOST_THROW_EXCEPTION(Transport::Error(error, "Unable to resolve because host or port"));
    }

    this->m_socket.async_connect(*endpoint, bind(&Impl::connectHandler, this->shared_from_this(), _1));
  }

private:
  shared_ptr<Impl>
  shared_from_base()
  {
    return static_pointer_cast<Impl>(this->shared_from_this());
  }
};


} // namespace ndn

#endif // NDN_TRANSPORT_STREAM_TRANSPORT_WITH_RESOLVER_IMPL_HPP

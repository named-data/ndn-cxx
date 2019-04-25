/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#ifndef NDN_TRANSPORT_DETAIL_STREAM_TRANSPORT_WITH_RESOLVER_IMPL_HPP
#define NDN_TRANSPORT_DETAIL_STREAM_TRANSPORT_WITH_RESOLVER_IMPL_HPP

#include "ndn-cxx/transport/detail/stream-transport-impl.hpp"

namespace ndn {
namespace detail {

/** \brief implementation detail of a Boost.Asio-based stream-oriented transport
 *         with resolver support
 */
template<typename BaseTransport, typename Protocol>
class StreamTransportWithResolverImpl : public StreamTransportImpl<BaseTransport, Protocol>
{
public:
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
    this->m_connectTimer.expires_from_now(std::chrono::seconds(4));
    this->m_connectTimer.async_wait([self = this->shared_from_base()] (const auto& ec) {
      self->connectTimeoutHandler(ec);
    });

    auto resolver = make_shared<typename Protocol::resolver>(this->m_socket
#if BOOST_VERSION >= 107000
                                                             .get_executor()
#else
                                                             .get_io_service()
#endif
                                                             );
    resolver->async_resolve(query, [self = this->shared_from_base(), resolver] (auto&&... args) {
      self->resolveHandler(std::forward<decltype(args)>(args)..., resolver);
    });
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

      NDN_THROW(Transport::Error(error, "Error during resolution of host or port"));
    }

    typename Protocol::resolver::iterator end;
    if (endpoint == end) {
      this->m_transport.close();
      NDN_THROW(Transport::Error(error, "Unable to resolve host or port"));
    }

    this->m_socket.async_connect(*endpoint, [self = this->shared_from_base()] (const auto& ec) {
      self->connectHandler(ec);
    });
  }

private:
  using Impl = StreamTransportWithResolverImpl<BaseTransport, Protocol>;

  shared_ptr<Impl>
  shared_from_base()
  {
    return static_pointer_cast<Impl>(this->shared_from_this());
  }
};

} // namespace detail
} // namespace ndn

#endif // NDN_TRANSPORT_DETAIL_STREAM_TRANSPORT_WITH_RESOLVER_IMPL_HPP

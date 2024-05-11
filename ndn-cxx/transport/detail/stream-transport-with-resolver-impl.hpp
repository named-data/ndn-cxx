/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2024 Regents of the University of California.
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

#ifndef NDN_CXX_TRANSPORT_DETAIL_STREAM_TRANSPORT_WITH_RESOLVER_IMPL_HPP
#define NDN_CXX_TRANSPORT_DETAIL_STREAM_TRANSPORT_WITH_RESOLVER_IMPL_HPP

#include "ndn-cxx/transport/detail/stream-transport-impl.hpp"

namespace ndn::detail {

/**
 * \brief Implementation detail of a Boost.Asio-based stream-oriented transport
 *        with resolver support.
 */
template<typename BaseTransport, typename Protocol>
class StreamTransportWithResolverImpl : public StreamTransportImpl<BaseTransport, Protocol>
{
public:
  StreamTransportWithResolverImpl(BaseTransport& transport, boost::asio::io_context& ioCtx)
    : StreamTransportImpl<BaseTransport, Protocol>(transport, ioCtx)
  {
  }

  void
  connect(std::string_view host, std::string_view port)
  {
    if (this->m_transport.getState() == Transport::State::CONNECTING) {
      return;
    }

    this->m_transport.setState(Transport::State::CONNECTING);
    auto hostAndPort = std::string(host) + ':' + std::string(port);

    // Wait at most 4 seconds to connect
    /// @todo Decide whether this number should be configurable
    this->m_connectTimer.expires_after(std::chrono::seconds(4));
    this->m_connectTimer.async_wait([self = this->shared_from_base(), hostAndPort] (const auto& ec) {
      if (ec) // e.g., cancelled timer
        return;

      self->m_transport.close();
      NDN_THROW(Transport::Error(boost::system::errc::make_error_code(boost::system::errc::timed_out),
                                 "could not connect to NDN forwarder at " + hostAndPort));
    });

    auto resolver = make_shared<typename Protocol::resolver>(this->m_socket.get_executor());
    resolver->async_resolve(host, port,
      [self = this->shared_from_base(), hostAndPort, resolver] (auto&&... args) {
        self->resolveHandler(hostAndPort, std::forward<decltype(args)>(args)...);
      });
  }

protected:
  void
  resolveHandler(const std::string& hostAndPort,
                 const boost::system::error_code& error,
                 const typename Protocol::resolver::results_type& endpoints)
  {
    if (error) {
      if (error == boost::asio::error::operation_aborted)
        return;

      this->m_transport.close();
      NDN_THROW(Transport::Error(error, "could not resolve " + hostAndPort));
    }

    BOOST_ASSERT(!endpoints.empty()); // guaranteed by Asio if the resolve operation is successful

    this->m_endpoint = *endpoints.begin();
    this->m_socket.async_connect(this->m_endpoint, [self = this->shared_from_base()] (const auto& ec) {
      self->connectHandler(ec);
    });
  }

private:
  using Impl = StreamTransportWithResolverImpl<BaseTransport, Protocol>;

  shared_ptr<Impl>
  shared_from_base()
  {
    return std::static_pointer_cast<Impl>(this->shared_from_this());
  }
};

} // namespace ndn::detail

#endif // NDN_CXX_TRANSPORT_DETAIL_STREAM_TRANSPORT_WITH_RESOLVER_IMPL_HPP

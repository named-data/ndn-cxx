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

#ifndef NDN_CXX_TRANSPORT_DETAIL_STREAM_TRANSPORT_IMPL_HPP
#define NDN_CXX_TRANSPORT_DETAIL_STREAM_TRANSPORT_IMPL_HPP

#include "ndn-cxx/transport/transport.hpp"

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>

#include <list>
#include <queue>

namespace ndn::detail {

/** \brief Implementation detail of a Boost.Asio-based stream-oriented transport.
 *  \tparam BaseTransport a subclass of Transport
 *  \tparam Protocol a Boost.Asio stream-oriented protocol, e.g. boost::asio::ip::tcp
 *                   or boost::asio::local::stream_protocol
 */
template<typename BaseTransport, typename Protocol>
class StreamTransportImpl : public std::enable_shared_from_this<StreamTransportImpl<BaseTransport, Protocol>>
{
public:
  using Impl = StreamTransportImpl<BaseTransport, Protocol>;
  using TransmissionQueue = std::queue<Block, std::list<Block>>;

  StreamTransportImpl(BaseTransport& transport, boost::asio::io_service& ioService)
    : m_transport(transport)
    , m_socket(ioService)
    , m_connectTimer(ioService)
  {
  }

  void
  connect(const typename Protocol::endpoint& endpoint)
  {
    if (m_transport.getState() == Transport::State::CONNECTING) {
      return;
    }
    m_transport.setState(Transport::State::CONNECTING);

    // Wait at most 4 seconds to connect
    /// @todo Decide whether this number should be configurable
    m_connectTimer.expires_from_now(std::chrono::seconds(4));
    m_connectTimer.async_wait([self = this->shared_from_this()] (const auto& error) {
      self->connectTimeoutHandler(error);
    });

    m_socket.open();
    m_socket.async_connect(endpoint, [self = this->shared_from_this()] (const auto& error) {
      self->connectHandler(error);
    });
  }

  void
  close()
  {
    m_transport.setState(Transport::State::CLOSED);

    boost::system::error_code error; // to silently ignore all errors
    m_connectTimer.cancel(error);
    m_socket.cancel(error);
    m_socket.close(error);

    TransmissionQueue{}.swap(m_transmissionQueue); // clear the queue
  }

  void
  pause()
  {
    if (m_transport.getState() == Transport::State::RUNNING) {
      m_socket.cancel();
      m_transport.setState(Transport::State::PAUSED);
    }
  }

  void
  resume()
  {
    if (m_transport.getState() == Transport::State::PAUSED) {
      m_transport.setState(Transport::State::RUNNING);
      m_inputBufferSize = 0;
      asyncReceive();
    }
  }

  void
  send(const Block& block)
  {
    m_transmissionQueue.push(block);

    if (m_transport.getState() != Transport::State::CLOSED &&
        m_transport.getState() != Transport::State::CONNECTING &&
        m_transmissionQueue.size() == 1) {
      asyncWrite();
    }
    // if not connected or there's another transmission in progress (m_transmissionQueue.size() > 1),
    // the next write will be scheduled either in connectHandler or in asyncWriteHandler
  }

protected:
  void
  connectHandler(const boost::system::error_code& error)
  {
    m_connectTimer.cancel();

    if (error) {
      m_transport.close();
      NDN_THROW(Transport::Error(error, "error while connecting to the forwarder"));
    }

    m_transport.setState(Transport::State::PAUSED);

    if (!m_transmissionQueue.empty()) {
      resume();
      asyncWrite();
    }
  }

  void
  connectTimeoutHandler(const boost::system::error_code& error)
  {
    if (error) // e.g., cancelled timer
      return;

    m_transport.close();
    NDN_THROW(Transport::Error(error, "error while connecting to the forwarder"));
  }

  void
  asyncWrite()
  {
    BOOST_ASSERT(!m_transmissionQueue.empty());
    boost::asio::async_write(m_socket, boost::asio::buffer(m_transmissionQueue.front()),
      // capture a copy of the shared_ptr to "this" to prevent deallocation
      [this, self = this->shared_from_this()] (const auto& error, size_t) {
        if (error) {
          if (error == boost::system::errc::operation_canceled) {
            // async receive has been explicitly cancelled (e.g., socket close)
            return;
          }
          m_transport.close();
          NDN_THROW(Transport::Error(error, "error while writing data to socket"));
        }

        if (m_transport.getState() == Transport::State::CLOSED) {
          return; // queue has already been cleared
        }

        BOOST_ASSERT(!m_transmissionQueue.empty());
        m_transmissionQueue.pop();

        if (!m_transmissionQueue.empty()) {
          asyncWrite();
        }
      });
  }

  void
  asyncReceive()
  {
    m_socket.async_receive(boost::asio::buffer(m_inputBuffer + m_inputBufferSize,
                                               MAX_NDN_PACKET_SIZE - m_inputBufferSize), 0,
      // capture a copy of the shared_ptr to "this" to prevent deallocation
      [this, self = this->shared_from_this()] (const auto& error, size_t nBytesRecvd) {
        if (error) {
          if (error == boost::system::errc::operation_canceled) {
            // async receive has been explicitly cancelled (e.g., socket close)
            return;
          }
          m_transport.close();
          NDN_THROW(Transport::Error(error, "error while receiving data from socket"));
        }

        m_inputBufferSize += nBytesRecvd;
        // do magic

        std::size_t offset = 0;
        bool hasProcessedSome = processAllReceived(m_inputBuffer, offset, m_inputBufferSize);
        if (!hasProcessedSome && m_inputBufferSize == MAX_NDN_PACKET_SIZE && offset == 0) {
          m_transport.close();
          NDN_THROW(Transport::Error("input buffer full, but a valid TLV cannot be decoded"));
        }

        if (offset > 0) {
          if (offset != m_inputBufferSize) {
            std::copy(m_inputBuffer + offset, m_inputBuffer + m_inputBufferSize, m_inputBuffer);
            m_inputBufferSize -= offset;
          }
          else {
            m_inputBufferSize = 0;
          }
        }

        asyncReceive();
      });
  }

  bool
  processAllReceived(uint8_t* buffer, size_t& offset, size_t nBytesAvailable)
  {
    while (offset < nBytesAvailable) {
      auto [isOk, element] = Block::fromBuffer({buffer + offset, nBytesAvailable - offset});
      if (!isOk) {
        return false;
      }
      m_transport.m_receiveCallback(element);
      offset += element.size();
    }
    return true;
  }

protected:
  BaseTransport& m_transport;

  typename Protocol::socket m_socket;
  uint8_t m_inputBuffer[MAX_NDN_PACKET_SIZE];
  size_t m_inputBufferSize = 0;
  TransmissionQueue m_transmissionQueue;
  boost::asio::steady_timer m_connectTimer;
};

} // namespace ndn::detail

#endif // NDN_CXX_TRANSPORT_DETAIL_STREAM_TRANSPORT_IMPL_HPP

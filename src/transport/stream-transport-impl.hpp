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

#ifndef NDN_TRANSPORT_STREAM_TRANSPORT_IMPL_HPP
#define NDN_TRANSPORT_STREAM_TRANSPORT_IMPL_HPP

#include "transport.hpp"

#include <boost/asio.hpp>
#include <list>

namespace ndn {

/** \brief implementation detail of a Boost.Asio-based stream-oriented transport
 *  \tparam BaseTransport a subclass of Transport
 *  \tparam Protocol a Boost.Asio stream-oriented protocol, including boost::asio::ip::tcp
 *                   and boost::asio::local::stream_protocol
 */
template<typename BaseTransport, typename Protocol>
class StreamTransportImpl : public enable_shared_from_this<StreamTransportImpl<BaseTransport, Protocol>>
{
public:
  typedef StreamTransportImpl<BaseTransport,Protocol> Impl;
  typedef std::list<Block> BlockSequence;
  typedef std::list<BlockSequence> TransmissionQueue;

  StreamTransportImpl(BaseTransport& transport, boost::asio::io_service& ioService)
    : m_transport(transport)
    , m_socket(ioService)
    , m_inputBufferSize(0)
    , m_isConnecting(false)
    , m_connectTimer(ioService)
  {
  }

  void
  connect(const typename Protocol::endpoint& endpoint)
  {
    if (!m_isConnecting) {
      m_isConnecting = true;

      // Wait at most 4 seconds to connect
      /// @todo Decide whether this number should be configurable
      m_connectTimer.expires_from_now(boost::posix_time::seconds(4));
      m_connectTimer.async_wait(bind(&Impl::connectTimeoutHandler, this->shared_from_this(), _1));

      m_socket.open();
      m_socket.async_connect(endpoint, bind(&Impl::connectHandler, this->shared_from_this(), _1));
    }
  }

  void
  close()
  {
    m_isConnecting = false;

    boost::system::error_code error; // to silently ignore all errors
    m_connectTimer.cancel(error);
    m_socket.cancel(error);
    m_socket.close(error);

    m_transport.m_isConnected = false;
    m_transport.m_isReceiving = false;
    m_transmissionQueue.clear();
  }

  void
  pause()
  {
    if (m_isConnecting)
      return;

    if (m_transport.m_isReceiving) {
      m_transport.m_isReceiving = false;
      m_socket.cancel();
    }
  }

  void
  resume()
  {
    if (m_isConnecting)
      return;

    if (!m_transport.m_isReceiving) {
      m_transport.m_isReceiving = true;
      m_inputBufferSize = 0;
      asyncReceive();
    }
  }

  void
  send(const Block& wire)
  {
    BlockSequence sequence;
    sequence.push_back(wire);
    send(std::move(sequence));
  }

  void
  send(const Block& header, const Block& payload)
  {
    BlockSequence sequence;
    sequence.push_back(header);
    sequence.push_back(payload);
    send(std::move(sequence));
  }

protected:
  void
  connectHandler(const boost::system::error_code& error)
  {
    m_isConnecting = false;
    m_connectTimer.cancel();

    if (!error) {
      resume();
      m_transport.m_isConnected = true;

      if (!m_transmissionQueue.empty()) {
        asyncWrite();
      }
    }
    else {
      m_transport.m_isConnected = false;
      m_transport.close();
      BOOST_THROW_EXCEPTION(Transport::Error(error, "error while connecting to the forwarder"));
    }
  }

  void
  connectTimeoutHandler(const boost::system::error_code& error)
  {
    if (error) // e.g., cancelled timer
      return;

    m_transport.close();
    BOOST_THROW_EXCEPTION(Transport::Error(error, "error while connecting to the forwarder"));
  }

  void
  send(BlockSequence&& sequence)
  {
    m_transmissionQueue.emplace_back(sequence);

    if (m_transport.m_isConnected && m_transmissionQueue.size() == 1) {
      asyncWrite();
    }

    // if not connected or there is transmission in progress (m_transmissionQueue.size() > 1),
    // next write will be scheduled either in connectHandler or in asyncWriteHandler
  }

  void
  asyncWrite()
  {
    BOOST_ASSERT(!m_transmissionQueue.empty());
    boost::asio::async_write(m_socket, m_transmissionQueue.front(),
      bind(&Impl::handleAsyncWrite, this->shared_from_this(), _1, m_transmissionQueue.begin()));
  }

  void
  handleAsyncWrite(const boost::system::error_code& error, TransmissionQueue::iterator queueItem)
  {
    if (error) {
      if (error == boost::system::errc::operation_canceled) {
        // async receive has been explicitly cancelled (e.g., socket close)
        return;
      }

      m_transport.close();
      BOOST_THROW_EXCEPTION(Transport::Error(error, "error while sending data to socket"));
    }

    if (!m_transport.m_isConnected) {
      return; // queue has been already cleared
    }

    m_transmissionQueue.erase(queueItem);

    if (!m_transmissionQueue.empty()) {
      asyncWrite();
    }
  }

  void
  asyncReceive()
  {
    m_socket.async_receive(boost::asio::buffer(m_inputBuffer + m_inputBufferSize,
                                               MAX_NDN_PACKET_SIZE - m_inputBufferSize), 0,
                           bind(&Impl::handleAsyncReceive, this->shared_from_this(), _1, _2));
  }

  void
  handleAsyncReceive(const boost::system::error_code& error, std::size_t nBytesRecvd)
  {
    if (error) {
      if (error == boost::system::errc::operation_canceled) {
        // async receive has been explicitly cancelled (e.g., socket close)
        return;
      }

      m_transport.close();
      BOOST_THROW_EXCEPTION(Transport::Error(error, "error while receiving data from socket"));
    }

    m_inputBufferSize += nBytesRecvd;
    // do magic

    std::size_t offset = 0;
    bool hasProcessedSome = processAllReceived(m_inputBuffer, offset, m_inputBufferSize);
    if (!hasProcessedSome && m_inputBufferSize == MAX_NDN_PACKET_SIZE && offset == 0) {
      m_transport.close();
      BOOST_THROW_EXCEPTION(Transport::Error(boost::system::error_code(),
                                             "input buffer full, but a valid TLV cannot be "
                                             "decoded"));
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
  }

  bool
  processAllReceived(uint8_t* buffer, size_t& offset, size_t nBytesAvailable)
  {
    while (offset < nBytesAvailable) {
      bool isOk = false;
      Block element;
      std::tie(isOk, element) = Block::fromBuffer(buffer + offset, nBytesAvailable - offset);
      if (!isOk)
        return false;

      m_transport.receive(element);
      offset += element.size();
    }
    return true;
  }

protected:
  BaseTransport& m_transport;

  typename Protocol::socket m_socket;
  uint8_t m_inputBuffer[MAX_NDN_PACKET_SIZE];
  size_t m_inputBufferSize;

  TransmissionQueue m_transmissionQueue;
  bool m_isConnecting;

  boost::asio::deadline_timer m_connectTimer;
};

} // namespace ndn

#endif // NDN_TRANSPORT_STREAM_TRANSPORT_IMPL_HPP

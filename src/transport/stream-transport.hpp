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

#ifndef NDN_TRANSPORT_STREAM_TRANSPORT_HPP
#define NDN_TRANSPORT_STREAM_TRANSPORT_HPP

#include "transport.hpp"

#include <list>

namespace ndn {

template<class BaseTransport, class Protocol>
class StreamTransportImpl
{
public:
  typedef StreamTransportImpl<BaseTransport,Protocol> Impl;

  typedef std::list<Block> BlockSequence;
  typedef std::list<BlockSequence> TransmissionQueue;

  StreamTransportImpl(BaseTransport& transport, boost::asio::io_service& ioService)
    : m_transport(transport)
    , m_socket(ioService)
    , m_inputBufferSize(0)
    , m_connectionInProgress(false)
    , m_connectTimer(ioService)
  {
  }

  void
  connectHandler(const boost::system::error_code& error)
  {
    m_connectionInProgress = false;
    m_connectTimer.cancel();

    if (!error)
      {
        resume();
        m_transport.m_isConnected = true;

        if (!m_transmissionQueue.empty()) {
          boost::asio::async_write(m_socket, *m_transmissionQueue.begin(),
                                   bind(&Impl::handleAsyncWrite, this, _1,
                                        m_transmissionQueue.begin()));
        }
      }
    else
      {
        // may need to throw exception
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
  connect(const typename Protocol::endpoint& endpoint)
  {
    if (!m_connectionInProgress) {
      m_connectionInProgress = true;

      // Wait at most 4 seconds to connect
      /// @todo Decide whether this number should be configurable
      m_connectTimer.expires_from_now(boost::posix_time::seconds(4));
      m_connectTimer.async_wait(bind(&Impl::connectTimeoutHandler, this, _1));

      m_socket.open();
      m_socket.async_connect(endpoint,
                             bind(&Impl::connectHandler, this, _1));
    }
  }

  void
  close()
  {
    m_connectionInProgress = false;

    boost::system::error_code error; // to silently ignore all errors
    m_connectTimer.cancel(error);
    m_socket.cancel(error);
    m_socket.close(error);

    m_transport.m_isConnected = false;
    m_transport.m_isExpectingData = false;
    m_transmissionQueue.clear();
  }

  void
  pause()
  {
    if (m_connectionInProgress)
      return;

    if (m_transport.m_isExpectingData)
      {
        m_transport.m_isExpectingData = false;
        m_socket.cancel();
      }
  }

  /**
   * @warning Must not be called directly or indirectly from within handleAsyncReceive invocation
   */
  void
  resume()
  {
    if (m_connectionInProgress)
      return;

    if (!m_transport.m_isExpectingData)
      {
        m_transport.m_isExpectingData = true;
        m_inputBufferSize = 0;
        m_socket.async_receive(boost::asio::buffer(m_inputBuffer, MAX_NDN_PACKET_SIZE), 0,
                               bind(&Impl::handleAsyncReceive, this, _1, _2));
      }
  }

  void
  send(const Block& wire)
  {
    BlockSequence sequence;
    sequence.push_back(wire);
    m_transmissionQueue.push_back(sequence);

    if (m_transport.m_isConnected && m_transmissionQueue.size() == 1) {
      boost::asio::async_write(m_socket, *m_transmissionQueue.begin(),
                               bind(&Impl::handleAsyncWrite, this, _1,
                                    m_transmissionQueue.begin()));
    }

    // if not connected or there is transmission in progress (m_transmissionQueue.size() > 1),
    // next write will be scheduled either in connectHandler or in asyncWriteHandler
  }

  void
  send(const Block& header, const Block& payload)
  {
    BlockSequence sequence;
    sequence.push_back(header);
    sequence.push_back(payload);
    m_transmissionQueue.push_back(sequence);

    if (m_transport.m_isConnected && m_transmissionQueue.size() == 1) {
      boost::asio::async_write(m_socket, *m_transmissionQueue.begin(),
                               bind(&Impl::handleAsyncWrite, this, _1,
                                    m_transmissionQueue.begin()));
    }

    // if not connected or there is transmission in progress (m_transmissionQueue.size() > 1),
    // next write will be scheduled either in connectHandler or in asyncWriteHandler
  }

  void
  handleAsyncWrite(const boost::system::error_code& error,
                   TransmissionQueue::iterator queueItem)
  {
    if (error)
      {
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
      boost::asio::async_write(m_socket, *m_transmissionQueue.begin(),
                               bind(&Impl::handleAsyncWrite, this, _1,
                                    m_transmissionQueue.begin()));
    }
  }

  bool
  processAll(uint8_t* buffer, size_t& offset, size_t nBytesAvailable)
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

  void
  handleAsyncReceive(const boost::system::error_code& error, std::size_t nBytesRecvd)
  {
    if (error)
      {
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
    bool hasProcessedSome = processAll(m_inputBuffer, offset, m_inputBufferSize);
    if (!hasProcessedSome && m_inputBufferSize == MAX_NDN_PACKET_SIZE && offset == 0)
      {
        m_transport.close();
        BOOST_THROW_EXCEPTION(Transport::Error(boost::system::error_code(),
                                               "input buffer full, but a valid TLV cannot be "
                                               "decoded"));
      }

    if (offset > 0)
      {
        if (offset != m_inputBufferSize)
          {
            std::copy(m_inputBuffer + offset, m_inputBuffer + m_inputBufferSize,
                      m_inputBuffer);
            m_inputBufferSize -= offset;
          }
        else
          {
            m_inputBufferSize = 0;
          }
      }

    m_socket.async_receive(boost::asio::buffer(m_inputBuffer + m_inputBufferSize,
                                               MAX_NDN_PACKET_SIZE - m_inputBufferSize), 0,
                           bind(&Impl::handleAsyncReceive, this, _1, _2));
  }

protected:
  BaseTransport& m_transport;

  typename Protocol::socket m_socket;
  uint8_t m_inputBuffer[MAX_NDN_PACKET_SIZE];
  size_t m_inputBufferSize;

  TransmissionQueue m_transmissionQueue;
  bool m_connectionInProgress;

  boost::asio::deadline_timer m_connectTimer;
};


template<class BaseTransport, class Protocol>
class StreamTransportWithResolverImpl : public StreamTransportImpl<BaseTransport, Protocol>
{
public:
  typedef StreamTransportWithResolverImpl<BaseTransport,Protocol> Impl;

  StreamTransportWithResolverImpl(BaseTransport& transport, boost::asio::io_service& ioService)
    : StreamTransportImpl<BaseTransport, Protocol>(transport, ioService)
  {
  }

  void
  resolveHandler(const boost::system::error_code& error,
                 typename Protocol::resolver::iterator endpoint,
                 const shared_ptr<typename Protocol::resolver>&)
  {
    if (error)
      {
        if (error == boost::system::errc::operation_canceled)
          return;

        BOOST_THROW_EXCEPTION(Transport::Error(error, "Error during resolution of host or port"));
      }

    typename Protocol::resolver::iterator end;
    if (endpoint == end)
      {
        this->m_transport.close();
        BOOST_THROW_EXCEPTION(Transport::Error(error, "Unable to resolve because host or port"));
      }

    this->m_socket.async_connect(*endpoint,
                                 bind(&Impl::connectHandler, this, _1));
  }

  void
  connect(const typename Protocol::resolver::query& query)
  {
    if (!this->m_connectionInProgress) {
      this->m_connectionInProgress = true;

      // Wait at most 4 seconds to connect
      /// @todo Decide whether this number should be configurable
      this->m_connectTimer.expires_from_now(boost::posix_time::seconds(4));
      this->m_connectTimer.async_wait(bind(&Impl::connectTimeoutHandler, this, _1));

      // typename boost::asio::ip::basic_resolver< Protocol > resolver;
      shared_ptr<typename Protocol::resolver> resolver =
        make_shared<typename Protocol::resolver>(ref(this->m_socket.get_io_service()));

      resolver->async_resolve(query, bind(&Impl::resolveHandler, this, _1, _2, resolver));
    }
  }
};


} // namespace ndn

#endif // NDN_TRANSPORT_STREAM_TRANSPORT_HPP

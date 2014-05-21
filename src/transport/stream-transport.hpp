/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "../common.hpp"

#include <list>

namespace ndn {

const size_t MAX_LENGTH = 9000;

template<class BaseTransport, class Protocol>
class StreamTransportImpl
{
public:
  typedef BaseTransport base_transport;
  typedef Protocol      protocol;
  typedef StreamTransportImpl<BaseTransport,Protocol> impl;

  StreamTransportImpl(base_transport& transport, boost::asio::io_service& ioService)
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

        for (std::list<Block>::iterator i = m_sendQueue.begin(); i != m_sendQueue.end(); ++i)
          m_socket.async_send(boost::asio::buffer(i->wire(), i->size()),
                              bind(&impl::handle_async_send, this, _1, *i));

        for (std::list< std::pair<Block,Block> >::iterator i = m_sendPairQueue.begin();
             i != m_sendPairQueue.end(); ++i)
          {
            std::vector<boost::asio::const_buffer> buffer;
            buffer.reserve(2);
            buffer.push_back(boost::asio::buffer(i->first.wire(),  i->first.size()));
            buffer.push_back(boost::asio::buffer(i->second.wire(), i->second.size()));
            m_socket.async_send(buffer,
                                bind(&impl::handle_async_send2, this, _1, i->first, i->second));
          }

        m_sendQueue.clear();
        m_sendPairQueue.clear();
      }
    else
      {
        // may need to throw exception
        m_transport.m_isConnected = false;
        m_transport.close();
        throw Transport::Error(error, "error while connecting to the forwarder");
      }
  }

  void
  connectTimeoutHandler(const boost::system::error_code& error)
  {
    if (error) // e.g., cancelled timer
      return;

    m_connectionInProgress = false;
    m_transport.m_isConnected = false;
    m_transport.m_isExpectingData = false;
    m_socket.cancel();
    m_socket.close();
    throw Transport::Error(error, "error while connecting to the forwarder");
  }

  void
  connect(const typename protocol::endpoint& endpoint)
  {
    if (!m_connectionInProgress) {
      m_connectionInProgress = true;

      // Wait at most 4 time::seconds to connect
      /// @todo Decide whether this number should be configurable
      m_connectTimer.expires_from_now(boost::posix_time::seconds(4));
      m_connectTimer.async_wait(bind(&impl::connectTimeoutHandler, this, _1));

      m_socket.open();
      m_socket.async_connect(endpoint,
                             bind(&impl::connectHandler, this, _1));
    }
  }

  void
  close()
  {
    boost::system::error_code error; // to silently ignore all errors
    m_connectTimer.cancel(error);
    m_socket.cancel(error);
    m_socket.close(error);

    m_transport.m_isConnected = false;
    m_transport.m_isExpectingData = false;
    m_sendQueue.clear();
    m_sendPairQueue.clear();
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

  void
  resume()
  {
    if (m_connectionInProgress)
      return;

    if (!m_transport.m_isExpectingData)
      {
        m_transport.m_isExpectingData = true;
        m_inputBufferSize = 0;
        m_socket.async_receive(boost::asio::buffer(m_inputBuffer, MAX_LENGTH), 0,
                               bind(&impl::handle_async_receive, this, _1, _2));
      }
  }

  void
  send(const Block& wire)
  {
    if (!m_transport.m_isConnected)
      m_sendQueue.push_back(wire);
    else
      m_socket.async_send(boost::asio::buffer(wire.wire(), wire.size()),
                         bind(&impl::handle_async_send, this, _1, wire));
  }

  void
  send(const Block& header, const Block& payload)
  {
    if (!m_transport.m_isConnected)
      {
        m_sendPairQueue.push_back(std::make_pair(header, payload));
      }
    else
      {
        std::vector<boost::asio::const_buffer> buffers;
        buffers.reserve(2);
        buffers.push_back(boost::asio::buffer(header.wire(),  header.size()));
        buffers.push_back(boost::asio::buffer(payload.wire(), payload.size()));

        m_socket.async_send(buffers,
                            bind(&impl::handle_async_send2, this, _1, header, payload));
      }
  }

  inline bool
  processAll(uint8_t* buffer, size_t& offset, size_t availableSize)
  {
    Block element;
    while(offset < availableSize)
      {
        bool ok = Block::fromBuffer(buffer + offset, availableSize - offset, element);
        if (!ok)
          return false;

        m_transport.receive(element);
        offset += element.size();
      }
    return true;
  }

  void
  handle_async_receive(const boost::system::error_code& error, std::size_t bytes_recvd)
  {
    if (error)
      {
        if (error == boost::system::errc::operation_canceled) {
          // async receive has been explicitly cancelled (e.g., socket close)
          return;
        }

        boost::system::error_code error; // to silently ignore all errors
        m_socket.cancel(error);
        m_socket.close(error); // closing at this point may not be that necessary
        m_transport.m_isConnected = true;
        m_transport.m_isExpectingData = false;
        throw Transport::Error(error, "error while receiving data from socket");
      }

    m_inputBufferSize += bytes_recvd;
    // do magic

    std::size_t offset = 0;
    bool ok = processAll(m_inputBuffer, offset, m_inputBufferSize);
    if (!ok && m_inputBufferSize == MAX_LENGTH && offset == 0)
      {
        // very bad... should close connection
        boost::system::error_code error; // to silently ignore all errors
        m_socket.cancel(error);
        m_socket.close(error);
        m_transport.m_isConnected = false;
        m_transport.m_isExpectingData = false;
        throw Transport::Error(boost::system::error_code(),
                               "input buffer full, but a valid TLV cannot be decoded");
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
                                               MAX_LENGTH - m_inputBufferSize), 0,
                           bind(&impl::handle_async_receive, this, _1, _2));
  }

  void
  handle_async_send(const boost::system::error_code& error, const Block& wire)
  {
    // pass (needed to keep data block alive during the send)
  }

  void
  handle_async_send2(const boost::system::error_code& error,
                     const Block& header, const Block& payload)
  {
    // pass (needed to keep data blocks alive during the send)
  }

protected:
  base_transport& m_transport;

  typename protocol::socket m_socket;
  uint8_t m_inputBuffer[MAX_LENGTH];
  size_t m_inputBufferSize;

  std::list< Block > m_sendQueue;
  std::list< std::pair<Block, Block> > m_sendPairQueue;
  bool m_connectionInProgress;

  boost::asio::deadline_timer m_connectTimer;
};


template<class BaseTransport, class Protocol>
class StreamTransportWithResolverImpl : public StreamTransportImpl<BaseTransport, Protocol>
{
public:
  typedef BaseTransport base_transport;
  typedef Protocol      protocol;
  typedef StreamTransportWithResolverImpl<BaseTransport,Protocol> impl;

  StreamTransportWithResolverImpl(base_transport& transport, boost::asio::io_service& ioService)
    : StreamTransportImpl<base_transport, protocol>(transport, ioService)
  {
  }

  void
  resolveHandler(const boost::system::error_code& error,
                 typename protocol::resolver::iterator endpoint,
                 const shared_ptr<typename protocol::resolver>&)
  {
    if (error)
      {
        if (error == boost::system::errc::operation_canceled)
          return;

        throw Transport::Error(error, "Error during resolution of host or port");
      }

    typename protocol::resolver::iterator end;
    if (endpoint == end)
      {
        this->m_connectionInProgress = false;
        this->m_transport.m_isConnected = false;
        this->m_transport.m_isExpectingData = false;
        this->m_socket.close();
        throw Transport::Error(error, "Unable to resolve because host or port");
      }

    this->m_socket.async_connect(*endpoint,
                                 bind(&impl::connectHandler, this, _1));
  }

  void
  connect(const typename protocol::resolver::query& query)
  {
    if (!this->m_connectionInProgress) {
      this->m_connectionInProgress = true;

      // Wait at most 4 time::seconds to connect
      /// @todo Decide whether this number should be configurable
      this->m_connectTimer.expires_from_now(boost::posix_time::seconds(4));
      this->m_connectTimer.async_wait(bind(&impl::connectTimeoutHandler, this, _1));

      // typename boost::asio::ip::basic_resolver< protocol > resolver;
      shared_ptr<typename protocol::resolver> resolver =
        make_shared<typename protocol::resolver>(ref(this->m_socket.get_io_service()));

      resolver->async_resolve(query, bind(&impl::resolveHandler, this, _1, _2, resolver));
    }
  }
};


} // namespace ndn

#endif // NDN_TRANSPORT_STREAM_TRANSPORT_HPP

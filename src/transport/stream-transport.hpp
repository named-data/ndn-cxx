/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013-2014 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TRANSPORT_STREAM_TRANSPORT_HPP
#define NDN_TRANSPORT_STREAM_TRANSPORT_HPP

#include "../common.hpp"

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
    , m_partialDataSize(0)
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
        m_partialDataSize = 0;
        m_socket.async_receive(boost::asio::buffer(m_inputBuffer, MAX_LENGTH), 0,
                               bind(&impl::handle_async_receive, this, _1, _2));

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
                                bind(&impl::handle_async_send, this, _1, i->first, i->second));
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
    m_socket.close();
    throw Transport::Error(error, "error while connecting to the forwarder");
  }

  void
  connect(const typename protocol::endpoint& endpoint)
  {
    if (!m_connectionInProgress) {
      m_connectionInProgress = true;

      // Wait at most 4 seconds to connect
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
    m_connectTimer.cancel();
    m_socket.close();
    m_transport.m_isConnected = false;
    m_sendQueue.clear();
    m_sendPairQueue.clear();
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
                            bind(&impl::handle_async_send, this, _1, header, payload));
      }
  }
  
  inline void
  processAll(uint8_t* buffer, size_t& offset, size_t availableSize)
  {
    while(offset < availableSize)
      {
        Block element(buffer + offset, availableSize - offset);
        m_transport.receive(element);

        offset += element.size();
      }
  }
  
  void
  handle_async_receive(const boost::system::error_code& error, std::size_t bytes_recvd)
  {
    /// @todo The socket is not datagram, so need to have internal buffer to handle partial data reception

    if (error)
      {
        if (error == boost::system::errc::operation_canceled) {
          // async receive has been explicitly cancelled (e.g., socket close)
          return;
        }
        
        m_socket.close(); // closing at this point may not be that necessary
        m_transport.m_isConnected = true;
        throw Transport::Error(error, "error while receiving data from socket");
      }
    
    if (!error && bytes_recvd > 0)
      {
        // m_inputBuffer has bytes_recvd received bytes of data
        if (m_partialDataSize > 0)
          {
            size_t newDataSize = std::min(bytes_recvd, MAX_LENGTH-m_partialDataSize);
            std::copy(m_inputBuffer, m_inputBuffer + newDataSize, m_partialData + m_partialDataSize);

            m_partialDataSize += newDataSize;
              
            size_t offset = 0;
            try
              {
                processAll(m_partialData, offset, m_partialDataSize);

                // no exceptions => processed the whole thing
                if (bytes_recvd - newDataSize > 0)
                  {
                    // there is a little bit more data available
                        
                    offset = 0;
                    m_partialDataSize = bytes_recvd - newDataSize;
                    std::copy(m_inputBuffer + newDataSize, m_inputBuffer + newDataSize + m_partialDataSize, m_partialData);

                    processAll(m_partialData, offset, m_partialDataSize);

                    // no exceptions => processed the whole thing
                    m_partialDataSize = 0;
                  }
                else
                  {
                    // done processing
                    m_partialDataSize = 0;
                  }
              }
            catch(Tlv::Error &)
              {
                if (offset > 0)
                  {
                    m_partialDataSize -= offset;
                    std::copy(m_partialData + offset, m_partialData + offset + m_partialDataSize, m_partialData);
                  }
                else if (offset == 0 && m_partialDataSize == MAX_LENGTH)
                  {
                    // very bad... should close connection
                    m_socket.close();
                    m_transport.m_isConnected = true;
                    throw Transport::Error(boost::system::error_code(),
                                           "input buffer full, but a valid TLV cannot be decoded");
                  }
              }
          }
        else
          {
            size_t offset = 0;
            try
              {
                processAll(m_inputBuffer, offset, bytes_recvd);
              }
            catch(Tlv::Error &error)
              {
                if (offset > 0)
                  {
                    m_partialDataSize = bytes_recvd - offset;
                    std::copy(m_inputBuffer + offset, m_inputBuffer + offset + m_partialDataSize, m_partialData);
                  }
              }
          }
      }

    m_socket.async_receive(boost::asio::buffer(m_inputBuffer, MAX_LENGTH), 0,
                          bind(&impl::handle_async_receive, this, _1, _2));
  }

  void
  handle_async_send(const boost::system::error_code& error, const Block& wire)
  {
    // pass (needed to keep data block alive during the send)
  }

  void
  handle_async_send(const boost::system::error_code& error,
                    const Block& header, const Block& payload)
  {
    // pass (needed to keep data blocks alive during the send)
  }

protected:
  base_transport& m_transport;
  
  typename protocol::socket m_socket;
  uint8_t m_inputBuffer[MAX_LENGTH];

  uint8_t m_partialData[MAX_LENGTH];
  size_t m_partialDataSize;

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

      // Wait at most 4 seconds to connect
      /// @todo Decide whether this number should be configurable
      this->m_connectTimer.expires_from_now(boost::posix_time::seconds(4));
      this->m_connectTimer.async_wait(bind(&impl::connectTimeoutHandler, this, _1));

      // typename boost::asio::ip::basic_resolver< protocol > resolver;
      shared_ptr<typename protocol::resolver> resolver =
        make_shared<typename protocol::resolver>(boost::ref(this->m_socket.get_io_service()));

      resolver->async_resolve(query, bind(&impl::resolveHandler, this, _1, _2, resolver));
    }
  }
};


} // namespace ndn

#endif // NDN_TRANSPORT_STREAM_TRANSPORT_HPP

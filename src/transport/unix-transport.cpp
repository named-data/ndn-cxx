/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include <stdlib.h>

#include <ndn-cpp/face.hpp>
#include <ndn-cpp/transport/unix-transport.hpp>
#include <ndn-cpp/c/util/ndn_memory.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace std;
typedef boost::asio::local::stream_protocol protocol;

namespace ndn {

const size_t MAX_LENGTH = 9000;

class UnixTransport::Impl
{
public:
  Impl(UnixTransport &transport)
    : transport_(transport)
    , socket_(*transport_.ioService_)
    , partialDataSize_(0)
  {
  }

  void
  connect()
  {
    socket_.open();
    socket_.connect(protocol::endpoint(transport_.unixSocket_));
    // socket_.async_connect(protocol::endpoint(unixSocket));

    partialDataSize_ = 0;
    socket_.async_receive(boost::asio::buffer(inputBuffer_, MAX_LENGTH), 0,
                          boost::bind(&Impl::handle_async_receive, this, _1, _2));
  }

  void 
  close()
  {
    socket_.close();
  }

  void 
  send(const Block &wire)
  {
    socket_.async_send(boost::asio::buffer(wire.wire(), wire.size()),
                       boost::bind(&Impl::handle_async_send, this, _1, wire));
  }

  inline void
  processAll(uint8_t *buffer, size_t &offset, size_t availableSize)
  {
    while(offset < availableSize)
      {
        Block element(buffer + offset, availableSize - offset);
        transport_.receive(element);

        offset += element.size();
      }
  }
  
  void
  handle_async_receive(const boost::system::error_code& error, std::size_t bytes_recvd)
  {
    /// @todo The socket is not datagram, so need to have internal buffer to handle partial data reception

    if (!error && bytes_recvd > 0)
      {
        try
          {
            // inputBuffer_ has bytes_recvd received bytes of data
            if (partialDataSize_ > 0)
              {
                size_t newDataSize = std::min(bytes_recvd, MAX_LENGTH-partialDataSize_);
                ndn_memcpy(partialData_ + partialDataSize_, inputBuffer_, newDataSize);
                partialDataSize_ += newDataSize;
              
                size_t offset = 0;
                try
                  {
                    processAll(partialData_, offset, partialDataSize_);

                    // no exceptions => processed the whole thing
                    if (bytes_recvd - newDataSize > 0)
                      {
                        // there is a little bit more data available
                        
                        offset = 0;
                        partialDataSize_ = bytes_recvd - newDataSize;
                        ndn_memcpy(partialData_, inputBuffer_ + newDataSize, partialDataSize_);

                        processAll(partialData_, offset, partialDataSize_);

                        // no exceptions => processed the whole thing
                        partialDataSize_ = 0;
                      }
                    else
                      {
                        // done processing
                        partialDataSize_ = 0;
                      }
                  }
                catch(Block::Error &)
                  {
                    if (offset > 0)
                      {
                        partialDataSize_ -= offset;
                        ndn_memcpy(partialData_, partialData_ + offset, partialDataSize_);
                      }
                    else if (offset == 0 && partialDataSize_ == MAX_LENGTH)
                      {
                        // very bad... should close connection
                        /// @todo Notify somebody 
                        socket_.close();
                      }
                  }
              }
            else
              {
                size_t offset = 0;
                try
                  {
                    processAll(inputBuffer_, offset, bytes_recvd);
                  }
                catch(Block::Error &error)
                  {
                    if (offset > 0)
                      {
                        partialDataSize_ = bytes_recvd - offset;
                        ndn_memcpy(partialData_, inputBuffer_ + offset, partialDataSize_);
                      }
                  }
              }
          }
        catch(Tlv::Error &error)
          {
            std::cerr << "[[handle_async_receive]] Tlv::Error: " << error.what() << std::endl;
            // pass
          }
      }

    socket_.async_receive(boost::asio::buffer(inputBuffer_, MAX_LENGTH), 0,
                          boost::bind(&Impl::handle_async_receive, this, _1, _2));
  }

  void
  handle_async_send(const boost::system::error_code& error, const Block &wire)
  {
    // pass (needed to keep data block alive during the send)
  }
  
private:
  UnixTransport &transport_;
  
  protocol::socket socket_;
  uint8_t inputBuffer_[MAX_LENGTH];

  uint8_t partialData_[MAX_LENGTH];
  size_t partialDataSize_;
};

UnixTransport::UnixTransport(const std::string &unixSocket/* = "/tmp/.ndnd.sock"*/) 
  : unixSocket_(unixSocket)
{
}

UnixTransport::~UnixTransport()
{
}

void 
UnixTransport::connect(boost::asio::io_service &ioService, const ReceiveCallback &receiveCallback)
{
  Transport::connect(ioService, receiveCallback);
  
  impl_ = std::auto_ptr<UnixTransport::Impl> (new UnixTransport::Impl(*this));
  impl_->connect();
  
  isConnected_ = true;
}

void 
UnixTransport::send(const Block &wire)
{
  impl_->send(wire);
}

void 
UnixTransport::close()
{
  impl_->close();
}

}

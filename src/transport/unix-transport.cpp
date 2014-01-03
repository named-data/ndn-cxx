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
    : transport_(transport),
      socket_(*transport_.ioService_)
  {
  }

  void
  connect()
  {
    socket_.open();
    socket_.connect(protocol::endpoint(transport_.unixSocket_));
    // socket_.async_connect(protocol::endpoint(unixSocket));

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
  
  void
  handle_async_receive(const boost::system::error_code& error, std::size_t bytes_recvd)
  {
    /// @todo The socket is not datagram, so need to have internal buffer to handle partial data reception
    
    if (!error && bytes_recvd > 0)
      {
        // inputBuffer_ has bytes_recvd received bytes of data
        try {
          Block element(inputBuffer_, bytes_recvd);
          transport_.receive(element);
        }
        catch(Tlv::Error &error)
          {
            // pass
          }
        catch(Block::Error &error)
          {
            // pass
          }
      }

    socket_.async_receive(boost::asio::buffer(inputBuffer_, MAX_LENGTH), 0,
                          boost::bind(&Impl::handle_async_receive, this, _1, _2));
  }

  void
  handle_async_send(const boost::system::error_code& error, const Block &wire)
  {
    // pass
  }
  
private:
  UnixTransport &transport_;
  
  protocol::socket socket_;
  uint8_t inputBuffer_[MAX_LENGTH];
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

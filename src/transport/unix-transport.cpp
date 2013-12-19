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
typedef boost::asio::local::datagram_protocol protocol;

namespace ndn {

const size_t MAX_LENGTH = 9000;

class UnixTransport::Impl
{
public:
  Impl() : socket_(io_)
  {
  }

  bool
  connect(const std::string &unixSocket, ElementListener& elementListener)
  {
    socket_.open();
    socket_.connect(protocol::endpoint(unixSocket));
    // socket_.async_connect(protocol::endpoint(unixSocket));

    socket_.async_receive(boost::asio::buffer(inputBuffer_, MAX_LENGTH), 0,
                          boost::bind(&Impl::handle_async_receive, this, _1, _2));

    return true;
  }

  void 
  send(const uint8_t *data, size_t dataLength)
  {
    socket_.send(boost::asio::buffer(data, dataLength));
  }

  void
  processEvents()
  {
    io_.poll();
    // from boost docs:
    // The poll() function runs handlers that are ready to run, without blocking, until the io_service has been stopped or there are no more ready handlers.
  }

  void
  handle_async_receive(const boost::system::error_code& error, std::size_t bytes_recvd)
  {
    if (!error && bytes_recvd > 0)
      {
        // inputBuffer_ has bytes_recvd received bytes of data
      }

    socket_.async_receive(boost::asio::buffer(inputBuffer_, MAX_LENGTH), 0,
                          boost::bind(&Impl::handle_async_receive, this, _1, _2));
  }
  
  void 
  close()
  {
    socket_.close();
  }
  
private:
  boost::asio::io_service io_;

  protocol::socket socket_;

  uint8_t inputBuffer_[MAX_LENGTH];
};

UnixTransport::UnixTransport(const std::string &unixSocket/* = "/tmp/.ndnd.sock"*/) 
  : unixSocket_(unixSocket)
  , isConnected_(false)
  , impl_(new UnixTransport::Impl())
{
}

UnixTransport::~UnixTransport()
{
}

void 
UnixTransport::connect(ElementListener& elementListener)
{
  if (impl_->connect(unixSocket_, elementListener))
    {
      isConnected_ = true;
    }
}

void 
UnixTransport::send(const uint8_t *data, size_t dataLength)
{
  impl_->send(data, dataLength);
}

void 
UnixTransport::processEvents()
{
  impl_->processEvents();
}

bool 
UnixTransport::getIsConnected()
{
  return isConnected_;
}

void 
UnixTransport::close()
{
  impl_->close();
}

}

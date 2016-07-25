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

#ifndef NDN_TRANSPORT_TRANSPORT_HPP
#define NDN_TRANSPORT_TRANSPORT_HPP

#include "../common.hpp"
#include "../encoding/block.hpp"

#include <boost/system/error_code.hpp>

namespace boost {
namespace asio {
class io_service;
} // namespace asio
} // namespace boost

namespace ndn {

/** \brief provides TLV-block delivery service
 */
class Transport : noncopyable
{
public:
  class Error : public std::runtime_error
  {
  public:
    Error(const boost::system::error_code& code, const std::string& msg);

    explicit
    Error(const std::string& msg);
  };

  typedef function<void(const Block& wire)> ReceiveCallback;
  typedef function<void()> ErrorCallback;

  Transport();

  virtual
  ~Transport() = default;

  /** \brief asynchronously open the connection
   *  \param ioService io_service to create socket on
   *  \param receiveCallback callback function when a TLV block is received; must not be empty
   *  \throw boost::system::system_error connection cannot be established
   */
  virtual void
  connect(boost::asio::io_service& ioService, const ReceiveCallback& receiveCallback);

  /** \brief Close the connection.
   */
  virtual void
  close() = 0;

  /** \brief send a TLV block through the transport
   */
  virtual void
  send(const Block& wire) = 0;

  /** \brief send two memory blocks through the transport
   *
   *  Scatter/gather API is utilized to send two non-consecutive memory blocks together
   *  (as part of the same message in datagram-oriented transports).
   */
  virtual void
  send(const Block& header, const Block& payload) = 0;

  /** \brief pause the transport
   *  \post receiveCallback will not be invoked
   *  \note This operation has no effect if transport has been paused,
   *        or when connection is being established.
   */
  virtual void
  pause() = 0;

  /** \brief resume the transport
   *  \post receiveCallback will be invoked
   *  \note This operation has no effect if transport is not paused,
   *        or when connection is being established.
   */
  virtual void
  resume() = 0;

  /** \retval true connection has been established
   *  \retval false connection is not yet established or has been closed
   */
  bool
  isConnected() const;

  /** \retval true incoming packets are expected, receiveCallback will be invoked
   *  \retval false incoming packets are not expected, receiveCallback will not be invoked
   */
  bool
  isReceiving() const;

protected:
  /** \brief invoke the receive callback
   */
  void
  receive(const Block& wire);

protected:
  boost::asio::io_service* m_ioService;
  bool m_isConnected;
  bool m_isReceiving;
  ReceiveCallback m_receiveCallback;
};

inline bool
Transport::isConnected() const
{
  return m_isConnected;
}

inline bool
Transport::isReceiving() const
{
  return m_isReceiving;
}

inline void
Transport::receive(const Block& wire)
{
  m_receiveCallback(wire);
}

} // namespace ndn

#endif // NDN_TRANSPORT_TRANSPORT_HPP

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

#ifndef NDN_TRANSPORT_TRANSPORT_HPP
#define NDN_TRANSPORT_TRANSPORT_HPP

#include "../common.hpp"
#include "../encoding/block.hpp"

#include <boost/asio.hpp>

namespace ndn {

class Transport : noncopyable
{
public:
  class Error : public std::runtime_error
  {
  public:
    inline Error(const boost::system::error_code& code, const std::string& msg);
    inline Error(const std::string& msg);
  };

  typedef function<void (const Block& wire)> ReceiveCallback;
  typedef function<void ()> ErrorCallback;

  inline
  Transport();

  inline virtual
  ~Transport();

  /**
   * @brief Connect transport
   *
   * @throws boost::system::system_error if connection cannot be established
   */
  inline virtual void
  connect(boost::asio::io_service& io_service,
          const ReceiveCallback& receiveCallback);

  /**
   * @brief Close the connection.
   */
  virtual void
  close() = 0;

  /**
   * @brief Send block of data from @param wire through the transport
   *
   * @param wire A block of data to send
   */
  virtual void
  send(const Block& wire) = 0;

  /**
   * @brief Alternative version of sending data, applying scatter/gather I/O concept
   *
   * Two non-consecutive memory blocks will be send out together, e.g., as part of the
   * same message in datagram-oriented transports.
   */
  virtual void
  send(const Block& header, const Block& payload) = 0;

  virtual void
  pause() = 0;

  virtual void
  resume() = 0;

  inline bool
  isConnected();

  inline bool
  isExpectingData();

protected:
  inline void
  receive(const Block& wire);

protected:
  boost::asio::io_service* m_ioService;
  bool m_isConnected;
  bool m_isExpectingData;
  ReceiveCallback m_receiveCallback;
};

inline
Transport::Transport()
  : m_ioService(0)
  , m_isConnected(false)
  , m_isExpectingData(false)
{
}

inline
Transport::Error::Error(const boost::system::error_code& code, const std::string& msg)
  : std::runtime_error(msg + (code.value() ? " (" + code.category().message(code.value()) + ")" : ""))
{
}

inline
Transport::Error::Error(const std::string& msg)
  : std::runtime_error(msg)
{
}

inline
Transport::~Transport()
{
}

inline void
Transport::connect(boost::asio::io_service& ioService,
                   const ReceiveCallback& receiveCallback)
{
  m_ioService = &ioService;
  m_receiveCallback = receiveCallback;
}

inline bool
Transport::isConnected()
{
  return m_isConnected;
}

inline bool
Transport::isExpectingData()
{
  return m_isExpectingData;
}

inline void
Transport::receive(const Block& wire)
{
  m_receiveCallback(wire);
}

} // namespace ndn

#endif // NDN_TRANSPORT_TRANSPORT_HPP

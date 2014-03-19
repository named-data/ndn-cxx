/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TRANSPORT_TRANSPORT_HPP
#define NDN_TRANSPORT_TRANSPORT_HPP

#include "../common.hpp"
#include "../encoding/block.hpp"

namespace ndn {

class Transport {
public:
  class Error : public std::runtime_error
  {
  public:
    inline Error(const boost::system::error_code &code, const std::string &msg);
    inline Error(const std::string& msg);
  };
  
  typedef ptr_lib::function<void (const Block &wire)> ReceiveCallback;
  typedef ptr_lib::function<void ()> ErrorCallback;
  
  inline
  Transport();
  
  inline virtual
  ~Transport();

  /**
   * Connect transport
   *
   * @throws If connection cannot be established
   */
  inline virtual void 
  connect(boost::asio::io_service& io_service,
          const ReceiveCallback& receiveCallback);
  
  /**
   * Close the connection.
   */
  virtual void 
  close() = 0;

  /**
   * @brief Set data to the host
   *
   * @param data A pointer to the buffer of data to send.
   * @param dataLength The number of bytes in data.
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

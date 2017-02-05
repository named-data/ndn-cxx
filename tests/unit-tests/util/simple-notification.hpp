/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2017 Regents of the University of California,
 *                         Arizona Board of Regents,
 *                         Colorado State University,
 *                         University Pierre & Marie Curie, Sorbonne University,
 *                         Washington University in St. Louis,
 *                         Beijing Institute of Technology,
 *                         The University of Memphis.
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

#ifndef NDN_TESTS_UNIT_TESTS_UTIL_SIMPLE_NOTIFICATION_HPP
#define NDN_TESTS_UNIT_TESTS_UTIL_SIMPLE_NOTIFICATION_HPP

#include "common.hpp"

#include "encoding/encoding-buffer.hpp"

namespace ndn {
namespace util {
namespace tests {

class SimpleNotification
{
public:
  SimpleNotification() = default;

  explicit
  SimpleNotification(const Block& block)
  {
    wireDecode(block);
  }

  SimpleNotification(const std::string& message)
    : m_message(message)
  {
  }

  Block
  wireEncode() const
  {
    ndn::EncodingBuffer buffer;
    buffer.prependByteArrayBlock(0x8888,
                                 reinterpret_cast<const uint8_t*>(m_message.c_str()),
                                 m_message.size());
    return buffer.block();
  }

  void
  wireDecode(const Block& block)
  {
    m_message.assign(reinterpret_cast<const char*>(block.value()),
                     block.value_size());

    // error for testing
    if (!m_message.empty() && m_message[0] == '\x07')
      BOOST_THROW_EXCEPTION(tlv::Error("0x07 error"));
  }

  const std::string&
  getMessage() const
  {
    return m_message;
  }

  void
  setMessage(const std::string& message)
  {
    m_message = message;
  }

private:
  std::string m_message;
};

} // namespace tests
} // namespace util
} // namespace ndn

#endif // NDN_TESTS_UNIT_TESTS_UTIL_SIMPLE_NOTIFICATION_HPP

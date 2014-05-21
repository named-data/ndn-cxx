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

#ifndef NDN_MANAGEMENT_CONTROL_RESPONSE_HPP
#define NDN_MANAGEMENT_CONTROL_RESPONSE_HPP

#include "../encoding/block.hpp"
#include "../encoding/block-helpers.hpp"
#include "../encoding/tlv-nfd.hpp"

namespace ndn {
namespace nfd {

/**
 * @ingroup management
 * @brief Class defining abstraction of ControlResponse for NFD Control Protocol
 *
 * @see http://redmine.named-data.net/projects/nfd/wiki/ControlCommand
 */
class ControlResponse
{
public:
  class Error : public Tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Tlv::Error(what)
    {
    }
  };

  ControlResponse()
    : m_code(200)
  {
  }

  ControlResponse(uint32_t code, const std::string& text)
    : m_code(code)
    , m_text(text)
  {
  }

  ControlResponse(const Block& block)
  {
    wireDecode(block);
  }

  inline uint32_t
  getCode() const;

  inline void
  setCode(uint32_t code);

  inline const std::string&
  getText() const;

  inline void
  setText(const std::string& text);

  inline const Block&
  getBody() const;

  inline void
  setBody(const Block& body);

  inline const Block&
  wireEncode() const;

  inline void
  wireDecode(const Block& block);

protected:
  uint32_t m_code;
  std::string m_text;
  Block m_body;

  mutable Block m_wire;
};

inline uint32_t
ControlResponse::getCode() const
{
  return m_code;
}

inline void
ControlResponse::setCode(uint32_t code)
{
  m_code = code;
  m_wire.reset();
}

inline const std::string&
ControlResponse::getText() const
{
  return m_text;
}

inline void
ControlResponse::setText(const std::string& text)
{
  m_text = text;
  m_wire.reset();
}

inline const Block&
ControlResponse::getBody() const
{
  return m_body;
}

inline void
ControlResponse::setBody(const Block& body)
{
  m_body = body;
  m_body.encode(); // will do nothing if already encoded
  m_wire.reset();
}


inline const Block&
ControlResponse::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  m_wire = Block(tlv::nfd::ControlResponse);
  m_wire.push_back
    (nonNegativeIntegerBlock(tlv::nfd::StatusCode, m_code));

  m_wire.push_back
    (dataBlock(tlv::nfd::StatusText, m_text.c_str(), m_text.size()));

  if (m_body.hasWire())
    {
      m_wire.push_back(m_body);
    }

  m_wire.encode();
  return m_wire;
}

inline void
ControlResponse::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  if (m_wire.type() != tlv::nfd::ControlResponse)
    throw Error("Requested decoding of ControlResponse, but Block is of different type");

  Block::element_const_iterator val = m_wire.elements_begin();
  if (val == m_wire.elements_end() ||
      val->type() != tlv::nfd::StatusCode)
    {
      throw Error("Incorrect ControlResponse format (StatusCode missing or not the first item)");
    }

  m_code = readNonNegativeInteger(*val);
  ++val;

  if (val == m_wire.elements_end() ||
      val->type() != tlv::nfd::StatusText)
    {
      throw Error("Incorrect ControlResponse format (StatusText missing or not the second item)");
    }
  m_text.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
  ++val;

  if (val != m_wire.elements_end())
    m_body = *val;
  else
    m_body = Block();
}

inline std::ostream&
operator << (std::ostream& os, const ControlResponse& status)
{
  os << status.getCode() << " " << status.getText();
  return os;
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_CONTROL_RESPONSE_HPP

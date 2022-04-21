/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/mgmt/control-response.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"
#include "ndn-cxx/encoding/tlv-nfd.hpp"

namespace ndn {
namespace mgmt {

// BOOST_CONCEPT_ASSERT((boost::EqualityComparable<ControlResponse>));
BOOST_CONCEPT_ASSERT((WireEncodable<ControlResponse>));
BOOST_CONCEPT_ASSERT((WireDecodable<ControlResponse>));
static_assert(std::is_base_of<tlv::Error, ControlResponse::Error>::value,
              "ControlResponse::Error must inherit from tlv::Error");

ControlResponse::ControlResponse()
  : m_code(200)
{
}

ControlResponse::ControlResponse(uint32_t code, const std::string& text)
  : m_code(code)
  , m_text(text)
{
}

ControlResponse::ControlResponse(const Block& block)
{
  wireDecode(block);
}

const Block&
ControlResponse::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  m_wire = Block(tlv::nfd::ControlResponse);
  m_wire.push_back(makeNonNegativeIntegerBlock(tlv::nfd::StatusCode, m_code));
  m_wire.push_back(makeStringBlock(tlv::nfd::StatusText, m_text));

  if (m_body.hasWire()) {
    m_wire.push_back(m_body);
  }

  m_wire.encode();
  return m_wire;
}

void
ControlResponse::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  if (m_wire.type() != tlv::nfd::ControlResponse)
    NDN_THROW(Error("ControlResponse", m_wire.type()));

  auto val = m_wire.elements_begin();
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::StatusCode) {
    NDN_THROW(Error("missing StatusCode sub-element"));
  }
  m_code = readNonNegativeIntegerAs<uint32_t>(*val);
  ++val;

  if (val == m_wire.elements_end() || val->type() != tlv::nfd::StatusText) {
    NDN_THROW(Error("missing StatusText sub-element"));
  }
  m_text = readString(*val);
  ++val;

  if (val != m_wire.elements_end())
    m_body = *val;
  else
    m_body = {};
}

std::ostream&
operator<<(std::ostream& os, const ControlResponse& response)
{
  os << response.getCode() << " " << response.getText();
  return os;
}

} // namespace mgmt
} // namespace ndn

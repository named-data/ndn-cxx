/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2015,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "control-response.hpp"
#include "../encoding/block-helpers.hpp"
#include "../encoding/tlv-nfd.hpp"

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
  m_wire.push_back(nonNegativeIntegerBlock(tlv::nfd::StatusCode, m_code));

  m_wire.push_back(dataBlock(tlv::nfd::StatusText, m_text.c_str(), m_text.size()));

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
    throw Error("Requested decoding of ControlResponse, but Block is of different type");

  Block::element_const_iterator val = m_wire.elements_begin();
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::StatusCode) {
    throw Error("Incorrect ControlResponse format (StatusCode missing or not the first item)");
  }

  m_code = readNonNegativeInteger(*val);
  ++val;

  if (val == m_wire.elements_end() || val->type() != tlv::nfd::StatusText) {
    throw Error("Incorrect ControlResponse format (StatusText missing or not the second item)");
  }
  m_text.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
  ++val;

  if (val != m_wire.elements_end())
    m_body = *val;
  else
    m_body = Block();
}

std::ostream&
operator<<(std::ostream& os, const ControlResponse& response)
{
  os << response.getCode() << " " << response.getText();
  return os;
}

} // namespace mgmt
} // namespace ndn

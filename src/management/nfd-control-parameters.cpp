/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "nfd-control-parameters.hpp"
#include "encoding/tlv-nfd.hpp"
#include "encoding/block-helpers.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace nfd {

//BOOST_CONCEPT_ASSERT((boost::EqualityComparable<ControlParameters>));
BOOST_CONCEPT_ASSERT((WireEncodable<ControlParameters>));
BOOST_CONCEPT_ASSERT((WireDecodable<ControlParameters>));
static_assert(std::is_base_of<tlv::Error, ControlParameters::Error>::value,
              "ControlParameters::Error must inherit from tlv::Error");

ControlParameters::ControlParameters()
  : m_hasFields(CONTROL_PARAMETER_UBOUND)
{
}

ControlParameters::ControlParameters(const Block& block)
  : m_hasFields(CONTROL_PARAMETER_UBOUND)
{
  wireDecode(block);
}

template<encoding::Tag TAG>
size_t
ControlParameters::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  if (this->hasExpirationPeriod()) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::ExpirationPeriod, m_expirationPeriod.count());
  }
  if (this->hasStrategy()) {
    totalLength += prependNestedBlock(encoder, tlv::nfd::Strategy, m_strategy);
  }
  if (this->hasFlags()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Flags, m_flags);
  }
  if (this->hasCost()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Cost, m_cost);
  }
  if (this->hasOrigin()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Origin, m_origin);
  }
  if (this->hasLocalControlFeature()) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::LocalControlFeature, m_localControlFeature);
  }
  if (this->hasUri()) {
    size_t valLength = encoder.prependByteArray(
                       reinterpret_cast<const uint8_t*>(m_uri.c_str()), m_uri.size());
    totalLength += valLength;
    totalLength += encoder.prependVarNumber(valLength);
    totalLength += encoder.prependVarNumber(tlv::nfd::Uri);
  }
  if (this->hasFaceId()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::FaceId, m_faceId);
  }
  if (this->hasName()) {
    totalLength += m_name.wireEncode(encoder);
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::ControlParameters);
  return totalLength;
}

template size_t
ControlParameters::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>&) const;

template size_t
ControlParameters::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>&) const;

Block
ControlParameters::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

void
ControlParameters::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::ControlParameters) {
    BOOST_THROW_EXCEPTION(Error("Expecting TLV-TYPE ControlParameters"));
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val;

  val = m_wire.find(tlv::Name);
  m_hasFields[CONTROL_PARAMETER_NAME] = val != m_wire.elements_end();
  if (this->hasName()) {
    m_name.wireDecode(*val);
  }

  val = m_wire.find(tlv::nfd::FaceId);
  m_hasFields[CONTROL_PARAMETER_FACE_ID] = val != m_wire.elements_end();
  if (this->hasFaceId()) {
    m_faceId = static_cast<uint64_t>(readNonNegativeInteger(*val));
  }

  val = m_wire.find(tlv::nfd::Uri);
  m_hasFields[CONTROL_PARAMETER_URI] = val != m_wire.elements_end();
  if (this->hasUri()) {
    m_uri.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
  }

  val = m_wire.find(tlv::nfd::LocalControlFeature);
  m_hasFields[CONTROL_PARAMETER_LOCAL_CONTROL_FEATURE] = val != m_wire.elements_end();
  if (this->hasLocalControlFeature()) {
    m_localControlFeature = static_cast<LocalControlFeature>(readNonNegativeInteger(*val));
  }

  val = m_wire.find(tlv::nfd::Origin);
  m_hasFields[CONTROL_PARAMETER_ORIGIN] = val != m_wire.elements_end();
  if (this->hasOrigin()) {
    m_origin = static_cast<uint64_t>(readNonNegativeInteger(*val));
  }

  val = m_wire.find(tlv::nfd::Cost);
  m_hasFields[CONTROL_PARAMETER_COST] = val != m_wire.elements_end();
  if (this->hasCost()) {
    m_cost = static_cast<uint64_t>(readNonNegativeInteger(*val));
  }

  val = m_wire.find(tlv::nfd::Flags);
  m_hasFields[CONTROL_PARAMETER_FLAGS] = val != m_wire.elements_end();
  if (this->hasFlags()) {
    m_flags = static_cast<uint64_t>(readNonNegativeInteger(*val));
  }

  val = m_wire.find(tlv::nfd::Strategy);
  m_hasFields[CONTROL_PARAMETER_STRATEGY] = val != m_wire.elements_end();
  if (this->hasStrategy()) {
    val->parse();
    if (val->elements().empty()) {
      BOOST_THROW_EXCEPTION(Error("Expecting Strategy/Name"));
    }
    else {
      m_strategy.wireDecode(*val->elements_begin());
    }
  }

  val = m_wire.find(tlv::nfd::ExpirationPeriod);
  m_hasFields[CONTROL_PARAMETER_EXPIRATION_PERIOD] = val != m_wire.elements_end();
  if (this->hasExpirationPeriod()) {
    m_expirationPeriod = time::milliseconds(readNonNegativeInteger(*val));
  }
}

std::ostream&
operator<<(std::ostream& os, const ControlParameters& parameters)
{
  os << "ControlParameters(";

  if (parameters.hasName()) {
    os << "Name: " << parameters.getName() << ", ";
  }

  if (parameters.hasFaceId()) {
    os << "FaceId: " << parameters.getFaceId() << ", ";
  }

  if (parameters.hasUri()) {
    os << "Uri: " << parameters.getUri() << ", ";
  }

  if (parameters.hasLocalControlFeature()) {
    os << "LocalControlFeature: " << parameters.getLocalControlFeature() << ", ";
  }

  if (parameters.hasOrigin()) {
    os << "Origin: " << parameters.getOrigin() << ", ";
  }

  if (parameters.hasCost()) {
    os << "Cost: " << parameters.getCost() << ", ";
  }

  if (parameters.hasFlags()) {
    os << "Flags: " << parameters.getFlags() << ", ";
  }

  if (parameters.hasStrategy()) {
    os << "Strategy: " << parameters.getStrategy() << ", ";
  }

  if (parameters.hasExpirationPeriod()) {
    os << "ExpirationPeriod: " << parameters.getExpirationPeriod() << ", ";
  }

  os << ")";
  return os;
}

} // namespace nfd
} // namespace ndn

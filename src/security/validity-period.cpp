/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "validity-period.hpp"
#include "../encoding/block-helpers.hpp"
#include "../util/concepts.hpp"

namespace ndn {
namespace security {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<ValidityPeriod>));
BOOST_CONCEPT_ASSERT((WireEncodable<ValidityPeriod>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<ValidityPeriod>));
BOOST_CONCEPT_ASSERT((WireDecodable<ValidityPeriod>));
static_assert(std::is_base_of<tlv::Error, ValidityPeriod::Error>::value,
              "ValidityPeriod::Error must inherit from tlv::Error");

static const size_t ISO_DATETIME_SIZE = 15;
static const size_t NOT_BEFORE_OFFSET = 0;
static const size_t NOT_AFTER_OFFSET = 1;

using boost::chrono::time_point_cast;

ValidityPeriod::ValidityPeriod()
  : ValidityPeriod(time::system_clock::TimePoint() + 1_ns,
                   time::system_clock::TimePoint())
{
}

ValidityPeriod::ValidityPeriod(const time::system_clock::TimePoint& notBefore,
                               const time::system_clock::TimePoint& notAfter)
  : m_notBefore(time_point_cast<TimePoint::duration>(notBefore + TimePoint::duration(1) -
                                                     time::system_clock::TimePoint::duration(1)))
  , m_notAfter(time_point_cast<TimePoint::duration>(notAfter))
{
}

ValidityPeriod::ValidityPeriod(const Block& block)
{
  wireDecode(block);
}

template<encoding::Tag TAG>
size_t
ValidityPeriod::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  totalLength += prependStringBlock(encoder, tlv::NotAfter, time::toIsoString(m_notAfter));
  totalLength += prependStringBlock(encoder, tlv::NotBefore, time::toIsoString(m_notBefore));

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::ValidityPeriod);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(ValidityPeriod);

const Block&
ValidityPeriod::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  m_wire.parse();

  return m_wire;
}

void
ValidityPeriod::wireDecode(const Block& wire)
{
  if (!wire.hasWire()) {
    BOOST_THROW_EXCEPTION(Error("The supplied block does not contain wire format"));
  }

  m_wire = wire;
  m_wire.parse();

  if (m_wire.type() != tlv::ValidityPeriod)
    BOOST_THROW_EXCEPTION(Error("Unexpected TLV type when decoding ValidityPeriod"));

  if (m_wire.elements_size() != 2)
    BOOST_THROW_EXCEPTION(Error("Does not have two sub-TLVs"));

  if (m_wire.elements()[NOT_BEFORE_OFFSET].type() != tlv::NotBefore ||
      m_wire.elements()[NOT_BEFORE_OFFSET].value_size() != ISO_DATETIME_SIZE ||
      m_wire.elements()[NOT_AFTER_OFFSET].type() != tlv::NotAfter ||
      m_wire.elements()[NOT_AFTER_OFFSET].value_size() != ISO_DATETIME_SIZE) {
    BOOST_THROW_EXCEPTION(Error("Invalid NotBefore or NotAfter field"));
  }

  try {
    m_notBefore = time_point_cast<TimePoint::duration>(
                    time::fromIsoString(readString(m_wire.elements()[NOT_BEFORE_OFFSET])));
    m_notAfter = time_point_cast<TimePoint::duration>(
                   time::fromIsoString(readString(m_wire.elements()[NOT_AFTER_OFFSET])));
  }
  catch (const std::bad_cast&) {
    BOOST_THROW_EXCEPTION(Error("Invalid date format in NOT-BEFORE or NOT-AFTER field"));
  }
}

ValidityPeriod&
ValidityPeriod::setPeriod(const time::system_clock::TimePoint& notBefore,
                          const time::system_clock::TimePoint& notAfter)
{
  m_wire.reset();
  m_notBefore = time_point_cast<TimePoint::duration>(notBefore + TimePoint::duration(1) -
                                                     time::system_clock::TimePoint::duration(1));
  m_notAfter = time_point_cast<TimePoint::duration>(notAfter);
  return *this;
}

std::pair<time::system_clock::TimePoint, time::system_clock::TimePoint>
ValidityPeriod::getPeriod() const
{
  return std::make_pair(m_notBefore, m_notAfter);
}

bool
ValidityPeriod::isValid(const time::system_clock::TimePoint& now) const
{
  return m_notBefore <= now && now <= m_notAfter;
}

bool
ValidityPeriod::operator==(const ValidityPeriod& other) const
{
  return (this->m_notBefore == other.m_notBefore &&
          this->m_notAfter == other.m_notAfter);
}

bool
ValidityPeriod::operator!=(const ValidityPeriod& other) const
{
  return !(*this == other);
}

std::ostream&
operator<<(std::ostream& os, const ValidityPeriod& period)
{
  os << "(" << time::toIsoString(period.getPeriod().first)
     << ", " << time::toIsoString(period.getPeriod().second) << ")";
  return os;
}

} // namespace security
} // namespace ndn

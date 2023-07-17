/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_VALIDITY_PERIOD_HPP
#define NDN_CXX_SECURITY_VALIDITY_PERIOD_HPP

#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/encoding/tlv.hpp"
#include "ndn-cxx/util/time.hpp"

namespace ndn::security {

/**
 * @brief Represents a %ValidityPeriod TLV element.
 * @sa https://docs.named-data.net/NDN-packet-spec/0.3/certificate.html
 */
class ValidityPeriod
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  /**
   * @brief Construct ValidityPeriod relative to a timepoint.
   * @param validFrom NotBefore is computed as `now+validFrom`.
   *                  This should be negative to construct a ValidityPeriod that includes @p now .
   * @param validUntil NotAfter is computed as `now+validTo`.
   *                   This should be positive to construct a ValidityPeriod that includes @p now .
   * @param now Reference timepoint. Default is current system clock timestamp.
   */
  static ValidityPeriod
  makeRelative(time::seconds validFrom, time::seconds validUntil,
               const time::system_clock::time_point& now = time::system_clock::now());

  /**
   * @brief Create a validity period that is invalid for any timepoint.
   */
  ValidityPeriod();

  /**
   * @brief Decode validity period from @p block .
   */
  explicit
  ValidityPeriod(const Block& block);

  /**
   * @brief Create validity period [@p notBefore, @p notAfter].
   * @param notBefore exclusive beginning of the validity period range,
   *                  to be rounded up to the next whole second.
   * @param notAfter exclusive end of the validity period range,
   *                  to be rounded down to the previous whole second.
   */
  ValidityPeriod(const time::system_clock::time_point& notBefore,
                 const time::system_clock::time_point& notAfter);

  /**
   * @brief Check if @p now falls within the validity period.
   * @param now Time point to check if it falls within the period
   * @return notBefore <= @p now and @p now <= notAfter.
   */
  bool
  isValid(const time::system_clock::time_point& now = time::system_clock::now()) const;

  /**
   * @brief Set validity period [@p notBefore, @p notAfter].
   * @param notBefore exclusive beginning of the validity period range,
   *                  to be rounded up to the next whole second.
   * @param notAfter exclusive end of the validity period range,
   *                  to be rounded down to the previous whole second.
   */
  ValidityPeriod&
  setPeriod(const time::system_clock::time_point& notBefore,
            const time::system_clock::time_point& notAfter);

  /**
   * @brief Get the stored validity period.
   */
  std::pair<time::system_clock::time_point, time::system_clock::time_point>
  getPeriod() const;

  /**
   * @brief Fast encoding or block size estimation.
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /**
   * @brief Encode ValidityPeriod into TLV block.
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode ValidityPeriod from TLV block.
   * @throw Error when an invalid TLV block supplied.
   *
   * @note If either timestamp in @p wire is earlier than 1677-09-21 or later than 2262-04-11,
   *       it will be adjusted to these dates so that they are representable as
   *       @c time::system_clock::time_point type returned by getPeriod() method.
   */
  void
  wireDecode(const Block& wire);

private:
  using TimePoint = boost::chrono::time_point<time::system_clock, time::seconds>;

  static TimePoint
  toTimePointFloor(const time::system_clock::time_point& t);

  static TimePoint
  toTimePointCeil(const time::system_clock::time_point& t);

  static TimePoint
  decodeTimePoint(const Block& element);

private: // EqualityComparable concept
  // NOTE: the following "hidden friend" operators are available via
  //       argument-dependent lookup only and must be defined inline.

  friend bool
  operator==(const ValidityPeriod& lhs, const ValidityPeriod& rhs)
  {
    return !(lhs != rhs);
  }

  friend bool
  operator!=(const ValidityPeriod& lhs, const ValidityPeriod& rhs)
  {
    return lhs.m_notBefore != rhs.m_notBefore ||
           lhs.m_notAfter != rhs.m_notAfter;
  }

private:
  TimePoint m_notBefore;
  TimePoint m_notAfter;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(ValidityPeriod);

std::ostream&
operator<<(std::ostream& os, const ValidityPeriod& period);

} // namespace ndn::security

#endif // NDN_CXX_SECURITY_VALIDITY_PERIOD_HPP

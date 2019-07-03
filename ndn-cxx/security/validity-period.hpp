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

#ifndef NDN_SECURITY_VALIDITY_PERIOD_HPP
#define NDN_SECURITY_VALIDITY_PERIOD_HPP

#include "ndn-cxx/detail/common.hpp"
#include "ndn-cxx/encoding/tlv.hpp"
#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/util/time.hpp"

namespace ndn {
namespace security {


/** @brief Abstraction of validity period
 *  @sa docs/specs/certificate-format.rst
 */
class ValidityPeriod
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

public:
  /** @brief Set validity period [UNIX epoch + 1 nanosecond, UNIX epoch] that is always invalid
   */
  ValidityPeriod();

  /** @brief Create validity period from @p block
   */
  explicit
  ValidityPeriod(const Block& block);

  /** @brief Create validity period [@p notBefore, @p notAfter]
   *  @param notBefore exclusive beginning of the validity period range
   *  @param notAfter exclusive end of the validity period range
   *
   *  @note The supplied time points will be rounded up to the whole seconds:
   *        - @p notBefore is rounded up the next whole second
   *        - @p notAfter is truncated to the previous whole second
   */
  ValidityPeriod(const time::system_clock::TimePoint& notBefore,
                 const time::system_clock::TimePoint& notAfter);

  /** @brief Check if @p now falls within the validity period
   *  @param now Time point to check if it falls within the period
   *  @return periodBegin <= @p now and @p now <= periodEnd
   */
  bool
  isValid(const time::system_clock::TimePoint& now = time::system_clock::now()) const;

  /** @brief Set validity period [@p notBefore, @p notAfter]
   *  @param notBefore exclusive beginning of the validity period range
   *  @param notAfter exclusive end of the validity period range
   *
   *  @note The supplied time points will be rounded up to the whole seconds:
   *        - @p notBefore is rounded up the next whole second
   *        - @p notAfter is truncated to the previous whole second
   */
  ValidityPeriod&
  setPeriod(const time::system_clock::TimePoint& notBefore,
            const time::system_clock::TimePoint& notAfter);

  /** @brief Get the stored validity period
   */
  std::pair<time::system_clock::TimePoint, time::system_clock::TimePoint>
  getPeriod() const;

  /** @brief Fast encoding or block size estimation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** @brief Encode ValidityPeriod into TLV block
   */
  const Block&
  wireEncode() const;

  /** @brief Decode ValidityPeriod from TLV block
   *  @throw Error when an invalid TLV block supplied
   */
  void
  wireDecode(const Block& wire);

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
  typedef boost::chrono::time_point<time::system_clock, time::seconds> TimePoint;

  TimePoint m_notBefore;
  TimePoint m_notAfter;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(ValidityPeriod);

std::ostream&
operator<<(std::ostream& os, const ValidityPeriod& period);

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_VALIDITY_PERIOD_HPP

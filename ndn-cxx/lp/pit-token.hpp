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

#ifndef NDN_CXX_LP_PIT_TOKEN_HPP
#define NDN_CXX_LP_PIT_TOKEN_HPP

#include "ndn-cxx/encoding/buffer.hpp"
#include "ndn-cxx/tag.hpp"

namespace ndn {
namespace lp {

/** \brief represent a PIT token field
 *  \sa https://redmine.named-data.net/projects/nfd/wiki/NDNLPv2#PIT-Token
 */
class PitToken : public Buffer, public Tag
{
public:
  static constexpr int
  getTypeId() noexcept
  {
    return 98;
  }

  /** \brief Construct from header field.
   *  \throw ndn::tlv::Error element length is out of range.
   */
  explicit
  PitToken(const std::pair<Buffer::const_iterator, Buffer::const_iterator>& value)
    : Buffer(value.first, value.second)
  {
    validate();
  }

  /** \brief Convert to header field.
   *  \throw ndn::tlv::Error element length is out of range.
   */
  operator std::pair<Buffer::const_iterator, Buffer::const_iterator>() const
  {
    validate();
    return std::make_pair(begin(), end());
  }

private:
  void
  validate() const;
};

std::ostream&
operator<<(std::ostream& os, const PitToken& pitToken);

} // namespace lp
} // namespace ndn

#endif // NDN_CXX_LP_PIT_TOKEN_HPP

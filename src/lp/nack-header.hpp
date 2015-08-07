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
 *
 * @author Eric Newberry <enewberry@email.arizona.edu>
 */

#ifndef NDN_CXX_LP_NACK_HEADER_HPP
#define NDN_CXX_LP_NACK_HEADER_HPP

#include "../common.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/block-helpers.hpp"

#include "tlv.hpp"

namespace ndn {
namespace lp {

/**
 * \brief indicates the reason type of a network NACK
 */
enum class NackReason {
  NONE = 0,
  CONGESTION = 50,
  DUPLICATE = 100,
  NO_ROUTE = 150
};

std::ostream&
operator<<(std::ostream& os, NackReason reason);

/**
 * \brief represents a Network NACK header
 */
class NackHeader
{
public:
  NackHeader();

  explicit
  NackHeader(const Block& block);

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

public: // reason
  /**
   * \return reason code
   * \retval NackReason::NONE if NackReason element does not exist or has an unknown code
   */
  NackReason
  getReason() const;

  /**
   * \brief set reason code
   * \param reason a reason code; NackReason::NONE clears the reason
   */
  NackHeader&
  setReason(NackReason reason);

private:
  NackReason m_reason;
  mutable Block m_wire;
};

} // namespace lp
} // namespace ndn

#endif // NDN_CXX_LP_NACK_HEADER_HPP
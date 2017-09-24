/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_CXX_LP_NACK_HPP
#define NDN_CXX_LP_NACK_HPP

#include "../common.hpp"
#include "../interest.hpp"
#include "../packet-base.hpp"

#include "nack-header.hpp"

namespace ndn {
namespace lp {

/** \brief represents a Network Nack
 *
 *  This type binds a NackHeader and an Interest, and is intended for use in network layer.
 */
class Nack : public PacketBase
{
public:
  Nack();

  explicit
  Nack(const Interest& interest);

  explicit
  Nack(Interest&& interest);

public: // getter/setter
  const Interest&
  getInterest() const
  {
    return m_interest;
  }

  Interest&
  getInterest()
  {
    return m_interest;
  }

  const NackHeader&
  getHeader() const
  {
    return m_header;
  }

  NackHeader&
  getHeader()
  {
    return m_header;
  }

  Nack&
  setHeader(const NackHeader& header)
  {
    m_header = header;
    return *this;
  }

  Nack&
  setHeader(NackHeader&& header)
  {
    m_header = header;
    return *this;
  }

public: // NackHeader proxy
  NackReason
  getReason() const
  {
    return m_header.getReason();
  }

  Nack&
  setReason(NackReason reason)
  {
    m_header.setReason(reason);
    return *this;
  }

private:
  Interest m_interest;
  NackHeader m_header;
};

} // namespace lp
} // namespace ndn

#endif // NDN_CXX_LP_NACK_HPP

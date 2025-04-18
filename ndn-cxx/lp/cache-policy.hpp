/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2025 Regents of the University of California.
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

#ifndef NDN_CXX_LP_CACHE_POLICY_HPP
#define NDN_CXX_LP_CACHE_POLICY_HPP

#include "ndn-cxx/encoding/encoding-buffer.hpp"
#include "ndn-cxx/lp/tlv.hpp"

namespace ndn::lp {

/**
 * \brief Indicates the cache policy applied to a Data packet.
 */
enum class CachePolicyType {
  NONE = 0,
  NO_CACHE = 1,
};

std::ostream&
operator<<(std::ostream& os, CachePolicyType policy);

/**
 * \brief Represents a %CachePolicy header field.
 */
class CachePolicy
{
public:
  class Error : public ndn::tlv::Error
  {
  public:
    using ndn::tlv::Error::Error;
  };

  CachePolicy() = default;

  explicit
  CachePolicy(const Block& block);

  /**
   * \brief Prepend CachePolicy to encoder.
   * \pre getPolicy() != CachePolicyType::NONE
   * \throw Error policy type is unset
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /**
   * \brief Encode CachePolicy into wire format.
   */
  const Block&
  wireEncode() const;

  /**
   * \brief Get CachePolicyType from wire format.
   */
  void
  wireDecode(const Block& wire);

public: // policy type
  /**
   * \brief Get policy type code.
   * \retval CachePolicyType::NONE if policy type is unset or has an unknown code.
   */
  CachePolicyType
  getPolicy() const;

  /**
   * \brief Set policy type code.
   * \param policy a policy type code; CachePolicyType::NONE clears the policy
   */
  CachePolicy&
  setPolicy(CachePolicyType policy);

private:
  CachePolicyType m_policy = CachePolicyType::NONE;
  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(CachePolicy);

} // namespace ndn::lp

#endif // NDN_CXX_LP_CACHE_POLICY_HPP

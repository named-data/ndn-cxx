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
 */

#ifndef NDN_ENCODING_ENCODING_BUFFER_HPP
#define NDN_ENCODING_ENCODING_BUFFER_HPP

#include "encoding-buffer-fwd.hpp"
#include "encoder.hpp"
#include "estimator.hpp"

namespace ndn {
namespace encoding {

/**
 * @brief EncodingImpl specialization for actual TLV encoding
 */
template<>
class EncodingImpl<EncoderTag> : public Encoder
{
public:
  explicit
  EncodingImpl(size_t totalReserve = MAX_NDN_PACKET_SIZE, size_t reserveFromBack = 400)
    : Encoder(totalReserve, reserveFromBack)
  {
  }

  explicit
  EncodingImpl(const Block& block)
    : Encoder(block)
  {
  }
};

/**
 * @brief EncodingImpl specialization for TLV size estimation
 */
template<>
class EncodingImpl<EstimatorTag> : public Estimator
{
public:
  explicit
  EncodingImpl(size_t totalReserve = 0, size_t totalFromBack = 0)
    : Estimator(totalReserve, totalFromBack)
  {
  }
};

} // namespace encoding
} // namespace ndn

#endif // NDN_ENCODING_ENCODING_BUFFER_HPP

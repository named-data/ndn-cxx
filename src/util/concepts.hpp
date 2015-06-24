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

#ifndef NDN_UTIL_CONCEPTS_HPP
#define NDN_UTIL_CONCEPTS_HPP

#include <boost/concept/usage.hpp>
#include "../encoding/block.hpp"
#include "../encoding/encoding-buffer.hpp"

namespace ndn {

/** \brief a concept check for TLV abstraction with .wireEncode method
 */
template<class X>
class WireEncodable
{
public:
  BOOST_CONCEPT_USAGE(WireEncodable)
  {
    Block block = j.wireEncode();
    block.size(); // avoid 'unused variable block'
  }

private:
  X j;
};

/** \brief a concept check for TLV abstraction with .wireEncode method
 */
template<class X>
class WireEncodableWithEncodingBuffer
{
public:
  BOOST_CONCEPT_USAGE(WireEncodableWithEncodingBuffer)
  {
    EncodingEstimator estimator;
    size_t estimatedSize = j.wireEncode(estimator);

    EncodingBuffer encoder(estimatedSize, 0);
    j.wireEncode(encoder);
  }

private:
  X j;
};

/** \brief a concept check for TLV abstraction with .wireDecode method
 *         and constructible from Block
 */
template<class X>
class WireDecodable
{
public:
  BOOST_CONCEPT_USAGE(WireDecodable)
  {
    Block block;
    X j(block);
    j.wireDecode(block);
  }
};

/** \brief a concept check for CryptoPP hash algorithm
 */
template<class X>
class Hashable
{
public:
  BOOST_CONCEPT_USAGE(Hashable)
  {
    X hash;
    uint8_t* buf = 0;
    size_t size = hash.DigestSize();

    hash.Update(buf, size);
    hash.Final(buf);
    hash.Restart();
  }
};

} // namespace ndn

#endif // NDN_UTIL_CONCEPTS_HPP

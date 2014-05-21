/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "common.hpp"

#include "random.hpp"

#include "../security/cryptopp.hpp"

namespace ndn {
namespace random {

uint32_t
generateWord32()
{
  static CryptoPP::AutoSeededRandomPool rng;

  return rng.GenerateWord32();
}

uint64_t
generateWord64()
{
  static CryptoPP::AutoSeededRandomPool rng;

  uint64_t random;

  rng.GenerateBlock(reinterpret_cast<unsigned char*>(&random), 8);

  return random;
}

} // namespace random
} // namespace ndn

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

#include "random.hpp"
#include "../security/detail/openssl.hpp"

#include <random>

namespace ndn {
namespace random {

uint32_t
generateSecureWord32()
{
  uint32_t random;
  generateSecureBytes(reinterpret_cast<uint8_t*>(&random), sizeof(random));
  return random;
}

uint64_t
generateSecureWord64()
{
  uint64_t random;
  generateSecureBytes(reinterpret_cast<uint8_t*>(&random), sizeof(random));
  return random;
}

void
generateSecureBytes(uint8_t* bytes, size_t size)
{
  if (RAND_bytes(bytes, size) != 1) {
    BOOST_THROW_EXCEPTION(std::runtime_error("Failed to generate random bytes (error code " +
                                             to_string(ERR_get_error()) + ")"));
  }
}

static std::mt19937&
getRandomGenerator()
{
  static std::mt19937 rng{std::random_device{}()};
  return rng;
}

uint32_t
generateWord32()
{
  static std::uniform_int_distribution<uint32_t> distribution;
  return distribution(getRandomGenerator());
}

uint64_t
generateWord64()
{
  static std::uniform_int_distribution<uint64_t> distribution;
  return distribution(getRandomGenerator());
}

} // namespace random
} // namespace ndn

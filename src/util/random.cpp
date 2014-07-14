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

#include <boost/nondet_random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "../security/cryptopp.hpp"

namespace ndn {
namespace random {

// CryptoPP-based (secure) random generators

static CryptoPP::AutoSeededRandomPool&
getSecureRandomGenerator()
{
  static CryptoPP::AutoSeededRandomPool rng;

  return rng;
}

uint32_t
generateSecureWord32()
{
  return getSecureRandomGenerator().GenerateWord32();
}

uint64_t
generateSecureWord64()
{
  uint64_t random;
  getSecureRandomGenerator()
    .GenerateBlock(reinterpret_cast<unsigned char*>(&random), sizeof(uint64_t));

  return random;
}

// Boost.Random-based (simple) random generators

static boost::random::mt19937&
getRandomGenerator()
{
  static boost::random_device randomSeedGenerator;
  static boost::random::mt19937 gen(randomSeedGenerator);

  return gen;
}

uint32_t
generateWord32()
{
  static boost::random::uniform_int_distribution<uint32_t> distribution;
  return distribution(getRandomGenerator());
}

uint64_t
generateWord64()
{
  static boost::random::uniform_int_distribution<uint64_t> distribution;
  return distribution(getRandomGenerator());
}


} // namespace random
} // namespace ndn

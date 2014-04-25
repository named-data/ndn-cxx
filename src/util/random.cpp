/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
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

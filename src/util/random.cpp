/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
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

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

#include "key-params.hpp"

namespace ndn {

static const uint32_t MIN_RSA_KEY_SIZE = 1024;
static const uint32_t DEFAULT_RSA_KEY_SIZE = 2048;
static const uint32_t EC_KEY_SIZES[] = {256, 384};
static const uint32_t AES_KEY_SIZES[] = {128, 192, 256};

KeyParams::KeyParams(KeyType keyType, KeyIdType keyIdType)
  : m_keyType(keyType)
  , m_keyIdType(keyIdType)
{
  BOOST_ASSERT(keyIdType != KeyIdType::USER_SPECIFIED);
}

KeyParams::KeyParams(KeyType keyType, const name::Component& keyId)
  : m_keyType(keyType)
  , m_keyIdType(KeyIdType::USER_SPECIFIED)
  , m_keyId(keyId)
{
  BOOST_ASSERT(!keyId.empty());
}

KeyParams::~KeyParams() = default;

namespace detail {

uint32_t
RsaKeyParamsInfo::checkKeySize(uint32_t size)
{
  if (size < MIN_RSA_KEY_SIZE)
    BOOST_THROW_EXCEPTION(KeyParams::Error("Unsupported RSA key size"));
  return size;
}

uint32_t
RsaKeyParamsInfo::getDefaultSize()
{
  return DEFAULT_RSA_KEY_SIZE;
}

uint32_t
EcKeyParamsInfo::checkKeySize(uint32_t size)
{
  for (size_t i = 0; i < (sizeof(EC_KEY_SIZES) / sizeof(EC_KEY_SIZES[0])); i++) {
    if (EC_KEY_SIZES[i] == size)
      return size;
  }
  BOOST_THROW_EXCEPTION(KeyParams::Error("Unsupported EC key size"));
}

uint32_t
EcKeyParamsInfo::getDefaultSize()
{
  return EC_KEY_SIZES[0];
}

uint32_t
AesKeyParamsInfo::checkKeySize(uint32_t size)
{
  for (size_t i = 0; i < (sizeof(AES_KEY_SIZES) / sizeof(AES_KEY_SIZES[0])); i++) {
    if (AES_KEY_SIZES[i] == size)
      return size;
  }
  BOOST_THROW_EXCEPTION(KeyParams::Error("Unsupported AES key size"));
}

uint32_t
AesKeyParamsInfo::getDefaultSize()
{
  return AES_KEY_SIZES[0];
}

} // namespace detail
} // namespace ndn

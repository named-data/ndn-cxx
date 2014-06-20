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

#include "key-params.hpp"

namespace ndn {

static const uint32_t RSA_KEY_SIZES[] = {2048, 1024};
static const uint32_t ECDSA_KEY_SIZES[] = {256, 384};
static const uint32_t AES_KEY_SIZES[] = {64, 128, 256};

uint32_t
RsaKeyParamsInfo::checkKeySize(uint32_t size)
{
  for (size_t i = 0; i < (sizeof(RSA_KEY_SIZES) / sizeof(uint32_t)); i++)
    {
      if (RSA_KEY_SIZES[i] == size)
        return size;
    }
  return getDefaultSize();
}

uint32_t
RsaKeyParamsInfo::getDefaultSize()
{
  return RSA_KEY_SIZES[0];
}

uint32_t
EcdsaKeyParamsInfo::checkKeySize(uint32_t size)
{

  for (size_t i = 0; i < (sizeof(ECDSA_KEY_SIZES) / sizeof(uint32_t)); i++)
    {
      if (ECDSA_KEY_SIZES[i] == size)
        return size;
    }
  return getDefaultSize();
}

uint32_t
EcdsaKeyParamsInfo::getDefaultSize()
{
  return ECDSA_KEY_SIZES[0];
}


uint32_t
AesKeyParamsInfo::checkKeySize(uint32_t size)
{
  for (size_t i = 0; i < (sizeof(AES_KEY_SIZES) / sizeof(uint32_t)); i++)
    {
      if (AES_KEY_SIZES[i] == size)
        return size;
    }
  return getDefaultSize();
}

uint32_t
AesKeyParamsInfo::getDefaultSize()
{
  return AES_KEY_SIZES[0];
}

} // namespace ndn

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/security/security-common.hpp"

#include <ostream>

namespace ndn {

std::ostream&
operator<<(std::ostream& os, KeyIdType keyIdType)
{
  switch (keyIdType) {
    case KeyIdType::USER_SPECIFIED:
      return os << "USER-SPECIFIED";
    case KeyIdType::SHA256:
      return os << "SHA256";
    case KeyIdType::RANDOM:
      return os << "RANDOM";
  }
  return os << to_underlying(keyIdType);
}

std::ostream&
operator<<(std::ostream& os, KeyType keyType)
{
  switch (keyType) {
    case KeyType::NONE:
      return os << "NONE";
    case KeyType::RSA:
      return os << "RSA";
    case KeyType::EC:
      return os << "EC";
    case KeyType::AES:
      return os << "AES";
    case KeyType::HMAC:
      return os << "HMAC";
  }
  return os << to_underlying(keyType);
}

std::ostream&
operator<<(std::ostream& os, DigestAlgorithm algorithm)
{
  switch (algorithm) {
    case DigestAlgorithm::NONE:
      return os << "NONE";
    case DigestAlgorithm::SHA224:
      return os << "SHA224";
    case DigestAlgorithm::SHA256:
      return os << "SHA256";
    case DigestAlgorithm::SHA384:
      return os << "SHA384";
    case DigestAlgorithm::SHA512:
      return os << "SHA512";
    case DigestAlgorithm::BLAKE2B_512:
      return os << "BLAKE2b-512";
    case DigestAlgorithm::BLAKE2S_256:
      return os << "BLAKE2s-256";
    case DigestAlgorithm::SHA3_224:
      return os << "SHA3-224";
    case DigestAlgorithm::SHA3_256:
      return os << "SHA3-256";
    case DigestAlgorithm::SHA3_384:
      return os << "SHA3-384";
    case DigestAlgorithm::SHA3_512:
      return os << "SHA3-512";
  }
  return os << to_underlying(algorithm);
}

std::ostream&
operator<<(std::ostream& os, BlockCipherAlgorithm algorithm)
{
  switch (algorithm) {
    case BlockCipherAlgorithm::NONE:
      return os << "NONE";
    case BlockCipherAlgorithm::AES_CBC:
      return os << "AES-CBC";
  }
  return os << to_underlying(algorithm);
}

std::ostream&
operator<<(std::ostream& os, CipherOperator op)
{
  switch (op) {
    case CipherOperator::DECRYPT:
      return os << "DECRYPT";
    case CipherOperator::ENCRYPT:
      return os << "ENCRYPT";
  }
  return os << to_underlying(op);
}

} // namespace ndn

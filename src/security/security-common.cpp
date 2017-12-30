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

#include "security-common.hpp"

#include <ostream>

namespace ndn {

std::ostream&
operator<<(std::ostream& os, KeyIdType keyIdType)
{
  switch (keyIdType) {
    case KeyIdType::USER_SPECIFIED:
      return os << "USER_SPECIFIED";
    case KeyIdType::SHA256:
      return os << "SHA256";
    case KeyIdType::RANDOM:
      return os << "RANDOM";
  }
  return os << static_cast<int>(keyIdType);
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
  }
  return os << static_cast<int>(keyType);
}

std::ostream&
operator<<(std::ostream& os, KeyClass keyClass)
{
  switch (keyClass) {
    case KeyClass::NONE:
      return os << "NONE";
    case KeyClass::PUBLIC:
      return os << "PUBLIC";
    case KeyClass::PRIVATE:
      return os << "PRIVATE";
    case KeyClass::SYMMETRIC:
      return os << "SYMMETRIC";
  }
  return os << static_cast<int>(keyClass);
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
  }
  return os << static_cast<int>(algorithm);
}

std::ostream&
operator<<(std::ostream& os, BlockCipherAlgorithm algorithm)
{
  switch (algorithm) {
    case BlockCipherAlgorithm::NONE:
      return os << "NONE";
    case BlockCipherAlgorithm::AES_CBC:
      return os << "AES_CBC";
  }
  return os << static_cast<int>(algorithm);
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
  return os << static_cast<int>(op);
}

std::ostream&
operator<<(std::ostream& os, AclType aclType)
{
  switch (aclType) {
    case AclType::NONE:
      return os << "NONE";
    case AclType::PUBLIC:
      return os << "PUBLIC";
    case AclType::PRIVATE:
      return os << "PRIVATE";
  }
  return os << static_cast<int>(aclType);
}

} // namespace ndn

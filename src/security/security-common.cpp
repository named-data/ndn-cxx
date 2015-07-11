/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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
operator<<(std::ostream& os, KeyType keyType)
{
  switch (keyType) {
  case KeyType::NONE:
    os << "NONE";
    break;
  case KeyType::RSA:
    os << "RSA";
    break;
  case KeyType::EC:
    os << "EC";
    break;
  case KeyType::AES:
    os << "AES";
    break;
  default:
    os << static_cast<int>(keyType);
    break;
  };
  return os;
}

std::ostream&
operator<<(std::ostream& os, KeyClass keyClass)
{
  switch (keyClass) {
  case KeyClass::NONE:
    os << "NONE";
    break;
  case KeyClass::PUBLIC:
    os << "PUBLIC";
    break;
  case KeyClass::PRIVATE:
    os << "PRIVATE";
    break;
  case KeyClass::SYMMETRIC:
    os << "SYMMETRIC";
    break;
  default:
    os << static_cast<int>(keyClass);
    break;
  };
  return os;
}

std::ostream&
operator<<(std::ostream& os, DigestAlgorithm algorithm)
{
  switch (algorithm) {
  case DigestAlgorithm::NONE:
    os << "NONE";
    break;
  case DigestAlgorithm::SHA256:
    os << "SHA256";
    break;
  default:
    os << static_cast<int>(algorithm);
    break;
  };
  return os;
}

std::ostream&
operator<<(std::ostream& os, BlockCipherAlgorithm algorithm)
{
  switch (algorithm) {
  case BlockCipherAlgorithm::NONE:
    os << "NONE";
    break;
  case BlockCipherAlgorithm::AES_CBC:
    os << "AES_CBC";
    break;
  default:
    os << static_cast<int>(algorithm);
    break;
  };
  return os;
}

std::ostream&
operator<<(std::ostream& os, CipherOperator op)
{
  switch (op) {
  case CipherOperator::DECRYPT:
    os << "DECRYPT";
    break;
  case CipherOperator::ENCRYPT:
    os << "ENCRYPT";
    break;
  default:
    os << static_cast<int>(op);
    break;
  };
  return os;
}

std::ostream&
operator<<(std::ostream& os, AclType aclType)
{
  switch (aclType) {
  case AclType::NONE:
    os << "NONE";
    break;
  case AclType::PUBLIC:
    os << "PUBLIC";
    break;
  case AclType::PRIVATE:
    os << "PRIVATE";
    break;
  default:
    os << static_cast<int>(aclType);
    break;
  };
  return os;
}

} // namespace ndn

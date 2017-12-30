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

#ifndef NDN_SECURITY_SECURITY_COMMON_HPP
#define NDN_SECURITY_SECURITY_COMMON_HPP

#include "../common.hpp"

namespace ndn {

namespace signed_interest {

const ssize_t POS_SIG_VALUE = -1;
const ssize_t POS_SIG_INFO = -2;

/** \brief minimal number of components for Signed Interest
 *  \sa https://redmine.named-data.net/projects/ndn-cxx/wiki/SignedInterest
 */
const size_t MIN_SIZE = 2;

} // namespace signed_interest

namespace command_interest {

using signed_interest::POS_SIG_VALUE;
using signed_interest::POS_SIG_INFO;
const ssize_t POS_RANDOM_VAL = -3;
const ssize_t POS_TIMESTAMP = -4;

/** \brief minimal number of components for Command Interest
 *  \sa https://redmine.named-data.net/projects/ndn-cxx/wiki/CommandInterest
 */
const size_t MIN_SIZE = 4;

} // namespace command_interest

/**
 * @brief The type of KeyId component in a key name
 */
enum class KeyIdType {
  /**
   * @brief User-specified key ID
   *
   * It is user's responsibility to assure the uniqueness of the key names.
   */
  USER_SPECIFIED = 0,
  /**
   * @brief Use the SHA256 hash of the public key as the key id
   *
   * This KeyId type guarantees the uniqueness of the key names.
   */
  SHA256 = 1,
  /**
   * @brief Use a 64-bit random number as the key id
   *
   * This KeyId provides roughly uniqueness of the key names.
   */
  RANDOM = 2,
};

std::ostream&
operator<<(std::ostream& os, KeyIdType keyIdType);

/**
 * @brief The type of a cryptographic key
 */
enum class KeyType {
  NONE = 0,   ///< Unknown key type
  RSA  = 1,   ///< RSA key, supports sign/verify and encrypt/decrypt operations
  EC   = 2,   ///< Elliptic Curve key (e.g. for ECDSA), supports sign/verify operations
  AES  = 128, ///< AES key, supports encrypt/decrypt operations
};

std::ostream&
operator<<(std::ostream& os, KeyType keyType);

enum class KeyClass {
  NONE,
  PUBLIC,
  PRIVATE,
  SYMMETRIC,
};

std::ostream&
operator<<(std::ostream& os, KeyClass keyClass);

enum class DigestAlgorithm {
  NONE   = 0,
  SHA224 = 2,
  SHA256 = 1,
  SHA384 = 3,
  SHA512 = 4,
  BLAKE2B_512 = 10,
  BLAKE2S_256 = 11,
};

std::ostream&
operator<<(std::ostream& os, DigestAlgorithm algorithm);

enum class BlockCipherAlgorithm {
  NONE,
  AES_CBC,
};

std::ostream&
operator<<(std::ostream& os, BlockCipherAlgorithm algorithm);

enum class CipherOperator {
  DECRYPT = 0,
  ENCRYPT = 1,
};

std::ostream&
operator<<(std::ostream& os, CipherOperator op);

enum class AclType {
  NONE,
  PUBLIC,
  PRIVATE,
};

std::ostream&
operator<<(std::ostream& os, AclType aclType);

namespace security {
namespace transform {
class PublicKey;
} // namespace transform
namespace v2 {
using transform::PublicKey;
} // namespace v2
} // namespace security

} // namespace ndn

#endif // NDN_SECURITY_SECURITY_COMMON_HPP

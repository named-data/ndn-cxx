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

#ifndef NDN_SECURITY_COMMON_HPP
#define NDN_SECURITY_COMMON_HPP

#include "../common.hpp"

#define NDN_CXX_KEEP_SECURITY_V1_ALIASES

namespace ndn {

namespace signed_interest {

const ssize_t POS_SIG_VALUE = -1;
const ssize_t POS_SIG_INFO = -2;
const ssize_t POS_RANDOM_VAL = -3;
const ssize_t POS_TIMESTAMP = -4;

/** \brief minimal number of components for Command Interest
 *  \sa https://redmine.named-data.net/projects/ndn-cxx/wiki/CommandInterest
 */
const size_t MIN_LENGTH = 4;

/** \brief minimal number of components for Signed Interest
 *  \sa https://redmine.named-data.net/projects/ndn-cxx/wiki/SignedInterest
 */
const size_t MIN_LENGTH_SIG_ONLY = 2;

} // namespace signed_interest

enum class KeyType {
  NONE = 0,
  RSA  = 1,
  EC   = 2,
  AES  = 128
};

std::ostream&
operator<<(std::ostream& os, KeyType keyType);

enum class KeyClass {
  NONE,
  PUBLIC,
  PRIVATE,
  SYMMETRIC
};

std::ostream&
operator<<(std::ostream& os, KeyClass keyClass);

enum class DigestAlgorithm {
  NONE   = 0,
  SHA256 = 1
};

std::ostream&
operator<<(std::ostream& os, DigestAlgorithm algorithm);

enum class BlockCipherAlgorithm {
  NONE,
  AES_CBC
};

std::ostream&
operator<<(std::ostream& os, BlockCipherAlgorithm algorithm);

enum class CipherOperator {
  DECRYPT = 0,
  ENCRYPT = 1
};

std::ostream&
operator<<(std::ostream& os, CipherOperator op);

enum class AclType {
  NONE,
  PUBLIC,
  PRIVATE
};

std::ostream&
operator<<(std::ostream& os, AclType aclType);

} // namespace ndn

#endif // NDN_SECURITY_COMMON_HPP

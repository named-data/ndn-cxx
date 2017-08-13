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

#ifndef NDN_SECURITY_DIGEST_SHA256_HPP
#define NDN_SECURITY_DIGEST_SHA256_HPP

#include "../signature.hpp"

namespace ndn {

/** @brief Represents a signature of DigestSha256 type
 *
 *  This signature type provides integrity protection using SHA-256 digest, but no provenance of a
 *  Data packet or any kind of guarantee that packet is from the original source.
 */
class DigestSha256 : public Signature
{
public:
  /** @brief Create empty DigestSha256 signature
   */
  DigestSha256();

  /** @brief Convert base Signature to DigestSha256 signature
   *  @throw Signature::Error SignatureType is not DigestSha256
   */
  explicit
  DigestSha256(const Signature& signature);
};

} // namespace ndn

#endif // NDN_SECURITY_DIGEST_SHA256_HPP

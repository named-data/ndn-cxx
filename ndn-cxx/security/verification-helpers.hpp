/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_VERIFICATION_HELPERS_HPP
#define NDN_CXX_SECURITY_VERIFICATION_HELPERS_HPP

#include "ndn-cxx/name.hpp"
#include "ndn-cxx/security/security-common.hpp"

namespace ndn {

class Interest;
class Data;

namespace security {

namespace pib {
class Key;
} // namespace pib

namespace tpm {
class Tpm;
} // namespace tpm

namespace transform {
class PublicKey;
} // namespace transform

inline namespace v2 {
class Certificate;
} // inline namespace v2

/**
 * @brief Verify @p blobs using @p key against @p sig.
 */
NDN_CXX_NODISCARD bool
verifySignature(const InputBuffers& blobs, span<const uint8_t> sig, const transform::PublicKey& key);

/**
 * @brief Verify @p blobs using @p key against @p sig.
 * @note @p key must be a public key in PKCS #8 format.
 */
NDN_CXX_NODISCARD bool
verifySignature(const InputBuffers& blobs, span<const uint8_t> sig, span<const uint8_t> key);

/**
 * @brief Verify @p data using @p key.
 * @note @p key must be a public key in PKCS #8 format.
 */
NDN_CXX_NODISCARD bool
verifySignature(const Data& data, span<const uint8_t> key);

/**
 * @brief Verify @p interest using @p key.
 * @note @p key must be a public key in PKCS #8 format.
 * @note This method verifies only signature of the signed interest.
 */
NDN_CXX_NODISCARD bool
verifySignature(const Interest& interest, span<const uint8_t> key);

/**
 * @brief Verify @p data using @p key.
 */
NDN_CXX_NODISCARD bool
verifySignature(const Data& data, const transform::PublicKey& key);

/**
 * @brief Verify @p interest using @p key.
 * @note This method verifies only signature of the signed interest.
 */
NDN_CXX_NODISCARD bool
verifySignature(const Interest& interest, const transform::PublicKey& key);

/**
 * @brief Verify @p data using @p key.
 */
NDN_CXX_NODISCARD bool
verifySignature(const Data& data, const pib::Key& key);

/**
 * @brief Verify @p interest using @p key.
 * @note This method verifies only signature of the signed interest.
 */
NDN_CXX_NODISCARD bool
verifySignature(const Interest& interest, const pib::Key& key);

/**
 * @brief Verify @p data using @p cert.
 *
 * If @p cert is nullopt, @p data assumed to be self-verifiable (with digest or attributes)
 */
NDN_CXX_NODISCARD bool
verifySignature(const Data& data, const optional<Certificate>& cert);

/**
 * @brief Verify @p interest using @p cert.
 * @note This method verifies only signature of the signed interest.
 *
 * If @p cert is nullptr, @p interest assumed to be self-verifiable (with digest or attributes)
 */
NDN_CXX_NODISCARD bool
verifySignature(const Interest& interest, const optional<Certificate>& cert);

/**
 * @brief Verify @p data using @p tpm and @p keyName with the @p digestAlgorithm.
 */
NDN_CXX_NODISCARD bool
verifySignature(const Data& data, const tpm::Tpm& tpm, const Name& keyName,
                DigestAlgorithm digestAlgorithm);

/**
 * @brief Verify @p interest using @p tpm and @p keyName with the @p digestAlgorithm.
 * @note This method verifies only signature of the signed interest.
 */
NDN_CXX_NODISCARD bool
verifySignature(const Interest& interest, const tpm::Tpm& tpm, const Name& keyName,
                DigestAlgorithm digestAlgorithm);

} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_VERIFICATION_HELPERS_HPP

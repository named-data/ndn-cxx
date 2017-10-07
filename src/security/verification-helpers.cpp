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

#include "verification-helpers.hpp"

#include "detail/openssl.hpp"
#include "pib/key.hpp"
#include "transform/bool-sink.hpp"
#include "transform/buffer-source.hpp"
#include "transform/digest-filter.hpp"
#include "transform/public-key.hpp"
#include "transform/stream-sink.hpp"
#include "transform/verifier-filter.hpp"
#include "v2/certificate.hpp"
#include "../data.hpp"
#include "../interest.hpp"
#include "../encoding/buffer-stream.hpp"

namespace ndn {
namespace security {

bool
verifySignature(const uint8_t* blob, size_t blobLen, const uint8_t* sig, size_t sigLen,
                const v2::PublicKey& pKey)
{
  bool result = false;
  try {
    using namespace transform;
    bufferSource(blob, blobLen) >> verifierFilter(DigestAlgorithm::SHA256, pKey, sig, sigLen)
                                >> boolSink(result);
  }
  catch (const transform::Error&) {
    return false;
  }
  return result;
}

bool
verifySignature(const uint8_t* data, size_t dataLen, const uint8_t* sig, size_t sigLen,
                const uint8_t* key, size_t keyLen)
{
  v2::PublicKey pKey;
  try {
    pKey.loadPkcs8(key, keyLen);
  }
  catch (const transform::Error&) {
    return false;
  }

  return verifySignature(data, dataLen, sig, sigLen, pKey);
}

static std::tuple<bool, const uint8_t*, size_t, const uint8_t*, size_t>
parse(const Data& data)
{
  try {
    return std::make_tuple(true,
                           data.wireEncode().value(),
                           data.wireEncode().value_size() - data.getSignature().getValue().size(),
                           data.getSignature().getValue().value(),
                           data.getSignature().getValue().value_size());
  }
  catch (const tlv::Error&) {
    return std::make_tuple(false, nullptr, 0, nullptr, 0);
  }
}

static std::tuple<bool, const uint8_t*, size_t, const uint8_t*, size_t>
parse(const Interest& interest)
{
  const Name& interestName = interest.getName();

  if (interestName.size() < signed_interest::MIN_SIZE)
    return std::make_tuple(false, nullptr, 0, nullptr, 0);

  try {
    const Block& nameBlock = interestName.wireEncode();

    return std::make_tuple(true,
                           nameBlock.value(), nameBlock.value_size() - interestName[signed_interest::POS_SIG_VALUE].size(),
                           interestName[signed_interest::POS_SIG_VALUE].blockFromValue().value(),
                           interestName[signed_interest::POS_SIG_VALUE].blockFromValue().value_size());
  }
  catch (const tlv::Error&) {
    return std::make_tuple(false, nullptr, 0, nullptr, 0);
  }
}

static bool
verifySignature(const std::tuple<bool, const uint8_t*, size_t, const uint8_t*, size_t>& params,
                const v2::PublicKey& pKey)
{
  bool isParsable = false;
  const uint8_t* buf = nullptr;
  size_t bufLen = 0;
  const uint8_t* sig = nullptr;
  size_t sigLen = 0;

  std::tie(isParsable, buf, bufLen, sig, sigLen) = params;

  if (isParsable)
    return verifySignature(buf, bufLen, sig, sigLen, pKey);
  else
    return false;
}

static bool
verifySignature(const std::tuple<bool, const uint8_t*, size_t, const uint8_t*, size_t>& params,
                const uint8_t* key, size_t keyLen)
{
  bool isParsable = false;
  const uint8_t* buf = nullptr;
  size_t bufLen = 0;
  const uint8_t* sig = nullptr;
  size_t sigLen = 0;

  std::tie(isParsable, buf, bufLen, sig, sigLen) = params;

  if (isParsable)
    return verifySignature(buf, bufLen, sig, sigLen, key, keyLen);
  else
    return false;
}

bool
verifySignature(const Data& data, const v2::PublicKey& key)
{
  return verifySignature(parse(data), key);
}

bool
verifySignature(const Interest& interest, const v2::PublicKey& key)
{
  return verifySignature(parse(interest), key);
}

bool
verifySignature(const Data& data, const pib::Key& key)
{
  return verifySignature(parse(data), key.getPublicKey().data(), key.getPublicKey().size());
}

bool
verifySignature(const Interest& interest, const pib::Key& key)
{
  return verifySignature(parse(interest), key.getPublicKey().data(), key.getPublicKey().size());
}

bool
verifySignature(const Data& data, const uint8_t* key, size_t keyLen)
{
  return verifySignature(parse(data), key, keyLen);
}

bool
verifySignature(const Interest& interest, const uint8_t* key, size_t keyLen)
{
  return verifySignature(parse(interest), key, keyLen);
}

bool
verifySignature(const Data& data, const v2::Certificate& cert)
{
  return verifySignature(parse(data), cert.getContent().value(), cert.getContent().value_size());
}

bool
verifySignature(const Interest& interest, const v2::Certificate& cert)
{
  return verifySignature(parse(interest), cert.getContent().value(), cert.getContent().value_size());
}

///////////////////////////////////////////////////////////////////////

bool
verifyDigest(const uint8_t* blob, size_t blobLen, const uint8_t* digest, size_t digestLen,
             DigestAlgorithm algorithm)
{
  using namespace transform;

  OBufferStream os;
  try {
    bufferSource(blob, blobLen) >> digestFilter(algorithm) >> streamSink(os);
  }
  catch (const transform::Error&) {
    return false;
  }
  ConstBufferPtr result = os.buf();

  if (result->size() != digestLen)
    return false;

  // constant-time buffer comparison to mitigate timing attacks
  return CRYPTO_memcmp(result->data(), digest, digestLen) == 0;
}

bool
verifyDigest(const Data& data, DigestAlgorithm algorithm)
{
  bool isParsable = false;
  const uint8_t* buf = nullptr;
  size_t bufLen = 0;
  const uint8_t* sig = nullptr;
  size_t sigLen = 0;

  std::tie(isParsable, buf, bufLen, sig, sigLen) = parse(data);

  if (isParsable) {
    return verifyDigest(buf, bufLen, sig, sigLen, algorithm);
  }
  else {
    return false;
  }
}

bool
verifyDigest(const Interest& interest, DigestAlgorithm algorithm)
{
  bool isParsable = false;
  const uint8_t* buf = nullptr;
  size_t bufLen = 0;
  const uint8_t* sig = nullptr;
  size_t sigLen = 0;

  std::tie(isParsable, buf, bufLen, sig, sigLen) = parse(interest);

  if (isParsable) {
    return verifyDigest(buf, bufLen, sig, sigLen, algorithm);
  }
  else {
    return false;
  }
}

} // namespace security
} // namespace ndn

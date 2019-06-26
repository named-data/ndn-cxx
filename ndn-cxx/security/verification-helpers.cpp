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

#include "ndn-cxx/security/verification-helpers.hpp"

#include "ndn-cxx/data.hpp"
#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/security/impl/openssl.hpp"
#include "ndn-cxx/security/pib/key.hpp"
#include "ndn-cxx/security/tpm/key-handle.hpp"
#include "ndn-cxx/security/tpm/tpm.hpp"
#include "ndn-cxx/security/transform/bool-sink.hpp"
#include "ndn-cxx/security/transform/buffer-source.hpp"
#include "ndn-cxx/security/transform/digest-filter.hpp"
#include "ndn-cxx/security/transform/public-key.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"
#include "ndn-cxx/security/transform/verifier-filter.hpp"
#include "ndn-cxx/security/v2/certificate.hpp"

namespace ndn {
namespace security {

namespace {

struct ParseResult
{
  bool isParsable = false;
  const uint8_t* buf = nullptr;
  size_t bufLen = 0;
  const uint8_t* sig = nullptr;
  size_t sigLen = 0;
};

} // namespace

bool
verifySignature(const uint8_t* blob, size_t blobLen, const uint8_t* sig, size_t sigLen,
                const transform::PublicKey& key)
{
  bool result = false;
  try {
    using namespace transform;
    bufferSource(blob, blobLen) >> verifierFilter(DigestAlgorithm::SHA256, key, sig, sigLen)
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
  transform::PublicKey pKey;
  try {
    pKey.loadPkcs8(key, keyLen);
  }
  catch (const transform::Error&) {
    return false;
  }

  return verifySignature(data, dataLen, sig, sigLen, pKey);
}

static ParseResult
parse(const Data& data)
{
  try {
    return {true,
            data.wireEncode().value(),
            data.wireEncode().value_size() - data.getSignature().getValue().size(),
            data.getSignature().getValue().value(),
            data.getSignature().getValue().value_size()};
  }
  catch (const tlv::Error&) {
    return ParseResult();
  }
}

static ParseResult
parse(const Interest& interest)
{
  const Name& interestName = interest.getName();

  if (interestName.size() < signed_interest::MIN_SIZE)
    return ParseResult();

  try {
    const Block& nameBlock = interestName.wireEncode();
    return {true,
            nameBlock.value(),
            nameBlock.value_size() - interestName[signed_interest::POS_SIG_VALUE].size(),
            interestName[signed_interest::POS_SIG_VALUE].blockFromValue().value(),
            interestName[signed_interest::POS_SIG_VALUE].blockFromValue().value_size()};
  }
  catch (const tlv::Error&) {
    return ParseResult();
  }
}

static bool
verifySignature(ParseResult params, const transform::PublicKey& key)
{
  return params.isParsable && verifySignature(params.buf, params.bufLen,
                                              params.sig, params.sigLen, key);
}

static bool
verifySignature(ParseResult params, const tpm::Tpm& tpm, const Name& keyName,
                DigestAlgorithm digestAlgorithm)
{
  return params.isParsable && bool(tpm.verify(params.buf, params.bufLen,
                                         params.sig, params.sigLen, keyName, digestAlgorithm));
}

static bool
verifySignature(ParseResult params, const uint8_t* key, size_t keyLen)
{
  return params.isParsable && verifySignature(params.buf, params.bufLen,
                                              params.sig, params.sigLen, key, keyLen);
}

bool
verifySignature(const Data& data, const transform::PublicKey& key)
{
  return verifySignature(parse(data), key);
}

bool
verifySignature(const Interest& interest, const transform::PublicKey& key)
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

bool
verifySignature(const Data& data, const tpm::Tpm& tpm,
                const Name& keyName, DigestAlgorithm digestAlgorithm)
{
  return verifySignature(parse(data), tpm, keyName, digestAlgorithm);
}

bool
verifySignature(const Interest& interest, const tpm::Tpm& tpm,
                const Name& keyName, DigestAlgorithm digestAlgorithm)
{
  return verifySignature(parse(interest), tpm, keyName, digestAlgorithm);
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
  ParseResult parseResult = parse(data);
  return parseResult.isParsable && verifyDigest(parseResult.buf, parseResult.bufLen,
                                                parseResult.sig, parseResult.sigLen, algorithm);
}

bool
verifyDigest(const Interest& interest, DigestAlgorithm algorithm)
{
  ParseResult parseResult = parse(interest);
  return parseResult.isParsable && verifyDigest(parseResult.buf, parseResult.bufLen,
                                                parseResult.sig, parseResult.sigLen, algorithm);
}

} // namespace security
} // namespace ndn

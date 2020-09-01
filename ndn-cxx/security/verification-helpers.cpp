/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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
#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/security/certificate.hpp"
#include "ndn-cxx/security/impl/openssl.hpp"
#include "ndn-cxx/security/pib/key.hpp"
#include "ndn-cxx/security/tpm/tpm.hpp"
#include "ndn-cxx/security/transform/bool-sink.hpp"
#include "ndn-cxx/security/transform/buffer-source.hpp"
#include "ndn-cxx/security/transform/digest-filter.hpp"
#include "ndn-cxx/security/transform/public-key.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"
#include "ndn-cxx/security/transform/verifier-filter.hpp"

namespace ndn {
namespace security {

namespace {

class ParseResult
{
public:
  ParseResult() = default;

  ParseResult(InputBuffers bufs, const uint8_t* sig, size_t sigLen)
    : bufs(std::move(bufs))
    , sig(sig)
    , sigLen(sigLen)
  {
  }

public:
  InputBuffers bufs;
  const uint8_t* sig = nullptr;
  size_t sigLen = 0;
};

} // namespace

bool
verifySignature(const InputBuffers& blobs, const uint8_t* sig, size_t sigLen,
                const transform::PublicKey& key)
{
  bool result = false;
  try {
    using namespace transform;
    bufferSource(blobs) >> verifierFilter(DigestAlgorithm::SHA256, key, sig, sigLen)
                        >> boolSink(result);
  }
  catch (const transform::Error&) {
    return false;
  }

  return result;
}

bool
verifySignature(const uint8_t* blob, size_t blobLen, const uint8_t* sig, size_t sigLen,
                const transform::PublicKey& key)
{
  return verifySignature({{blob, blobLen}}, sig, sigLen, key);
}

bool
verifySignature(const InputBuffers& blobs, const uint8_t* sig, size_t sigLen,
                const uint8_t* key, size_t keyLen)
{
  transform::PublicKey pKey;
  try {
    pKey.loadPkcs8(key, keyLen);
  }
  catch (const transform::Error&) {
    return false;
  }

  return verifySignature(blobs, sig, sigLen, pKey);
}

bool
verifySignature(const uint8_t* blob, size_t blobLen, const uint8_t* sig, size_t sigLen,
                const uint8_t* key, size_t keyLen)
{
  return verifySignature({{blob, blobLen}}, sig, sigLen, key, keyLen);
}

static ParseResult
parse(const Data& data)
{
  try {
    return ParseResult(data.extractSignedRanges(),
                       data.getSignatureValue().value(),
                       data.getSignatureValue().value_size());
  }
  catch (const tlv::Error&) {
    return ParseResult();
  }
}

static ParseResult
parse(const Interest& interest)
{
  try {
    interest.wireEncode();

    if (interest.getSignatureInfo() && interest.getSignatureValue().isValid()) {
      // Verify using v0.3 Signed Interest semantics
      Block sigValue = interest.getSignatureValue();
      return ParseResult(interest.extractSignedRanges(),
                         sigValue.value(),
                         sigValue.value_size());
    }
    else {
      // Verify using older Signed Interest semantics
      const Name& interestName = interest.getName();
      if (interestName.size() < signed_interest::MIN_SIZE) {
        return ParseResult();
      }

      const Block& nameBlock = interestName.wireEncode();
      Block sigValue = interestName[signed_interest::POS_SIG_VALUE].blockFromValue();
      return ParseResult({{nameBlock.value(),
                           nameBlock.value_size() - interestName[signed_interest::POS_SIG_VALUE].size()}},
                         sigValue.value(),
                         sigValue.value_size());
    }
  }
  catch (const tlv::Error&) {
    return ParseResult();
  }
}

static bool
verifySignature(ParseResult params, const transform::PublicKey& key)
{
  return !params.bufs.empty() && verifySignature(params.bufs, params.sig, params.sigLen, key);
}

static bool
verifySignature(ParseResult params, const tpm::Tpm& tpm, const Name& keyName,
                DigestAlgorithm digestAlgorithm)
{
  return !params.bufs.empty() && bool(tpm.verify(params.bufs, params.sig, params.sigLen, keyName, digestAlgorithm));
}

static bool
verifySignature(ParseResult params, const uint8_t* key, size_t keyLen)
{
  return !params.bufs.empty() && verifySignature(params.bufs, params.sig, params.sigLen, key, keyLen);
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
verifyDigest(const InputBuffers& bufs, const uint8_t* digest, size_t digestLen,
             DigestAlgorithm algorithm)
{
  using namespace transform;

  OBufferStream os;
  try {
    bufferSource(bufs) >> digestFilter(algorithm) >> streamSink(os);
  }
  catch (const transform::Error&) {
    return false;
  }
  ConstBufferPtr result = os.buf();

  if (result->size() != digestLen) {
    return false;
  }

  // constant-time buffer comparison to mitigate timing attacks
  return CRYPTO_memcmp(result->data(), digest, digestLen) == 0;
}

bool
verifyDigest(const uint8_t* blob, size_t blobLen, const uint8_t* digest, size_t digestLen,
             DigestAlgorithm algorithm)
{
  return verifyDigest({{blob, blobLen}}, digest, digestLen, algorithm);
}

bool
verifyDigest(const Data& data, DigestAlgorithm algorithm)
{
  ParseResult parseResult = parse(data);
  return !parseResult.bufs.empty() && verifyDigest(parseResult.bufs, parseResult.sig,
                                                   parseResult.sigLen, algorithm);
}

bool
verifyDigest(const Interest& interest, DigestAlgorithm algorithm)
{
  ParseResult parseResult = parse(interest);
  return !parseResult.bufs.empty() && verifyDigest(parseResult.bufs, parseResult.sig,
                                                   parseResult.sigLen, algorithm);
}

} // namespace security
} // namespace ndn

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

  ParseResult(SignatureInfo info, InputBuffers bufs, span<const uint8_t> sig)
    : info(std::move(info))
    , bufs(std::move(bufs))
    , sig(sig)
  {
  }

public:
  SignatureInfo info;
  InputBuffers bufs;
  span<const uint8_t> sig;
};

} // namespace

bool
verifySignature(const InputBuffers& blobs, span<const uint8_t> sig, const transform::PublicKey& key)
{
  bool result = false;
  try {
    using namespace transform;
    bufferSource(blobs) >> verifierFilter(DigestAlgorithm::SHA256, key, sig)
                        >> boolSink(result);
  }
  catch (const transform::Error&) {
    return false;
  }

  return result;
}

bool
verifySignature(const InputBuffers& blobs, span<const uint8_t> sig, span<const uint8_t> key)
{
  transform::PublicKey pKey;
  try {
    pKey.loadPkcs8(key);
  }
  catch (const transform::Error&) {
    return false;
  }

  return verifySignature(blobs, sig, pKey);
}

static ParseResult
parse(const Data& data)
{
  try {
    return {data.getSignatureInfo(), data.extractSignedRanges(),
            data.getSignatureValue().value_bytes()};
  }
  catch (const tlv::Error&) {
    return {};
  }
}

static ParseResult
parse(const Interest& interest)
{
  try {
    interest.wireEncode();

    if (interest.getSignatureInfo() && interest.getSignatureValue().isValid()) {
      // Verify using v0.3 Signed Interest semantics
      return {*interest.getSignatureInfo(), interest.extractSignedRanges(),
              interest.getSignatureValue().value_bytes()};
    }
    else {
      // Verify using older Signed Interest semantics
      const Name& interestName = interest.getName();
      if (interestName.size() < signed_interest::MIN_SIZE) {
        return {};
      }

      const Block& nameBlock = interestName.wireEncode();
      SignatureInfo info(interestName[signed_interest::POS_SIG_INFO].blockFromValue());
      Block sigValue(interestName[signed_interest::POS_SIG_VALUE].blockFromValue());
      return {info,
              {{nameBlock.value(),
                nameBlock.value_size() - interestName[signed_interest::POS_SIG_VALUE].size()}},
              sigValue.value_bytes()};
    }
  }
  catch (const tlv::Error&) {
    return {};
  }
}

static bool
verifySignature(const ParseResult& params, const transform::PublicKey& key)
{
  return !params.bufs.empty() && verifySignature(params.bufs, params.sig, key);
}

static bool
verifySignature(const ParseResult& params, span<const uint8_t> key)
{
  return !params.bufs.empty() && verifySignature(params.bufs, params.sig, key);
}

static bool
verifySignature(const ParseResult& params, const tpm::Tpm& tpm, const Name& keyName,
                DigestAlgorithm digestAlgorithm)
{
  return !params.bufs.empty() && bool(tpm.verify(params.bufs, params.sig, keyName, digestAlgorithm));
}

static bool
verifyDigest(const ParseResult& params, DigestAlgorithm algorithm)
{
  if (params.bufs.empty()) {
    return false;
  }

  OBufferStream os;
  try {
    using namespace transform;
    bufferSource(params.bufs) >> digestFilter(algorithm) >> streamSink(os);
  }
  catch (const transform::Error&) {
    return false;
  }
  auto result = os.buf();

  if (result->size() != params.sig.size()) {
    return false;
  }

  // constant-time buffer comparison to mitigate timing attacks
  return CRYPTO_memcmp(result->data(), params.sig.data(), params.sig.size()) == 0;
}

bool
verifySignature(const Data& data, span<const uint8_t> key)
{
  return verifySignature(parse(data), key);
}

bool
verifySignature(const Interest& interest, span<const uint8_t> key)
{
  return verifySignature(parse(interest), key);
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
  return verifySignature(parse(data), key.getPublicKey());
}

bool
verifySignature(const Interest& interest, const pib::Key& key)
{
  return verifySignature(parse(interest), key.getPublicKey());
}

bool
verifySignature(const Data& data, const optional<Certificate>& cert)
{
  auto parsed = parse(data);
  if (cert) {
    return verifySignature(parsed, cert->getContent().value_bytes());
  }
  else if (parsed.info.getSignatureType() == tlv::SignatureTypeValue::DigestSha256) {
    return verifyDigest(parsed, DigestAlgorithm::SHA256);
  }
  // Add any other self-verifying signatures here (if any)
  else {
    return false;
  }
}

bool
verifySignature(const Interest& interest, const optional<Certificate>& cert)
{
  auto parsed = parse(interest);
  if (cert) {
    return verifySignature(parsed, cert->getContent().value_bytes());
  }
  else if (parsed.info.getSignatureType() == tlv::SignatureTypeValue::DigestSha256) {
    return verifyDigest(parsed, DigestAlgorithm::SHA256);
  }
  // Add any other self-verifying signatures here (if any)
  else {
    return false;
  }
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

} // namespace security
} // namespace ndn

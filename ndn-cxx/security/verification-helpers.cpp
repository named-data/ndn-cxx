/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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
//added_GM, by liupenghui 
#if 1
#include <iostream>
#endif

namespace ndn {
namespace security {

namespace {

class ParseResult
{
public:
  ParseResult() = default;

  ParseResult(SignatureInfo info, InputBuffers bufs, const uint8_t* sig, size_t sigLen)
    : info(std::move(info))
    , bufs(std::move(bufs))
    , sig(sig)
    , sigLen(sigLen)
  {
  }

public:
  SignatureInfo info;
  InputBuffers bufs;
  const uint8_t* sig = nullptr;
  size_t sigLen = 0;
};

} // namespace
//added_GM, by liupenghui 
#if 1
bool
verifySignature(const InputBuffers& blobs, const uint8_t* sig, size_t sigLen,
                const transform::PublicKey& key, KeyType keyType)
{
  bool result = false;
  try {
    using namespace transform;
	if (keyType == KeyType::SM2) {
	  bufferSource(blobs) >> verifierFilter(DigestAlgorithm::SM3, key, keyType, sig, sigLen)
						  >> boolSink(result);
	} else {
	  bufferSource(blobs) >> verifierFilter(DigestAlgorithm::SHA256, key, keyType, sig, sigLen)
							>> boolSink(result);
	}
  }
  catch (const transform::Error&) {
    return false;
  }

  return result;
}

bool
verifySignature(const InputBuffers& blobs, const uint8_t* sig, size_t sigLen,
                const uint8_t* key, size_t keyLen, KeyType keyType)
{
  transform::PublicKey pKey;
  try {
    pKey.loadPkcs8(key, keyLen);
  }
  catch (const transform::Error&) {
    return false;
  }

  return verifySignature(blobs, sig, sigLen, pKey, keyType);
}

#else
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

#endif


static ParseResult
parse(const Data& data)
{
  try {
    return ParseResult(data.getSignatureInfo(),
                       data.extractSignedRanges(),
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
      return ParseResult(*interest.getSignatureInfo(),
                         interest.extractSignedRanges(),
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
      SignatureInfo info(interestName[signed_interest::POS_SIG_INFO].blockFromValue());
      Block sigValue(interestName[signed_interest::POS_SIG_VALUE].blockFromValue());
      return ParseResult(info,
                         {{nameBlock.value(),
                           nameBlock.value_size() - interestName[signed_interest::POS_SIG_VALUE].size()}},
                         sigValue.value(),
                         sigValue.value_size());
    }
  }
  catch (const tlv::Error&) {
    return ParseResult();
  }
}


//added_GM, by liupenghui
#if 1
static bool
verifySignature(const ParseResult& params, const transform::PublicKey& key, KeyType keyType)
{
  return !params.bufs.empty() && verifySignature(params.bufs, params.sig, params.sigLen, key, keyType);
}

static bool
verifySignature(const ParseResult& params, const uint8_t* key, size_t keyLen, KeyType keyType)
{
  return !params.bufs.empty() && verifySignature(params.bufs, params.sig, params.sigLen, key, keyLen, keyType);
}

static bool
verifySignature(const ParseResult& params, const tpm::Tpm& tpm, const Name& keyName, KeyType keyType,
                DigestAlgorithm digestAlgorithm)
{
  return !params.bufs.empty() && bool(tpm.verify(params.bufs, params.sig, params.sigLen, keyName, keyType,
                                                 digestAlgorithm));
}

#else
static bool
verifySignature(const ParseResult& params, const transform::PublicKey& key)
{
  return !params.bufs.empty() && verifySignature(params.bufs, params.sig, params.sigLen, key);
}

static bool
verifySignature(const ParseResult& params, const uint8_t* key, size_t keyLen)
{
  return !params.bufs.empty() && verifySignature(params.bufs, params.sig, params.sigLen, key, keyLen);
}

static bool
verifySignature(const ParseResult& params, const tpm::Tpm& tpm, const Name& keyName,
                DigestAlgorithm digestAlgorithm)
{
  return !params.bufs.empty() && bool(tpm.verify(params.bufs, params.sig, params.sigLen, keyName,
                                                 digestAlgorithm));
}
#endif


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

  if (result->size() != params.sigLen) {
    return false;
  }

  // constant-time buffer comparison to mitigate timing attacks
  return CRYPTO_memcmp(result->data(), params.sig, params.sigLen) == 0;
}

bool
verifySignature(const Data& data, const uint8_t* key, size_t keyLen)
{
//added_GM, by liupenghui 
#if 1
  KeyType keyType = KeyType::NONE;
  int32_t Signature_type = data.getSignatureType();
  if (Signature_type == tlv::SignatureSha256WithRsa)
	keyType =  KeyType::RSA;
  else if (Signature_type == tlv::SignatureSha256WithEcdsa)
	keyType =  KeyType::EC;
  else if (Signature_type == tlv::SignatureHmacWithSha256)
	keyType =  KeyType::HMAC;
  else if (Signature_type == tlv::SignatureSm3WithSm2)
	keyType =  KeyType::SM2;
  else
	keyType = KeyType::NONE;
  
  return verifySignature(parse(data), key, keyLen, keyType);
#else
  return verifySignature(parse(data), key, keyLen);
#endif
}

bool
verifySignature(const Interest& interest, const uint8_t* key, size_t keyLen)
{
//added_GM, by liupenghui 
#if 1
  KeyType keyType = KeyType::NONE;

  try {
	interest.wireEncode();

	if (interest.getSignatureInfo()) {
	  // Verify using v0.3 Signed Interest semantics
	  SignatureInfo info = *interest.getSignatureInfo();
	  int32_t Signature_type = info.getSignatureType();
	  if (Signature_type == tlv::SignatureSha256WithRsa)
		keyType =  KeyType::RSA;
	  else if (Signature_type == tlv::SignatureSha256WithEcdsa)
		keyType =  KeyType::EC;
	  else if (Signature_type == tlv::SignatureHmacWithSha256)
		keyType =  KeyType::HMAC;
	  else if (Signature_type == tlv::SignatureSm3WithSm2)
		keyType =  KeyType::SM2;
	  else
		keyType = KeyType::NONE;
	}
	else {
	  // Verify using older Signed Interest semantics
	  const Name& interestName = interest.getName();
	  if (interestName.size() < signed_interest::MIN_SIZE) {
		return false;
	  }

	  SignatureInfo info(interestName[signed_interest::POS_SIG_INFO].blockFromValue());
	  int32_t Signature_type = info.getSignatureType();
	  if (Signature_type == tlv::SignatureSha256WithRsa)
		keyType =  KeyType::RSA;
	  else if (Signature_type == tlv::SignatureSha256WithEcdsa)
		keyType =  KeyType::EC;
	  else if (Signature_type == tlv::SignatureHmacWithSha256)
		keyType =  KeyType::HMAC;
	  else if (Signature_type == tlv::SignatureSm3WithSm2)
		keyType =  KeyType::SM2;
	  else
		keyType = KeyType::NONE;
	}
  }
  catch (const tlv::Error&) {
	return false;
  }
  
  return verifySignature(parse(interest), key, keyLen, keyType);
#else
  return verifySignature(parse(interest), key, keyLen);
#endif

}

bool
verifySignature(const Data& data, const transform::PublicKey& key)
{
//added_GM, by liupenghui 
//the PublicKey.getKeyType() can't differ the SM2-type key from ECDSA key.
#if 1
  KeyType keyType = KeyType::NONE;
  int32_t Signature_type = data.getSignatureType();
  if (Signature_type == tlv::SignatureSha256WithRsa)
	keyType =  KeyType::RSA;
  else if (Signature_type == tlv::SignatureSha256WithEcdsa)
	keyType =  KeyType::EC;
  else if (Signature_type == tlv::SignatureHmacWithSha256)
	keyType =  KeyType::HMAC;
  else if (Signature_type == tlv::SignatureSm3WithSm2)
	keyType =  KeyType::SM2;
  else
	keyType = KeyType::NONE;
  
  return verifySignature(parse(data), key, keyType);
#else
  return verifySignature(parse(data), key);
#endif
}

bool
verifySignature(const Interest& interest, const transform::PublicKey& key)
{
//added_GM, by liupenghui 
//the PublicKey.getKeyType() can't differ the SM2-type key from ECDSA key.
#if 1
  KeyType keyType = KeyType::NONE;

  try {
	interest.wireEncode();

	if (interest.getSignatureInfo()) {
	  // Verify using v0.3 Signed Interest semantics
	  SignatureInfo info = *interest.getSignatureInfo();
	  int32_t Signature_type = info.getSignatureType();
	  if (Signature_type == tlv::SignatureSha256WithRsa)
		keyType =  KeyType::RSA;
	  else if (Signature_type == tlv::SignatureSha256WithEcdsa)
		keyType =  KeyType::EC;
	  else if (Signature_type == tlv::SignatureHmacWithSha256)
		keyType =  KeyType::HMAC;
	  else if (Signature_type == tlv::SignatureSm3WithSm2)
		keyType =  KeyType::SM2;
	  else
		keyType = KeyType::NONE;
	}
	else {
	  // Verify using older Signed Interest semantics
	  const Name& interestName = interest.getName();
	  if (interestName.size() < signed_interest::MIN_SIZE) {
		return false;
	  }

	  SignatureInfo info(interestName[signed_interest::POS_SIG_INFO].blockFromValue());
	  int32_t Signature_type = info.getSignatureType();
	  if (Signature_type == tlv::SignatureSha256WithRsa)
		keyType =  KeyType::RSA;
	  else if (Signature_type == tlv::SignatureSha256WithEcdsa)
		keyType =  KeyType::EC;
	  else if (Signature_type == tlv::SignatureHmacWithSha256)
		keyType =  KeyType::HMAC;
	  else if (Signature_type == tlv::SignatureSm3WithSm2)
		keyType =  KeyType::SM2;
	  else
		keyType = KeyType::NONE;
	}
  }
  catch (const tlv::Error&) {
	return false;
  }
  
  return verifySignature(parse(interest), key, keyType);
#else
  return verifySignature(parse(interest), key);
#endif
}

bool
verifySignature(const Data& data, const pib::Key& key)
{
//added_GM, by liupenghui 
#if 1
  KeyType keyType = KeyType::NONE;
  KeyType keyTypefromSig = KeyType::NONE;
  
  int32_t Signature_type = data.getSignatureType();
  if (Signature_type == tlv::SignatureSha256WithRsa)
	keyTypefromSig =  KeyType::RSA;
  else if (Signature_type == tlv::SignatureSha256WithEcdsa)
	keyTypefromSig =  KeyType::EC;
  else if (Signature_type == tlv::SignatureHmacWithSha256)
	keyTypefromSig =  KeyType::HMAC;
  else if (Signature_type == tlv::SignatureSm3WithSm2)
	keyTypefromSig =  KeyType::SM2;
  else
	keyTypefromSig = KeyType::NONE;
  
  keyType = key.getKeyType();
  if(keyTypefromSig != keyType) {
	std::cout<<"the type of data sig must be the same as the one of the key."<<std::endl;
	return false;
  }
  
  return verifySignature(parse(data), key.getPublicKey().data(), key.getPublicKey().size(), keyType);
#else
  return verifySignature(parse(data), key.getPublicKey().data(), key.getPublicKey().size());
#endif
}

bool
verifySignature(const Interest& interest, const pib::Key& key)
{
//added_GM, by liupenghui 
#if 1
  KeyType keyType = KeyType::NONE;
  KeyType keyTypefromSig = KeyType::NONE;
  
  try {
	interest.wireEncode();

	if (interest.getSignatureInfo()) {
	  // Verify using v0.3 Signed Interest semantics
	  SignatureInfo info = *interest.getSignatureInfo();
	  int32_t Signature_type = info.getSignatureType();
	  if (Signature_type == tlv::SignatureSha256WithRsa)
		keyTypefromSig =  KeyType::RSA;
	  else if (Signature_type == tlv::SignatureSha256WithEcdsa)
		keyTypefromSig =  KeyType::EC;
	  else if (Signature_type == tlv::SignatureHmacWithSha256)
		keyTypefromSig =  KeyType::HMAC;
	  else if (Signature_type == tlv::SignatureSm3WithSm2)
		keyTypefromSig =  KeyType::SM2;
	  else
		keyTypefromSig = KeyType::NONE;
	}
	else {
	  // Verify using older Signed Interest semantics
	  const Name& interestName = interest.getName();
	  if (interestName.size() < signed_interest::MIN_SIZE) {
		return false;
	  }

	  SignatureInfo info(interestName[signed_interest::POS_SIG_INFO].blockFromValue());
	  int32_t Signature_type = info.getSignatureType();
	  if (Signature_type == tlv::SignatureSha256WithRsa)
		keyTypefromSig =  KeyType::RSA;
	  else if (Signature_type == tlv::SignatureSha256WithEcdsa)
		keyTypefromSig =  KeyType::EC;
	  else if (Signature_type == tlv::SignatureHmacWithSha256)
		keyTypefromSig =  KeyType::HMAC;
	  else if (Signature_type == tlv::SignatureSm3WithSm2)
		keyTypefromSig =  KeyType::SM2;
	  else
		keyTypefromSig = KeyType::NONE;
	}
  }
  catch (const tlv::Error&) {
	return false;
  }
  
  keyType = key.getKeyType();
  if(keyTypefromSig != keyType) {
	std::cout<<"the type of interest sig must be the same as the one of the key."<<std::endl;
	return false;
  }
  
  return verifySignature(parse(interest), key.getPublicKey().data(), key.getPublicKey().size(), keyType);
#else
  return verifySignature(parse(interest), key.getPublicKey().data(), key.getPublicKey().size());
#endif
}

bool
verifySignature(const Data& data, const optional<Certificate>& cert)
{
  auto parsed = parse(data);
  if (cert) {
//added_GM, by liupenghui 
#if 1 
    KeyType keyTypefromData = KeyType::NONE;
    KeyType keyTypefromCert = KeyType::NONE;
    
    int32_t Signature_type = data.getSignatureType();
    if (Signature_type == tlv::SignatureSha256WithRsa)
  	keyTypefromData =  KeyType::RSA;
    else if (Signature_type == tlv::SignatureSha256WithEcdsa)
  	keyTypefromData =  KeyType::EC;
    else if (Signature_type == tlv::SignatureHmacWithSha256)
  	keyTypefromData =  KeyType::HMAC;
    else if (Signature_type == tlv::SignatureSm3WithSm2)
  	keyTypefromData =  KeyType::SM2;
    else
  	keyTypefromData = KeyType::NONE;
  
    SignatureInfo info = cert->getSignatureInfo();
    Signature_type = info.getSignatureType();
    if (Signature_type == tlv::SignatureSha256WithRsa)
  	keyTypefromCert =  KeyType::RSA;
    else if (Signature_type == tlv::SignatureSha256WithEcdsa)
  	keyTypefromCert =  KeyType::EC;
    else if (Signature_type == tlv::SignatureHmacWithSha256)
  	keyTypefromCert =  KeyType::HMAC;
    else if (Signature_type == tlv::SignatureSm3WithSm2)
  	keyTypefromCert =  KeyType::SM2;
    else
  	keyTypefromCert = KeyType::NONE;
    
    //The type of data  sig key can be different with the one used by certificate signature, but not a good action...
    if (keyTypefromData != keyTypefromCert)
  	std::cout<<"the type of data sig key is not the same as the one used by certificate signature."<<std::endl;
    
    return verifySignature(parsed, cert->getContent().value(), cert->getContent().value_size(), keyTypefromData);
#else  	
    return verifySignature(parsed, cert->getContent().value(), cert->getContent().value_size());
#endif
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
//added_GM, by liupenghui 
#if 1 
    KeyType keyTypefromSig = KeyType::NONE;
    
    try {
  	interest.wireEncode();
    
  	if (interest.getSignatureInfo()) {
  	  // Verify using v0.3 Signed Interest semantics
  	  SignatureInfo info = *interest.getSignatureInfo();
  	  int32_t Signature_type = info.getSignatureType();
  	  if (Signature_type == tlv::SignatureSha256WithRsa)
  		keyTypefromSig =  KeyType::RSA;
  	  else if (Signature_type == tlv::SignatureSha256WithEcdsa)
  		keyTypefromSig =  KeyType::EC;
  	  else if (Signature_type == tlv::SignatureHmacWithSha256)
  		keyTypefromSig =  KeyType::HMAC;
  	  else if (Signature_type == tlv::SignatureSm3WithSm2)
  		keyTypefromSig =  KeyType::SM2;
  	  else
  		keyTypefromSig = KeyType::NONE;
  	}
  	else {
  	  // Verify using older Signed Interest semantics
  	  const Name& interestName = interest.getName();
  	  if (interestName.size() < signed_interest::MIN_SIZE) {
  		return false;
  	  }
    
  	  SignatureInfo info(interestName[signed_interest::POS_SIG_INFO].blockFromValue());
  	  int32_t Signature_type = info.getSignatureType();
  	  if (Signature_type == tlv::SignatureSha256WithRsa)
  		keyTypefromSig =  KeyType::RSA;
  	  else if (Signature_type == tlv::SignatureSha256WithEcdsa)
  		keyTypefromSig =  KeyType::EC;
  	  else if (Signature_type == tlv::SignatureHmacWithSha256)
  		keyTypefromSig =  KeyType::HMAC;
  	  else if (Signature_type == tlv::SignatureSm3WithSm2)
  		keyTypefromSig =  KeyType::SM2;
  	  else
  		keyTypefromSig = KeyType::NONE;
  	}
    }
    catch (const tlv::Error&) {
  	return false;
    }
    
    KeyType keyTypefromCert = KeyType::NONE;
    SignatureInfo info = cert->getSignatureInfo();
    int32_t Signature_type = info.getSignatureType();
    if (Signature_type == tlv::SignatureSha256WithRsa)
  	keyTypefromCert =  KeyType::RSA;
    else if (Signature_type == tlv::SignatureSha256WithEcdsa)
  	keyTypefromCert =  KeyType::EC;
    else if (Signature_type == tlv::SignatureHmacWithSha256)
  	keyTypefromCert =  KeyType::HMAC;
    else if (Signature_type == tlv::SignatureSm3WithSm2)
  	keyTypefromCert =  KeyType::SM2;
    else
  	keyTypefromCert = KeyType::NONE;
  
    //The type of interest sig key can be different with the one used by certificate signature, but not a good action...
    if (keyTypefromSig != keyTypefromCert)
  	std::cout<<"the type of data sig key is not the same as the one used by certificate signature."<<std::endl;
    
    return verifySignature(parsed, cert->getContent().value(), cert->getContent().value_size(), keyTypefromSig);
#else  	
    return verifySignature(parsed, cert->getContent().value(), cert->getContent().value_size());
#endif
  }
  else if (parsed.info.getSignatureType() == tlv::SignatureTypeValue::DigestSha256) {
    return verifyDigest(parsed, DigestAlgorithm::SHA256);
  }
  // Add any other self-verifying signatures here (if any)
  else {
    return false;
  }
}

//added_GM, by liupenghui 
#if 1
bool
verifySignature(const Data& data, const tpm::Tpm& tpm,
                const Name& keyName,KeyType keyType, DigestAlgorithm digestAlgorithm)
{
  return verifySignature(parse(data), tpm, keyName, keyType, digestAlgorithm);
}

bool
verifySignature(const Interest& interest, const tpm::Tpm& tpm,
                const Name& keyName, KeyType keyType, DigestAlgorithm digestAlgorithm)
{
  return verifySignature(parse(interest), tpm, keyName, keyType, digestAlgorithm);
}

#else
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
#endif


} // namespace security
} // namespace ndn


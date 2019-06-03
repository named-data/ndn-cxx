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

#include "ndn-cxx/security/transform/private-key.hpp"
#include "ndn-cxx/security/transform/base64-decode.hpp"
#include "ndn-cxx/security/transform/base64-encode.hpp"
#include "ndn-cxx/security/transform/buffer-source.hpp"
#include "ndn-cxx/security/transform/digest-filter.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"
#include "ndn-cxx/security/transform/stream-source.hpp"
#include "ndn-cxx/security/impl/openssl-helper.hpp"
#include "ndn-cxx/security/key-params.hpp"
#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/util/random.hpp"

#include <boost/lexical_cast.hpp>
#include <cstring>

#define ENSURE_PRIVATE_KEY_LOADED(key) \
  do { \
    if ((key) == nullptr) \
      NDN_THROW(Error("Private key has not been loaded yet")); \
  } while (false)

#define ENSURE_PRIVATE_KEY_NOT_LOADED(key) \
  do { \
    if ((key) != nullptr) \
      NDN_THROW(Error("Private key has already been loaded")); \
  } while (false)

namespace ndn {
namespace security {
namespace transform {

static void
opensslInitAlgorithms()
{
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
  static bool isInitialized = false;
  if (!isInitialized) {
    OpenSSL_add_all_algorithms();
    isInitialized = true;
  }
#endif // OPENSSL_VERSION_NUMBER < 0x1010000fL
}

class PrivateKey::Impl : noncopyable
{
public:
  ~Impl()
  {
    EVP_PKEY_free(key);
  }

public:
  EVP_PKEY* key = nullptr;

#if OPENSSL_VERSION_NUMBER < 0x1010100fL
  size_t keySize = 0; // in bits, used only for HMAC
#endif
};

PrivateKey::PrivateKey()
  : m_impl(make_unique<Impl>())
{
}

PrivateKey::~PrivateKey() = default;

KeyType
PrivateKey::getKeyType() const
{
  if (!m_impl->key)
    return KeyType::NONE;

  switch (detail::getEvpPkeyType(m_impl->key)) {
  case EVP_PKEY_RSA:
    return KeyType::RSA;
  case EVP_PKEY_EC:
    return KeyType::EC;
  case EVP_PKEY_HMAC:
    return KeyType::HMAC;
  default:
    return KeyType::NONE;
  }
}

size_t
PrivateKey::getKeySize() const
{
  switch (getKeyType()) {
    case KeyType::RSA:
    case KeyType::EC:
      return static_cast<size_t>(EVP_PKEY_bits(m_impl->key));
    case KeyType::HMAC: {
#if OPENSSL_VERSION_NUMBER >= 0x1010100fL
      size_t nBytes = 0;
      EVP_PKEY_get_raw_private_key(m_impl->key, nullptr, &nBytes);
      return nBytes * 8;
#else
      return m_impl->keySize;
#endif
    }
    default:
      return 0;
  }
}

ConstBufferPtr
PrivateKey::getKeyDigest(DigestAlgorithm algo) const
{
  if (getKeyType() != KeyType::HMAC)
    NDN_THROW(Error("Digest is not supported for key type " +
                    boost::lexical_cast<std::string>(getKeyType())));

  const uint8_t* buf = nullptr;
  size_t len = 0;
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
  buf = EVP_PKEY_get0_hmac(m_impl->key, &len);
#else
  const auto* octstr = reinterpret_cast<ASN1_OCTET_STRING*>(EVP_PKEY_get0(m_impl->key));
  buf = octstr->data;
  len = octstr->length;
#endif
  if (buf == nullptr)
    NDN_THROW(Error("Failed to obtain raw key pointer"));
  if (len * 8 != getKeySize())
    NDN_THROW(Error("Key length mismatch"));

  OBufferStream os;
  bufferSource(buf, len) >> digestFilter(algo) >> streamSink(os);
  return os.buf();
}

void
PrivateKey::loadRaw(KeyType type, const uint8_t* buf, size_t size)
{
  ENSURE_PRIVATE_KEY_NOT_LOADED(m_impl->key);

  int pkeyType;
  switch (type) {
  case KeyType::HMAC:
    pkeyType = EVP_PKEY_HMAC;
    break;
  default:
    NDN_THROW(std::invalid_argument("Unsupported key type " + boost::lexical_cast<std::string>(type)));
  }

  m_impl->key =
#if OPENSSL_VERSION_NUMBER >= 0x1010100fL
      EVP_PKEY_new_raw_private_key(pkeyType, nullptr, buf, size);
#else
      EVP_PKEY_new_mac_key(pkeyType, nullptr, buf, static_cast<int>(size));
#endif
  if (m_impl->key == nullptr)
    NDN_THROW(Error("Failed to load private key"));

#if OPENSSL_VERSION_NUMBER < 0x1010100fL
  m_impl->keySize = size * 8;
#endif
}

void
PrivateKey::loadPkcs1(const uint8_t* buf, size_t size)
{
  ENSURE_PRIVATE_KEY_NOT_LOADED(m_impl->key);
  opensslInitAlgorithms();

  if (d2i_AutoPrivateKey(&m_impl->key, &buf, static_cast<long>(size)) == nullptr)
    NDN_THROW(Error("Failed to load private key"));
}

void
PrivateKey::loadPkcs1(std::istream& is)
{
  OBufferStream os;
  streamSource(is) >> streamSink(os);
  this->loadPkcs1(os.buf()->data(), os.buf()->size());
}

void
PrivateKey::loadPkcs1Base64(const uint8_t* buf, size_t size)
{
  OBufferStream os;
  bufferSource(buf, size) >> base64Decode() >> streamSink(os);
  this->loadPkcs1(os.buf()->data(), os.buf()->size());
}

void
PrivateKey::loadPkcs1Base64(std::istream& is)
{
  OBufferStream os;
  streamSource(is) >> base64Decode() >> streamSink(os);
  this->loadPkcs1(os.buf()->data(), os.buf()->size());
}

void
PrivateKey::loadPkcs8(const uint8_t* buf, size_t size, const char* pw, size_t pwLen)
{
  BOOST_ASSERT(std::strlen(pw) == pwLen);
  ENSURE_PRIVATE_KEY_NOT_LOADED(m_impl->key);
  opensslInitAlgorithms();

  detail::Bio membio(BIO_s_mem());
  if (!membio.write(buf, size))
    NDN_THROW(Error("Failed to copy buffer"));

  if (d2i_PKCS8PrivateKey_bio(membio, &m_impl->key, nullptr, const_cast<char*>(pw)) == nullptr)
    NDN_THROW(Error("Failed to load private key"));
}

static inline int
passwordCallbackWrapper(char* buf, int size, int rwflag, void* u)
{
  BOOST_ASSERT(size >= 0);
  auto cb = reinterpret_cast<PrivateKey::PasswordCallback*>(u);
  return (*cb)(buf, static_cast<size_t>(size), rwflag);
}

void
PrivateKey::loadPkcs8(const uint8_t* buf, size_t size, PasswordCallback pwCallback)
{
  ENSURE_PRIVATE_KEY_NOT_LOADED(m_impl->key);
  opensslInitAlgorithms();

  detail::Bio membio(BIO_s_mem());
  if (!membio.write(buf, size))
    NDN_THROW(Error("Failed to copy buffer"));

  if (pwCallback)
    m_impl->key = d2i_PKCS8PrivateKey_bio(membio, nullptr, &passwordCallbackWrapper, &pwCallback);
  else
    m_impl->key = d2i_PKCS8PrivateKey_bio(membio, nullptr, nullptr, nullptr);

  if (m_impl->key == nullptr)
    NDN_THROW(Error("Failed to load private key"));
}

void
PrivateKey::loadPkcs8(std::istream& is, const char* pw, size_t pwLen)
{
  OBufferStream os;
  streamSource(is) >> streamSink(os);
  this->loadPkcs8(os.buf()->data(), os.buf()->size(), pw, pwLen);
}

void
PrivateKey::loadPkcs8(std::istream& is, PasswordCallback pwCallback)
{
  OBufferStream os;
  streamSource(is) >> streamSink(os);
  this->loadPkcs8(os.buf()->data(), os.buf()->size(), pwCallback);
}

void
PrivateKey::loadPkcs8Base64(const uint8_t* buf, size_t size, const char* pw, size_t pwLen)
{
  OBufferStream os;
  bufferSource(buf, size) >> base64Decode() >> streamSink(os);
  this->loadPkcs8(os.buf()->data(), os.buf()->size(), pw, pwLen);
}

void
PrivateKey::loadPkcs8Base64(const uint8_t* buf, size_t size, PasswordCallback pwCallback)
{
  OBufferStream os;
  bufferSource(buf, size) >> base64Decode() >> streamSink(os);
  this->loadPkcs8(os.buf()->data(), os.buf()->size(), pwCallback);
}

void
PrivateKey::loadPkcs8Base64(std::istream& is, const char* pw, size_t pwLen)
{
  OBufferStream os;
  streamSource(is) >> base64Decode() >> streamSink(os);
  this->loadPkcs8(os.buf()->data(), os.buf()->size(), pw, pwLen);
}

void
PrivateKey::loadPkcs8Base64(std::istream& is, PasswordCallback pwCallback)
{
  OBufferStream os;
  streamSource(is) >> base64Decode() >> streamSink(os);
  this->loadPkcs8(os.buf()->data(), os.buf()->size(), pwCallback);
}

void
PrivateKey::savePkcs1(std::ostream& os) const
{
  bufferSource(*this->toPkcs1()) >> streamSink(os);
}

void
PrivateKey::savePkcs1Base64(std::ostream& os) const
{
  bufferSource(*this->toPkcs1()) >> base64Encode() >> streamSink(os);
}

void
PrivateKey::savePkcs8(std::ostream& os, const char* pw, size_t pwLen) const
{
  bufferSource(*this->toPkcs8(pw, pwLen)) >> streamSink(os);
}

void
PrivateKey::savePkcs8(std::ostream& os, PasswordCallback pwCallback) const
{
  bufferSource(*this->toPkcs8(pwCallback)) >> streamSink(os);
}

void
PrivateKey::savePkcs8Base64(std::ostream& os, const char* pw, size_t pwLen) const
{
  bufferSource(*this->toPkcs8(pw, pwLen)) >> base64Encode() >> streamSink(os);
}

void
PrivateKey::savePkcs8Base64(std::ostream& os, PasswordCallback pwCallback) const
{
  bufferSource(*this->toPkcs8(pwCallback)) >> base64Encode() >> streamSink(os);
}

ConstBufferPtr
PrivateKey::derivePublicKey() const
{
  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);

  uint8_t* pkcs8 = nullptr;
  int len = i2d_PUBKEY(m_impl->key, &pkcs8);
  if (len < 0)
    NDN_THROW(Error("Failed to derive public key"));

  auto result = make_shared<Buffer>(pkcs8, len);
  OPENSSL_free(pkcs8);

  return result;
}

ConstBufferPtr
PrivateKey::decrypt(const uint8_t* cipherText, size_t cipherLen) const
{
  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);

  int keyType = detail::getEvpPkeyType(m_impl->key);
  switch (keyType) {
    case EVP_PKEY_NONE:
      NDN_THROW(Error("Failed to determine key type"));
    case EVP_PKEY_RSA:
      return rsaDecrypt(cipherText, cipherLen);
    default:
      NDN_THROW(Error("Decryption is not supported for key type " + to_string(keyType)));
  }
}

void*
PrivateKey::getEvpPkey() const
{
  return m_impl->key;
}

ConstBufferPtr
PrivateKey::toPkcs1() const
{
  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);
  opensslInitAlgorithms();

  detail::Bio membio(BIO_s_mem());
  if (!i2d_PrivateKey_bio(membio, m_impl->key))
    NDN_THROW(Error("Cannot convert key to PKCS #1 format"));

  auto buffer = make_shared<Buffer>(BIO_pending(membio));
  if (!membio.read(buffer->data(), buffer->size()))
    NDN_THROW(Error("Read error during PKCS #1 conversion"));

  return buffer;
}

ConstBufferPtr
PrivateKey::toPkcs8(const char* pw, size_t pwLen) const
{
  BOOST_ASSERT(std::strlen(pw) == pwLen);
  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);
  opensslInitAlgorithms();

  detail::Bio membio(BIO_s_mem());
  if (!i2d_PKCS8PrivateKey_bio(membio, m_impl->key, EVP_aes_256_cbc(), nullptr, 0,
                               nullptr, const_cast<char*>(pw)))
    NDN_THROW(Error("Cannot convert key to PKCS #8 format"));

  auto buffer = make_shared<Buffer>(BIO_pending(membio));
  if (!membio.read(buffer->data(), buffer->size()))
    NDN_THROW(Error("Read error during PKCS #8 conversion"));

  return buffer;
}

ConstBufferPtr
PrivateKey::toPkcs8(PasswordCallback pwCallback) const
{
  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);
  opensslInitAlgorithms();

  detail::Bio membio(BIO_s_mem());
  if (!i2d_PKCS8PrivateKey_bio(membio, m_impl->key, EVP_aes_256_cbc(), nullptr, 0,
                               &passwordCallbackWrapper, &pwCallback))
    NDN_THROW(Error("Cannot convert key to PKCS #8 format"));

  auto buffer = make_shared<Buffer>(BIO_pending(membio));
  if (!membio.read(buffer->data(), buffer->size()))
    NDN_THROW(Error("Read error during PKCS #8 conversion"));

  return buffer;
}

ConstBufferPtr
PrivateKey::rsaDecrypt(const uint8_t* cipherText, size_t cipherLen) const
{
  detail::EvpPkeyCtx ctx(m_impl->key);

  if (EVP_PKEY_decrypt_init(ctx) <= 0)
    NDN_THROW(Error("Failed to initialize decryption context"));

  if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0)
    NDN_THROW(Error("Failed to set padding"));

  size_t outlen = 0;
  // Determine buffer length
  if (EVP_PKEY_decrypt(ctx, nullptr, &outlen, cipherText, cipherLen) <= 0)
    NDN_THROW(Error("Failed to estimate output length"));

  auto out = make_shared<Buffer>(outlen);
  if (EVP_PKEY_decrypt(ctx, out->data(), &outlen, cipherText, cipherLen) <= 0)
    NDN_THROW(Error("Failed to decrypt ciphertext"));

  out->resize(outlen);
  return out;
}

unique_ptr<PrivateKey>
PrivateKey::generateRsaKey(uint32_t keySize)
{
  detail::EvpPkeyCtx kctx(EVP_PKEY_RSA);

  if (EVP_PKEY_keygen_init(kctx) <= 0)
    NDN_THROW(PrivateKey::Error("Failed to initialize RSA keygen context"));

  if (EVP_PKEY_CTX_set_rsa_keygen_bits(kctx, static_cast<int>(keySize)) <= 0)
    NDN_THROW(PrivateKey::Error("Failed to set RSA key length"));

  auto privateKey = make_unique<PrivateKey>();
  if (EVP_PKEY_keygen(kctx, &privateKey->m_impl->key) <= 0)
    NDN_THROW(PrivateKey::Error("Failed to generate RSA key"));

  return privateKey;
}

unique_ptr<PrivateKey>
PrivateKey::generateEcKey(uint32_t keySize)
{
  detail::EvpPkeyCtx pctx(EVP_PKEY_EC);

  if (EVP_PKEY_paramgen_init(pctx) <= 0)
    NDN_THROW(PrivateKey::Error("Failed to initialize EC paramgen context"));

  int ret;
  switch (keySize) {
  case 224:
    ret = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_secp224r1);
    break;
  case 256:
    ret = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1); // same as secp256r1
    break;
  case 384:
    ret = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_secp384r1);
    break;
  case 521:
    ret = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_secp521r1);
    break;
  default:
    NDN_THROW(std::invalid_argument("Unsupported EC key length " + to_string(keySize)));
  }
  if (ret <= 0)
    NDN_THROW(PrivateKey::Error("Failed to set EC curve"));

  Impl params;
  if (EVP_PKEY_paramgen(pctx, &params.key) <= 0)
    NDN_THROW(PrivateKey::Error("Failed to generate EC parameters"));

  detail::EvpPkeyCtx kctx(params.key);
  if (EVP_PKEY_keygen_init(kctx) <= 0)
    NDN_THROW(PrivateKey::Error("Failed to initialize EC keygen context"));

  auto privateKey = make_unique<PrivateKey>();
  if (EVP_PKEY_keygen(kctx, &privateKey->m_impl->key) <= 0)
    NDN_THROW(PrivateKey::Error("Failed to generate EC key"));

  return privateKey;
}

unique_ptr<PrivateKey>
PrivateKey::generateHmacKey(uint32_t keySize)
{
  std::vector<uint8_t> rawKey(keySize / 8);
  random::generateSecureBytes(rawKey.data(), rawKey.size());

  auto privateKey = make_unique<PrivateKey>();
  try {
    privateKey->loadRaw(KeyType::HMAC, rawKey.data(), rawKey.size());
  }
  catch (const PrivateKey::Error&) {
    NDN_THROW(PrivateKey::Error("Failed to generate HMAC key"));
  }

  return privateKey;
}

unique_ptr<PrivateKey>
generatePrivateKey(const KeyParams& keyParams)
{
  switch (keyParams.getKeyType()) {
    case KeyType::RSA: {
      const RsaKeyParams& rsaParams = static_cast<const RsaKeyParams&>(keyParams);
      return PrivateKey::generateRsaKey(rsaParams.getKeySize());
    }
    case KeyType::EC: {
      const EcKeyParams& ecParams = static_cast<const EcKeyParams&>(keyParams);
      return PrivateKey::generateEcKey(ecParams.getKeySize());
    }
    case KeyType::HMAC: {
      const HmacKeyParams& hmacParams = static_cast<const HmacKeyParams&>(keyParams);
      return PrivateKey::generateHmacKey(hmacParams.getKeySize());
    }
    default:
      NDN_THROW(std::invalid_argument("Unsupported key type " +
                                      boost::lexical_cast<std::string>(keyParams.getKeyType())));
  }
}

} // namespace transform
} // namespace security
} // namespace ndn

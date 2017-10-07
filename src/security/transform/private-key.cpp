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

#include "private-key.hpp"
#include "base64-decode.hpp"
#include "base64-encode.hpp"
#include "buffer-source.hpp"
#include "stream-sink.hpp"
#include "stream-source.hpp"
#include "../detail/openssl-helper.hpp"
#include "../key-params.hpp"
#include "../../encoding/buffer-stream.hpp"

#include <boost/lexical_cast.hpp>
#include <cstring>

#define ENSURE_PRIVATE_KEY_LOADED(key) \
  do { \
    if ((key) == nullptr) \
      BOOST_THROW_EXCEPTION(Error("Private key has not been loaded yet")); \
  } while (false)

#define ENSURE_PRIVATE_KEY_NOT_LOADED(key) \
  do { \
    if ((key) != nullptr) \
      BOOST_THROW_EXCEPTION(Error("Private key has already been loaded")); \
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

class PrivateKey::Impl
{
public:
  Impl() noexcept
    : key(nullptr)
  {
  }

  ~Impl()
  {
    EVP_PKEY_free(key);
  }

public:
  EVP_PKEY* key;
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
  default:
    return KeyType::NONE;
  }
}

void
PrivateKey::loadPkcs1(const uint8_t* buf, size_t size)
{
  ENSURE_PRIVATE_KEY_NOT_LOADED(m_impl->key);
  opensslInitAlgorithms();

  if (d2i_AutoPrivateKey(&m_impl->key, &buf, static_cast<long>(size)) == nullptr)
    BOOST_THROW_EXCEPTION(Error("Failed to load private key"));
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
    BOOST_THROW_EXCEPTION(Error("Failed to copy buffer"));

  if (d2i_PKCS8PrivateKey_bio(membio, &m_impl->key, nullptr, const_cast<char*>(pw)) == nullptr)
    BOOST_THROW_EXCEPTION(Error("Failed to load private key"));
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
    BOOST_THROW_EXCEPTION(Error("Failed to copy buffer"));

  if (pwCallback)
    m_impl->key = d2i_PKCS8PrivateKey_bio(membio, nullptr, &passwordCallbackWrapper, &pwCallback);
  else
    m_impl->key = d2i_PKCS8PrivateKey_bio(membio, nullptr, nullptr, nullptr);

  if (m_impl->key == nullptr)
    BOOST_THROW_EXCEPTION(Error("Failed to load private key"));
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
    BOOST_THROW_EXCEPTION(Error("Failed to derive public key"));

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
      BOOST_THROW_EXCEPTION(Error("Failed to determine key type"));
    case EVP_PKEY_RSA:
      return rsaDecrypt(cipherText, cipherLen);
    default:
      BOOST_THROW_EXCEPTION(Error("Decryption is not supported for key type " + to_string(keyType)));
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
    BOOST_THROW_EXCEPTION(Error("Cannot convert key to PKCS #1 format"));

  auto buffer = make_shared<Buffer>(BIO_pending(membio));
  membio.read(buffer->data(), buffer->size());

  return buffer;
}

ConstBufferPtr
PrivateKey::toPkcs8(const char* pw, size_t pwLen) const
{
  BOOST_ASSERT(std::strlen(pw) == pwLen);
  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);
  opensslInitAlgorithms();

  detail::Bio membio(BIO_s_mem());
  if (!i2d_PKCS8PrivateKey_bio(membio, m_impl->key, EVP_des_ede3_cbc(), nullptr, 0,
                               nullptr, const_cast<char*>(pw)))
    BOOST_THROW_EXCEPTION(Error("Cannot convert key to PKCS #8 format"));

  auto buffer = make_shared<Buffer>(BIO_pending(membio));
  membio.read(buffer->data(), buffer->size());

  return buffer;
}

ConstBufferPtr
PrivateKey::toPkcs8(PasswordCallback pwCallback) const
{
  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);
  opensslInitAlgorithms();

  detail::Bio membio(BIO_s_mem());
  if (!i2d_PKCS8PrivateKey_bio(membio, m_impl->key, EVP_des_ede3_cbc(), nullptr, 0,
                               &passwordCallbackWrapper, &pwCallback))
    BOOST_THROW_EXCEPTION(Error("Cannot convert key to PKCS #8 format"));

  auto buffer = make_shared<Buffer>(BIO_pending(membio));
  membio.read(buffer->data(), buffer->size());

  return buffer;
}

ConstBufferPtr
PrivateKey::rsaDecrypt(const uint8_t* cipherText, size_t cipherLen) const
{
  detail::EvpPkeyCtx ctx(m_impl->key);

  if (EVP_PKEY_decrypt_init(ctx) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to initialize decryption context"));

  if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to set padding"));

  size_t outlen = 0;
  // Determine buffer length
  if (EVP_PKEY_decrypt(ctx, nullptr, &outlen, cipherText, cipherLen) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to estimate output length"));

  auto out = make_shared<Buffer>(outlen);
  if (EVP_PKEY_decrypt(ctx, out->data(), &outlen, cipherText, cipherLen) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to decrypt ciphertext"));

  out->resize(outlen);
  return out;
}

unique_ptr<PrivateKey>
PrivateKey::generateRsaKey(uint32_t keySize)
{
  detail::EvpPkeyCtx kctx(EVP_PKEY_RSA);

  if (EVP_PKEY_keygen_init(kctx) <= 0)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Failed to initialize RSA keygen context"));

  if (EVP_PKEY_CTX_set_rsa_keygen_bits(kctx, static_cast<int>(keySize)) <= 0)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Failed to set RSA key length"));

  auto privateKey = make_unique<PrivateKey>();
  if (EVP_PKEY_keygen(kctx, &privateKey->m_impl->key) <= 0)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Failed to generate RSA key"));

  return privateKey;
}

unique_ptr<PrivateKey>
PrivateKey::generateEcKey(uint32_t keySize)
{
  detail::EvpPkeyCtx pctx(EVP_PKEY_EC);

  if (EVP_PKEY_paramgen_init(pctx) <= 0)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Failed to initialize EC paramgen context"));

  int ret;
  switch (keySize) {
    case 256:
      ret = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1); // same as secp256r1
      break;
    case 384:
      ret = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_secp384r1);
      break;
    default:
      BOOST_THROW_EXCEPTION(PrivateKey::Error("Unsupported EC key length"));
  }
  if (ret <= 0)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Failed to set EC curve"));

  Impl params;
  if (EVP_PKEY_paramgen(pctx, &params.key) <= 0)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Failed to generate EC parameters"));

  detail::EvpPkeyCtx kctx(params.key);
  if (EVP_PKEY_keygen_init(kctx) <= 0)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Failed to initialize EC keygen context"));

  auto privateKey = make_unique<PrivateKey>();
  if (EVP_PKEY_keygen(kctx, &privateKey->m_impl->key) <= 0)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Failed to generate EC key"));

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
    default:
      BOOST_THROW_EXCEPTION(std::invalid_argument("Unsupported asymmetric key type " +
                                                  boost::lexical_cast<std::string>(keyParams.getKeyType())));
  }
}

} // namespace transform
} // namespace security
} // namespace ndn

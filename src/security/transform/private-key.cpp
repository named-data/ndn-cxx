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

#include "private-key.hpp"
#include "buffer-source.hpp"
#include "stream-source.hpp"
#include "base64-encode.hpp"
#include "base64-decode.hpp"
#include "stream-sink.hpp"
#include "../../encoding/buffer-stream.hpp"
#include "../detail/openssl-helper.hpp"
#include "../key-params.hpp"

#include <string.h>

#define ENSURE_PRIVATE_KEY_LOADED(key) \
  do { \
    if (key == nullptr) \
      BOOST_THROW_EXCEPTION(Error("Private key has not been loaded yet")); \
  } while (false)

namespace ndn {
namespace security {
namespace transform {

class PrivateKey::Impl
{
public:
  Impl()
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
  : m_impl(new Impl)
{
}

PrivateKey::~PrivateKey() = default;

void
PrivateKey::loadPkcs1(const uint8_t* buf, size_t size)
{
  detail::Bio mem(BIO_s_mem());
  BIO_write(mem.get(), buf, size);

  d2i_PrivateKey_bio(mem.get(), &m_impl->key);

  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);
}

void
PrivateKey::loadPkcs1(std::istream& is)
{
  OBufferStream os;
  streamSource(is) >> streamSink(os);
  this->loadPkcs1(os.buf()->buf(), os.buf()->size());
}

void
PrivateKey::loadPkcs1Base64(const uint8_t* buf, size_t size)
{
  OBufferStream os;
  bufferSource(buf, size) >> base64Decode() >> streamSink(os);
  this->loadPkcs1(os.buf()->buf(), os.buf()->size());
}

void
PrivateKey::loadPkcs1Base64(std::istream& is)
{
  OBufferStream os;
  streamSource(is) >> base64Decode() >> streamSink(os);
  this->loadPkcs1(os.buf()->buf(), os.buf()->size());
}

void
PrivateKey::loadPkcs8(const uint8_t* buf, size_t size, const char* pw, size_t pwLen)
{
  BOOST_ASSERT(std::strlen(pw) == pwLen);

  detail::Bio mem(BIO_s_mem());
  BIO_write(mem.get(), buf, size);

  m_impl->key = d2i_PKCS8PrivateKey_bio(mem.get(), &m_impl->key, nullptr, const_cast<char*>(pw));

  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);
}

static inline int
passwordCallback(char* buf, int size, int rwflag, void* u)
{
  auto cb = reinterpret_cast<PrivateKey::PasswordCallback*>(u);
  return (*cb)(buf, size, rwflag);
}

void
PrivateKey::loadPkcs8(const uint8_t* buf, size_t size, PasswordCallback pwCallback)
{
  OpenSSL_add_all_algorithms();
  detail::Bio mem(BIO_s_mem());
  BIO_write(mem.get(), buf, size);

  if (pwCallback)
    m_impl->key = d2i_PKCS8PrivateKey_bio(mem.get(), &m_impl->key, passwordCallback, &pwCallback);
  else
    m_impl->key = d2i_PKCS8PrivateKey_bio(mem.get(), &m_impl->key, nullptr, nullptr);

  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);
}

void
PrivateKey::loadPkcs8(std::istream& is, const char* pw, size_t pwLen)
{
  OBufferStream os;
  streamSource(is) >> streamSink(os);
  this->loadPkcs8(os.buf()->buf(), os.buf()->size(), pw, pwLen);
}

void
PrivateKey::loadPkcs8(std::istream& is, PasswordCallback pwCallback)
{
  OBufferStream os;
  streamSource(is) >> streamSink(os);
  this->loadPkcs8(os.buf()->buf(), os.buf()->size(), pwCallback);
}

void
PrivateKey::loadPkcs8Base64(const uint8_t* buf, size_t size, const char* pw, size_t pwLen)
{
  OBufferStream os;
  bufferSource(buf, size) >> base64Decode() >> streamSink(os);
  this->loadPkcs8(os.buf()->buf(), os.buf()->size(), pw, pwLen);
}

void
PrivateKey::loadPkcs8Base64(const uint8_t* buf, size_t size, PasswordCallback pwCallback)
{
  OBufferStream os;
  bufferSource(buf, size) >> base64Decode() >> streamSink(os);
  this->loadPkcs8(os.buf()->buf(), os.buf()->size(), pwCallback);
}

void
PrivateKey::loadPkcs8Base64(std::istream& is, const char* pw, size_t pwLen)
{
  OBufferStream os;
  streamSource(is) >> base64Decode() >> streamSink(os);
  this->loadPkcs8(os.buf()->buf(), os.buf()->size(), pw, pwLen);
}

void
PrivateKey::loadPkcs8Base64(std::istream& is, PasswordCallback pwCallback)
{
  OBufferStream os;
  streamSource(is) >> base64Decode() >> streamSink(os);
  this->loadPkcs8(os.buf()->buf(), os.buf()->size(), pwCallback);
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

  if (len <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to derive public key"));

  auto result = make_shared<Buffer>(pkcs8, len);
  OPENSSL_free(pkcs8);

  return result;
}

ConstBufferPtr
PrivateKey::decrypt(const uint8_t* cipherText, size_t cipherLen) const
{
  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);

#if OPENSSL_VERSION_NUMBER < 0x1010000fL
  switch (EVP_PKEY_type(m_impl->key->type)) {
#else
  switch (EVP_PKEY_base_id(m_impl->key)) {
#endif // OPENSSL_VERSION_NUMBER < 0x1010000fL
  case EVP_PKEY_RSA:
    return rsaDecrypt(cipherText, cipherLen);
  default:
    BOOST_THROW_EXCEPTION(Error("Decryption is not supported for this key type"));
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

  OpenSSL_add_all_algorithms();
  detail::Bio mem(BIO_s_mem());
  int ret = i2d_PrivateKey_bio(mem.get(), m_impl->key);
  if (ret != 1)
    BOOST_THROW_EXCEPTION(Error("Cannot convert key into PKCS1 format"));

  int len8 = BIO_pending(mem.get());
  auto buffer = make_shared<Buffer>(len8);
  BIO_read(mem.get(), buffer->buf(), len8);

  return buffer;
}

ConstBufferPtr
PrivateKey::toPkcs8(const char* pw, size_t pwLen) const
{
  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);

  BOOST_ASSERT(std::strlen(pw) == pwLen);

  OpenSSL_add_all_algorithms();
  detail::Bio mem(BIO_s_mem());
  int ret = i2d_PKCS8PrivateKey_bio(mem.get(), m_impl->key, EVP_des_cbc(),
                                    const_cast<char*>(pw), pwLen, nullptr, nullptr);
  if (ret != 1)
    BOOST_THROW_EXCEPTION(Error("Cannot convert key into PKCS8 format"));

  int len8 = BIO_pending(mem.get());
  auto buffer = make_shared<Buffer>(len8);
  BIO_read(mem.get(), buffer->buf(), len8);

  return buffer;
}

ConstBufferPtr
PrivateKey::toPkcs8(PasswordCallback pwCallback) const
{
  ENSURE_PRIVATE_KEY_LOADED(m_impl->key);

  OpenSSL_add_all_algorithms();
  detail::Bio mem(BIO_s_mem());
  int ret = i2d_PKCS8PrivateKey_bio(mem.get(), m_impl->key, EVP_des_cbc(),
                                    nullptr, 0,
                                    passwordCallback, &pwCallback);
  if (ret != 1)
    BOOST_THROW_EXCEPTION(Error("Cannot convert key into PKCS8 format"));

  int len8 = BIO_pending(mem.get());
  auto buffer = make_shared<Buffer>(len8);
  BIO_read(mem.get(), buffer->buf(), len8);

  return buffer;
}

ConstBufferPtr
PrivateKey::rsaDecrypt(const uint8_t* cipherText, size_t cipherLen) const
{
  detail::EvpPkeyCtx ctx(m_impl->key);

  if (EVP_PKEY_decrypt_init(ctx.get()) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to initialize decryption context"));

  if (EVP_PKEY_CTX_set_rsa_padding(ctx.get(), RSA_PKCS1_OAEP_PADDING) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to set padding"));

  size_t outlen = 0;
  // Determine buffer length
  if (EVP_PKEY_decrypt(ctx.get(), nullptr, &outlen, cipherText, cipherLen) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to estimate output length"));

  auto out = make_shared<Buffer>(outlen);

  if (EVP_PKEY_decrypt(ctx.get(), out->buf(), &outlen, cipherText, cipherLen) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to decrypt cipher text"));

  out->resize(outlen);
  return out;
}

static unique_ptr<PrivateKey>
generateRsaKey(uint32_t keySize)
{
  detail::EvpPkeyCtx kctx(EVP_PKEY_RSA);

  int ret = EVP_PKEY_keygen_init(kctx.get());
  if (ret != 1)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Fail to generate RSA key"));

  ret = EVP_PKEY_CTX_set_rsa_keygen_bits(kctx.get(), keySize);
  if (ret != 1)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Fail to generate RSA key"));

  detail::EvpPkey key;
  ret = EVP_PKEY_keygen(kctx.get(), &key);
  if (ret != 1)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Fail to generate RSA key"));

  detail::Bio mem(BIO_s_mem());
  i2d_PrivateKey_bio(mem.get(), key.get());
  int len = BIO_pending(mem.get());
  Buffer buffer(len);
  BIO_read(mem.get(), buffer.buf(), len);

  auto privateKey = make_unique<PrivateKey>();
  privateKey->loadPkcs1(buffer.buf(), buffer.size());

  return privateKey;
}

static unique_ptr<PrivateKey>
generateEcKey(uint32_t keySize)
{
  detail::EvpPkeyCtx ctx(EVP_PKEY_EC);

  int ret = EVP_PKEY_paramgen_init(ctx.get());
  if (ret != 1)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Fail to generate EC key"));

  switch (keySize) {
    case 256:
      ret = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx.get(), NID_X9_62_prime256v1);
      break;
    case 384:
      ret = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx.get(), NID_secp384r1);
      break;
    default:
      BOOST_THROW_EXCEPTION(PrivateKey::Error("Fail to generate EC key"));
  }
  if (ret != 1)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Fail to generate EC key"));

  detail::EvpPkey params;
  ret = EVP_PKEY_paramgen(ctx.get(), &params);
  if (ret != 1)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Fail to generate EC key"));

  detail::EvpPkeyCtx kctx(params.get());
  ret = EVP_PKEY_keygen_init(kctx.get());
  if (ret != 1)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Fail to generate EC key"));

  detail::EvpPkey key;
  ret = EVP_PKEY_keygen(kctx.get(), &key);
  if (ret != 1)
    BOOST_THROW_EXCEPTION(PrivateKey::Error("Fail to generate EC key"));

  detail::Bio mem(BIO_s_mem());
  i2d_PrivateKey_bio(mem.get(), key.get());
  int len = BIO_pending(mem.get());
  Buffer buffer(len);
  BIO_read(mem.get(), buffer.buf(), len);

  auto privateKey = make_unique<PrivateKey>();
  privateKey->loadPkcs1(buffer.buf(), buffer.size());

  return privateKey;
}

unique_ptr<PrivateKey>
generatePrivateKey(const KeyParams& keyParams)
{
  switch (keyParams.getKeyType()) {
    case KeyType::RSA: {
      const RsaKeyParams& rsaParams = static_cast<const RsaKeyParams&>(keyParams);
      return generateRsaKey(rsaParams.getKeySize());
    }
    case KeyType::EC: {
      const EcdsaKeyParams& ecdsaParams = static_cast<const EcdsaKeyParams&>(keyParams);
      return generateEcKey(ecdsaParams.getKeySize());
    }
    default:
      BOOST_THROW_EXCEPTION(std::invalid_argument("Unsupported asymmetric key type"));
  }
}

} // namespace transform
} // namespace security
} // namespace ndn

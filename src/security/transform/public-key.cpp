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

#include "public-key.hpp"
#include "buffer-source.hpp"
#include "stream-source.hpp"
#include "base64-encode.hpp"
#include "base64-decode.hpp"
#include "stream-sink.hpp"
#include "../../encoding/buffer-stream.hpp"
#include "../detail/openssl-helper.hpp"

#define ENSURE_PUBLIC_KEY_LOADED(key) \
  do { \
    if (key == nullptr) \
      BOOST_THROW_EXCEPTION(Error("Public key has not been loaded yet")); \
  } while (false)

namespace ndn {
namespace security {
namespace transform {

class PublicKey::Impl
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

PublicKey::PublicKey()
  : m_impl(new Impl)
{
}

PublicKey::~PublicKey() = default;

KeyType
PublicKey::getKeyType() const
{
  ENSURE_PUBLIC_KEY_LOADED(m_impl->key);

#if OPENSSL_VERSION_NUMBER < 0x1010000fL
  switch (EVP_PKEY_type(m_impl->key->type)) {
#else
  switch (EVP_PKEY_base_id(m_impl->key)) {
#endif // OPENSSL_VERSION_NUMBER < 0x1010000fL
  case EVP_PKEY_RSA:
    return KeyType::RSA;
  case EVP_PKEY_EC:
    return KeyType::EC;
  default:
    BOOST_THROW_EXCEPTION(Error("Public key type is not recognized"));
  }
}

void
PublicKey::loadPkcs8(const uint8_t* buf, size_t size)
{
  m_impl->key = d2i_PUBKEY(nullptr, &buf, size);

  ENSURE_PUBLIC_KEY_LOADED(m_impl->key);
}

void
PublicKey::loadPkcs8(std::istream& is)
{
  OBufferStream os;
  {
    using namespace transform;
    streamSource(is) >> streamSink(os);
  }
  this->loadPkcs8(os.buf()->buf(), os.buf()->size());
}

void
PublicKey::loadPkcs8Base64(const uint8_t* buf, size_t size)
{
  OBufferStream os;
  {
    using namespace transform;
    bufferSource(buf, size) >> base64Decode() >> streamSink(os);
  }
  this->loadPkcs8(os.buf()->buf(), os.buf()->size());
}

void
PublicKey::loadPkcs8Base64(std::istream& is)
{
  OBufferStream os;
  {
    using namespace transform;
    streamSource(is) >> base64Decode() >> streamSink(os);
  }
  this->loadPkcs8(os.buf()->buf(), os.buf()->size());
}

void
PublicKey::savePkcs8(std::ostream& os) const
{
  using namespace transform;
  bufferSource(*this->toPkcs8()) >> streamSink(os);
}

void
PublicKey::savePkcs8Base64(std::ostream& os) const
{
  using namespace transform;
  bufferSource(*this->toPkcs8()) >> base64Encode() >> streamSink(os);
}

ConstBufferPtr
PublicKey::encrypt(const uint8_t* plainText, size_t plainLen) const
{
  ENSURE_PUBLIC_KEY_LOADED(m_impl->key);

#if OPENSSL_VERSION_NUMBER < 0x1010000fL
  switch (EVP_PKEY_type(m_impl->key->type)) {
#else
  switch (EVP_PKEY_base_id(m_impl->key)) {
#endif // OPENSSL_VERSION_NUMBER < 0x1010000fL
  case EVP_PKEY_RSA:
    return rsaEncrypt(plainText, plainLen);
  default:
    BOOST_THROW_EXCEPTION(Error("Encryption is not supported for this key type"));
  }
}

void*
PublicKey::getEvpPkey() const
{
  return m_impl->key;
}

ConstBufferPtr
PublicKey::toPkcs8() const
{
  ENSURE_PUBLIC_KEY_LOADED(m_impl->key);

  uint8_t* pkcs8 = nullptr;
  int len = i2d_PUBKEY(m_impl->key, &pkcs8);

  if (pkcs8 == nullptr)
    BOOST_THROW_EXCEPTION(Error("Failed to convert to pkcs8 format"));

  auto buffer = make_shared<Buffer>(pkcs8, len);
  OPENSSL_free(pkcs8);

  return buffer;
}

ConstBufferPtr
PublicKey::rsaEncrypt(const uint8_t* plainText, size_t plainLen) const
{
  detail::EvpPkeyCtx ctx(m_impl->key);

  if (EVP_PKEY_encrypt_init(ctx.get()) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to initialize encryption context"));

  if (EVP_PKEY_CTX_set_rsa_padding(ctx.get(), RSA_PKCS1_OAEP_PADDING) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to set padding"));

  size_t outlen = 0;
  // Determine buffer length
  if (EVP_PKEY_encrypt(ctx.get(), nullptr, &outlen, plainText, plainLen) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to estimate output length"));

  auto out = make_shared<Buffer>(outlen);

  if (EVP_PKEY_encrypt(ctx.get(), out->buf(), &outlen, plainText, plainLen) <= 0)
    BOOST_THROW_EXCEPTION(Error("Failed to decrypt cipher text"));

  out->resize(outlen);
  return out;
}

} // namespace transform
} // namespace security
} // namespace ndn

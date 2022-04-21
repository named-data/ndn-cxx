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

#include "ndn-cxx/security/transform/block-cipher.hpp"
#include "ndn-cxx/security/impl/openssl.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace transform {

class BlockCipher::Impl
{
public:
  Impl() noexcept
    : m_cipher(BIO_new(BIO_f_cipher()))
    , m_sink(BIO_new(BIO_s_mem()))
  {
    BIO_push(m_cipher, m_sink);
  }

  ~Impl()
  {
    BIO_free_all(m_cipher);
  }

public:
  BIO* m_cipher;
  BIO* m_sink; // BIO_f_cipher alone does not work without a sink
};


BlockCipher::BlockCipher(BlockCipherAlgorithm algo, CipherOperator op,
                         const uint8_t* key, size_t keyLen,
                         const uint8_t* iv, size_t ivLen)
  : m_impl(make_unique<Impl>())
{

//added_GM, by liupenghui
#if 1
	  EVP_CIPHER_CTX *ctx = nullptr;
#endif

  switch (algo) {
  case BlockCipherAlgorithm::AES_CBC:
    initializeAesCbc(key, keyLen, iv, ivLen, op);
    break;
//added_GM, by liupenghui
#if 1
    case BlockCipherAlgorithm::AES_EBC:
   	initializeAesEbc(key, keyLen, iv, ivLen, op);
   	BIO_get_cipher_ctx(m_impl->m_cipher, &ctx);
   	// if using padding for ECB mode with ver 1.1.1f, the decryption of openssl will raise an error.
   	// the user must ensure the length of input data is a mutilple of 16.
   	EVP_CIPHER_CTX_set_padding(ctx, 0);
  	break;
    case BlockCipherAlgorithm::AES_CFB:
  	initializeAesCfb(key, keyLen, iv, ivLen, op);
  	break;
    case BlockCipherAlgorithm::AES_OFB:
  	initializeAesOfb(key, keyLen, iv, ivLen, op);
  	break;
    case BlockCipherAlgorithm::SM4_CBC:
  	initializeSm4Cbc(key, keyLen, iv, ivLen, op);
  	break;
    case BlockCipherAlgorithm::SM4_EBC:
  	initializeSm4Ebc(key, keyLen, iv, ivLen, op);
  	BIO_get_cipher_ctx(m_impl->m_cipher, &ctx);
  	// if using padding for SM4 ECB mode with ver 1.1.1f, the decryption of openssl will raise an error.
  	// the user must ensure the length of input data is a mutilple of 16.
  	EVP_CIPHER_CTX_set_padding(ctx, 0);
  	break;
    case BlockCipherAlgorithm::SM4_CFB:
  	initializeSm4Cfb(key, keyLen, iv, ivLen, op);
  	break;
    case BlockCipherAlgorithm::SM4_OFB:
  	initializeSm4Ofb(key, keyLen, iv, ivLen, op);
  	break;
#endif
  default:
    NDN_THROW(Error(getIndex(), "Unsupported block cipher algorithm " +
                    boost::lexical_cast<std::string>(algo)));
  }
}

BlockCipher::~BlockCipher() = default;

void
BlockCipher::preTransform()
{
  fillOutputBuffer();
}

size_t
BlockCipher::convert(const uint8_t* data, size_t dataLen)
{
  if (dataLen == 0)
    return 0;

  int wLen = BIO_write(m_impl->m_cipher, data, dataLen);

  if (wLen <= 0) { // failed to write data
    if (!BIO_should_retry(m_impl->m_cipher)) {
      // we haven't written everything but some error happens, and we cannot retry
      NDN_THROW(Error(getIndex(), "Failed to accept more input"));
    }
    return 0;
  }
  else { // update number of bytes written
    fillOutputBuffer();
    return static_cast<size_t>(wLen);
  }
}

void
BlockCipher::finalize()
{
  if (BIO_flush(m_impl->m_cipher) != 1)
    NDN_THROW(Error(getIndex(), "Failed to flush"));

  while (!isConverterEmpty()) {
    fillOutputBuffer();
    while (!isOutputBufferEmpty()) {
      flushOutputBuffer();
    }
  }
}

void
BlockCipher::fillOutputBuffer()
{
  int nPending = BIO_pending(m_impl->m_sink);
  if (nPending <= 0)
    return;

  // there is something to read from BIO
  auto buffer = make_unique<OBuffer>(nPending);
  int nRead = BIO_read(m_impl->m_sink, buffer->data(), nPending);
  if (nRead < 0)
    return;

  buffer->erase(buffer->begin() + nRead, buffer->end());
  setOutputBuffer(std::move(buffer));
}

bool
BlockCipher::isConverterEmpty() const
{
  return BIO_pending(m_impl->m_sink) <= 0;
}

void
BlockCipher::initializeAesCbc(const uint8_t* key, size_t keyLen,
                              const uint8_t* iv, size_t ivLen, CipherOperator op)
{
  const EVP_CIPHER* cipherType = nullptr;
  switch (keyLen) {
  case 16:
    cipherType = EVP_aes_128_cbc();
    break;
  case 24:
    cipherType = EVP_aes_192_cbc();
    break;
  case 32:
    cipherType = EVP_aes_256_cbc();
    break;
  default:
    NDN_THROW(Error(getIndex(), "Unsupported key length " + to_string(keyLen)));
  }

  size_t requiredIvLen = static_cast<size_t>(EVP_CIPHER_iv_length(cipherType));
  if (ivLen != requiredIvLen)
    NDN_THROW(Error(getIndex(), "IV length must be " + to_string(requiredIvLen)));

  BIO_set_cipher(m_impl->m_cipher, cipherType, key, iv, op == CipherOperator::ENCRYPT ? 1 : 0);
}

//added_GM, by liupenghui
//SM4 only support 128bits block size, is enough to security apps.
#if 1
void
BlockCipher::initializeAesEbc(const uint8_t* key, size_t keyLen,
                              const uint8_t* iv, size_t ivLen, CipherOperator op)
{
  const EVP_CIPHER* cipherType = nullptr;
  switch (keyLen) {
  case 16:
    cipherType = EVP_aes_128_ecb();
    break;
  case 24:
    cipherType = EVP_aes_192_ecb();
    break;
  case 32:
    cipherType = EVP_aes_256_ecb();
    break;
  default:
    NDN_THROW(Error(getIndex(), "Unsupported key length " + to_string(keyLen)));
  }

  BIO_set_cipher(m_impl->m_cipher, cipherType, key, iv,
                 op == CipherOperator::ENCRYPT ? 1 : 0);
}

void
BlockCipher::initializeAesCfb(const uint8_t* key, size_t keyLen,
                              const uint8_t* iv, size_t ivLen, CipherOperator op)
{
  const EVP_CIPHER* cipherType = nullptr;
  switch (keyLen) {
  case 16:
    cipherType = EVP_aes_128_cfb();
    break;
  case 24:
    cipherType = EVP_aes_192_cfb();
    break;
  case 32:
    cipherType = EVP_aes_256_cfb();
    break;
  default:
    NDN_THROW(Error(getIndex(), "Unsupported key length " + to_string(keyLen)));
  }
  auto requiredIvLen = static_cast<size_t>(EVP_CIPHER_iv_length(cipherType));
  if (ivLen != requiredIvLen)
    NDN_THROW(Error(getIndex(), "SM4 IV length must be " + to_string(keyLen)));

  BIO_set_cipher(m_impl->m_cipher, cipherType, key, iv,
                 op == CipherOperator::ENCRYPT ? 1 : 0);
  
  return;
}

void
BlockCipher::initializeAesOfb(const uint8_t* key, size_t keyLen,
                              const uint8_t* iv, size_t ivLen, CipherOperator op)
{
  const EVP_CIPHER* cipherType = nullptr;
  switch (keyLen) {
  case 16:
    cipherType = EVP_aes_128_ofb();
    break;
  case 24:
    cipherType = EVP_aes_192_ofb();
    break;
  case 32:
    cipherType = EVP_aes_256_ofb();
    break;
  default:
    NDN_THROW(Error(getIndex(), "Unsupported key length " + to_string(keyLen)));
  }

  auto requiredIvLen = static_cast<size_t>(EVP_CIPHER_iv_length(cipherType));
  if (ivLen != requiredIvLen)
    NDN_THROW(Error(getIndex(), "SM4 IV length must be " + to_string(requiredIvLen)));

  BIO_set_cipher(m_impl->m_cipher, cipherType, key, iv,
                 op == CipherOperator::ENCRYPT ? 1 : 0);
  
  return;
}


void
BlockCipher::initializeSm4Cbc(const uint8_t* key, size_t keyLen,
                              const uint8_t* iv, size_t ivLen, CipherOperator op)
{
  const EVP_CIPHER* cipherType = nullptr;
  switch (keyLen) {
  case 16:
    cipherType = EVP_sm4_cbc();
    break;
  default:
    NDN_THROW(Error(getIndex(), "SM4 Unsupported key length " + to_string(keyLen)));
  }

  auto requiredIvLen = static_cast<size_t>(EVP_CIPHER_iv_length(cipherType));
  if (ivLen != requiredIvLen)
    NDN_THROW(Error(getIndex(), "SM4 IV length must be " + to_string(requiredIvLen)));

  BIO_set_cipher(m_impl->m_cipher, cipherType, key, iv,
                 op == CipherOperator::ENCRYPT ? 1 : 0);
  
  return;
}

void
BlockCipher::initializeSm4Ebc(const uint8_t* key, size_t keyLen,
                              const uint8_t* iv, size_t ivLen, CipherOperator op)
{
  const EVP_CIPHER* cipherType = nullptr;
  switch (keyLen) {
  case 16:
    cipherType = EVP_sm4_ecb();
    break;
  default:
    NDN_THROW(Error(getIndex(), "SM4 Unsupported key length " + to_string(keyLen)));
  }

  BIO_set_cipher(m_impl->m_cipher, cipherType, key, iv,
                 op == CipherOperator::ENCRYPT ? 1 : 0);
  
  return;
}

void
BlockCipher::initializeSm4Cfb(const uint8_t* key, size_t keyLen,
                              const uint8_t* iv, size_t ivLen, CipherOperator op)
{
  const EVP_CIPHER* cipherType = nullptr;
  switch (keyLen) {
  case 16:
    cipherType = EVP_sm4_cfb();
    break;
  default:
    NDN_THROW(Error(getIndex(), "SM4 Unsupported key length " + to_string(keyLen)));
  }

  auto requiredIvLen = static_cast<size_t>(EVP_CIPHER_iv_length(cipherType));
  if (ivLen != requiredIvLen)
    NDN_THROW(Error(getIndex(), "SM4 IV length must be " + to_string(requiredIvLen)));

  BIO_set_cipher(m_impl->m_cipher, cipherType, key, iv,
                 op == CipherOperator::ENCRYPT ? 1 : 0);
  
  return;
}

void
BlockCipher::initializeSm4Ofb(const uint8_t* key, size_t keyLen,
                              const uint8_t* iv, size_t ivLen, CipherOperator op)
{
  const EVP_CIPHER* cipherType = nullptr;
  switch (keyLen) {
  case 16:
    cipherType = EVP_sm4_ofb();
    break;
  default:
    NDN_THROW(Error(getIndex(), "SM4 Unsupported key length " + to_string(keyLen)));
  }

  auto requiredIvLen = static_cast<size_t>(EVP_CIPHER_iv_length(cipherType));
  if (ivLen != requiredIvLen)
    NDN_THROW(Error(getIndex(), "SM4 IV length must be " + to_string(requiredIvLen)));

  BIO_set_cipher(m_impl->m_cipher, cipherType, key, iv,
                 op == CipherOperator::ENCRYPT ? 1 : 0);
  
  return;
}

#endif

unique_ptr<Transform>
blockCipher(BlockCipherAlgorithm algo, CipherOperator op,
            const uint8_t* key, size_t keyLen,
            const uint8_t* iv, size_t ivLen)
{
  return make_unique<BlockCipher>(algo, op, key, keyLen, iv, ivLen);
}

} // namespace transform
} // namespace security
} // namespace ndn


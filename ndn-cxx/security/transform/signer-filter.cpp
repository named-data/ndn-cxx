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

#include "ndn-cxx/security/transform/signer-filter.hpp"
#include "ndn-cxx/security/transform/private-key.hpp"
#include "ndn-cxx/security/impl/openssl-helper.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace transform {

class SignerFilter::Impl
{
public:
  detail::EvpMdCtx ctx;
};

//added_GM, by liupenghui
// After loading Pkcs8 key from outside file, key.getKeyType() can't differ SM2 from ECDSA,
#if 1
SignerFilter::SignerFilter(DigestAlgorithm algo, const PrivateKey& key, KeyType keyType)
  : m_impl(make_unique<Impl>())
{
  m_keyType = key.getKeyType();
  if (KeyType::NONE != keyType) {
  	  m_keyType = keyType;
  }

  const EVP_MD* md = detail::digestAlgorithmToEvpMd(algo);
  if (md == nullptr)
    NDN_THROW(Error(getIndex(), "Unsupported digest algorithm " +
                    boost::lexical_cast<std::string>(algo)));

  if (m_keyType ==  KeyType::SM2) {
    EVP_PKEY_CTX *sctx = nullptr;  	
	/* The default user id as specified in GM/T 0009-2012 */
    const char *sm2_id = "1234567812345678";
	  
    if ((EVP_PKEY_set_alias_type(reinterpret_cast<EVP_PKEY*>(key.getEvpPkey()), EVP_PKEY_SM2)) != 1) {
      NDN_THROW(Error(getIndex(), "Failed to EVP_PKEY_set_alias_type"));
    }
	
	if (!(sctx = EVP_PKEY_CTX_new(reinterpret_cast<EVP_PKEY*>(key.getEvpPkey()), nullptr))) {
      NDN_THROW(Error(getIndex(), "Failed to EVP_PKEY_CTX_new"));
	}
 
	if (EVP_PKEY_CTX_set1_id(sctx, (const uint8_t *)sm2_id, strlen(sm2_id)) <= 0) {
		EVP_PKEY_CTX_free(sctx);
		NDN_THROW(Error(getIndex(), "Failed to EVP_PKEY_CTX_set1_id"));
	}
 
	EVP_MD_CTX_set_pkey_ctx(m_impl->ctx, sctx);
  }
  	
  if (EVP_DigestSignInit(m_impl->ctx, nullptr, md, nullptr,
                         reinterpret_cast<EVP_PKEY*>(key.getEvpPkey())) != 1)
    NDN_THROW(Error(getIndex(), "Failed to initialize signing context with " +
                    boost::lexical_cast<std::string>(algo) + " digest and " +
                    boost::lexical_cast<std::string>(key.getKeyType()) + " key"));
}

#else
SignerFilter::SignerFilter(DigestAlgorithm algo, const PrivateKey& key)
  : m_impl(make_unique<Impl>())
{
  const EVP_MD* md = detail::digestAlgorithmToEvpMd(algo);
  if (md == nullptr)
    NDN_THROW(Error(getIndex(), "Unsupported digest algorithm " +
                    boost::lexical_cast<std::string>(algo)));

  if (EVP_DigestSignInit(m_impl->ctx, nullptr, md, nullptr,
                         reinterpret_cast<EVP_PKEY*>(key.getEvpPkey())) != 1)
    NDN_THROW(Error(getIndex(), "Failed to initialize signing context with " +
                    boost::lexical_cast<std::string>(algo) + " digest and " +
                    boost::lexical_cast<std::string>(key.getKeyType()) + " key"));
}

#endif


SignerFilter::~SignerFilter() = default;

size_t
SignerFilter::convert(const uint8_t* buf, size_t size)
{
  if (EVP_DigestSignUpdate(m_impl->ctx, buf, size) != 1)
    NDN_THROW(Error(getIndex(), "Failed to accept more input"));

  return size;
}

void
SignerFilter::finalize()
{
  size_t sigLen = 0;
  if (EVP_DigestSignFinal(m_impl->ctx, nullptr, &sigLen) != 1)
    NDN_THROW(Error(getIndex(), "Failed to estimate buffer length"));

  auto buffer = make_unique<OBuffer>(sigLen);
  if (EVP_DigestSignFinal(m_impl->ctx, buffer->data(), &sigLen) != 1)
    NDN_THROW(Error(getIndex(), "Failed to finalize signature"));
//added_GM, by liupenghui
#if 1
  if ((EVP_MD_CTX_pkey_ctx(m_impl->ctx) != nullptr) && (m_keyType == KeyType::SM2)) {
    EVP_PKEY_CTX_free(EVP_MD_CTX_pkey_ctx(m_impl->ctx));
  }
#endif

  buffer->erase(buffer->begin() + sigLen, buffer->end());
  setOutputBuffer(std::move(buffer));

  flushAllOutput();
}

//added_GM, by liupenghui
//SM2 Signer must use SM3, force to use SM3.
// After loading Pkcs8 key from outside file, key.getKeyType() can't differ SM2 from ECDSA,
#if 1
unique_ptr<Transform>
signerFilter(DigestAlgorithm algo, const PrivateKey& key, KeyType keyType)
{
  return make_unique<SignerFilter>(algo, key, keyType);
}
#else
unique_ptr<Transform>
signerFilter(DigestAlgorithm algo, const PrivateKey& key)
{
  return make_unique<SignerFilter>(algo, key);
}
#endif


} // namespace transform
} // namespace security
} // namespace ndn


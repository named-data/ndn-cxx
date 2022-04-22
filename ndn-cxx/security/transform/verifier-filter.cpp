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

#include "ndn-cxx/security/transform/verifier-filter.hpp"
#include "ndn-cxx/security/transform/private-key.hpp"
#include "ndn-cxx/security/transform/public-key.hpp"
#include "ndn-cxx/security/impl/openssl-helper.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace transform {

class VerifierFilter::Impl
{
public:
  explicit
  Impl(span<const uint8_t> sig)
    : sig(sig)
  {
  }

public:
  detail::EvpMdCtx ctx;
  span<const uint8_t> sig;
};

//added_GM, by liupenghui
//the PublicKey.getKeyType() can't differ the SM2-type key from ECDSA key, we add a common paramter Type to initiate the Key.
#if 1
VerifierFilter::VerifierFilter(DigestAlgorithm algo, const PublicKey& key, KeyType keyType, span<const uint8_t> sig)
  : m_impl(make_unique<Impl>(sig))
  , m_keyType(keyType)
{
  init(algo, key.getEvpPkey());
}

VerifierFilter::VerifierFilter(DigestAlgorithm algo, const PrivateKey& key, KeyType keyType, span<const uint8_t> sig)
  : m_impl(make_unique<Impl>(sig))
  , m_keyType(keyType)
{
  if (m_keyType != KeyType::HMAC)
    NDN_THROW(Error(getIndex(), "VerifierFilter only supports private keys of HMAC type"));

  init(algo, key.getEvpPkey());
}
#else
VerifierFilter::VerifierFilter(DigestAlgorithm algo, const PublicKey& key, span<const uint8_t> sig)
  : m_impl(make_unique<Impl>(sig))
  , m_keyType(key.getKeyType())
{
  init(algo, key.getEvpPkey());
}

VerifierFilter::VerifierFilter(DigestAlgorithm algo, const PrivateKey& key, span<const uint8_t> sig)
  : m_impl(make_unique<Impl>(sig))
  , m_keyType(key.getKeyType())
{
  if (m_keyType != KeyType::HMAC)
    NDN_THROW(Error(getIndex(), "VerifierFilter only supports private keys of HMAC type"));

  init(algo, key.getEvpPkey());
}
#endif


VerifierFilter::~VerifierFilter() = default;

void
VerifierFilter::init(DigestAlgorithm algo, void* pkey)
{
  const EVP_MD* md = detail::digestAlgorithmToEvpMd(algo);
  if (md == nullptr)
    NDN_THROW(Error(getIndex(), "Unsupported digest algorithm " +
                    boost::lexical_cast<std::string>(algo)));
//added_GM, by liupenghui
#if 1
if (m_keyType == KeyType::SM2) {
  EVP_PKEY_CTX *sctx = nullptr; 	
  /* The default user id as specified in GM/T 0009-2012 */
  const char *sm2_id = "1234567812345678";
	  
  if ((EVP_PKEY_set_alias_type(reinterpret_cast<EVP_PKEY*>(pkey), EVP_PKEY_SM2)) != 1) {
	NDN_THROW(Error(getIndex(), "Failed to EVP_PKEY_set_alias_type"));
  }
	
  if (!(sctx = EVP_PKEY_CTX_new(reinterpret_cast<EVP_PKEY*>(pkey), nullptr))) {
	NDN_THROW(Error(getIndex(), "Failed to EVP_PKEY_CTX_new"));
  }
 
  if (EVP_PKEY_CTX_set1_id(sctx, (const uint8_t *)sm2_id, strlen(sm2_id)) <= 0) {
	 EVP_PKEY_CTX_free(sctx);
	 NDN_THROW(Error(getIndex(), "Failed to EVP_PKEY_CTX_set1_id"));
  }
 
  EVP_MD_CTX_set_pkey_ctx(m_impl->ctx, sctx);
}
	
#endif

  int ret;
  if (m_keyType == KeyType::HMAC)
    ret = EVP_DigestSignInit(m_impl->ctx, nullptr, md, nullptr, reinterpret_cast<EVP_PKEY*>(pkey));
  else
    ret = EVP_DigestVerifyInit(m_impl->ctx, nullptr, md, nullptr, reinterpret_cast<EVP_PKEY*>(pkey));

  if (ret != 1)
    NDN_THROW(Error(getIndex(), "Failed to initialize verification context with " +
                    boost::lexical_cast<std::string>(algo) + " digest and " +
                    boost::lexical_cast<std::string>(m_keyType) + " key"));
}

size_t
VerifierFilter::convert(span<const uint8_t> buf)
{
  int ret;
  if (m_keyType == KeyType::HMAC)
    ret = EVP_DigestSignUpdate(m_impl->ctx, buf.data(), buf.size());
  else
    ret = EVP_DigestVerifyUpdate(m_impl->ctx, buf.data(), buf.size());

  if (ret != 1)
    NDN_THROW(Error(getIndex(), "Failed to accept more input"));

  return buf.size();
}

void
VerifierFilter::finalize()
{
  bool ok = false;
  if (m_keyType == KeyType::HMAC) {
    auto hmacBuf = make_unique<OBuffer>(EVP_MAX_MD_SIZE);
    size_t hmacLen = 0;

    if (EVP_DigestSignFinal(m_impl->ctx, hmacBuf->data(), &hmacLen) != 1)
      NDN_THROW(Error(getIndex(), "Failed to finalize HMAC"));

    ok = CRYPTO_memcmp(hmacBuf->data(), m_impl->sig.data(), std::min(hmacLen, m_impl->sig.size())) == 0;
  }
  else {
    ok = EVP_DigestVerifyFinal(m_impl->ctx, m_impl->sig.data(), m_impl->sig.size()) == 1;
  }

//added_GM, by liupenghui
#if 1
  if ((EVP_MD_CTX_pkey_ctx(m_impl->ctx) != nullptr) && (m_keyType == KeyType::SM2)) {
    EVP_PKEY_CTX_free(EVP_MD_CTX_pkey_ctx(m_impl->ctx));
  }
#endif

  auto buffer = make_unique<OBuffer>(1);
  (*buffer)[0] = ok ? 1 : 0;
  setOutputBuffer(std::move(buffer));

  flushAllOutput();
}

//added_GM, by liupenghui
//the PublicKey.getKeyType() can't differ the SM2-type key from ECDSA key, we add a common paramter Type to initiate the Key.
#if 1
unique_ptr<Transform>
verifierFilter(DigestAlgorithm algo, const PublicKey& key, KeyType keyType, span<const uint8_t> sig)
{
  return make_unique<VerifierFilter>(algo, key, keyType, sig);
}

unique_ptr<Transform>
verifierFilter(DigestAlgorithm algo, const PrivateKey& key, KeyType keyType, span<const uint8_t> sig)
{
  return make_unique<VerifierFilter>(algo, key, keyType, sig);
}
#else
unique_ptr<Transform>
verifierFilter(DigestAlgorithm algo, const PublicKey& key, span<const uint8_t> sig)
{
  return make_unique<VerifierFilter>(algo, key, sig);
}

unique_ptr<Transform>
verifierFilter(DigestAlgorithm algo, const PrivateKey& key, span<const uint8_t> sig)
{
  return make_unique<VerifierFilter>(algo, key, sig);
}
#endif


} // namespace transform
} // namespace security
} // namespace ndn

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
  Impl(const uint8_t* sig, size_t siglen)
    : sig(sig)
    , siglen(siglen)
  {
  }

public:
  detail::EvpMdCtx ctx;
  const uint8_t* sig;
  size_t siglen;
};


VerifierFilter::VerifierFilter(DigestAlgorithm algo, const PublicKey& key,
                               const uint8_t* sig, size_t sigLen)
  : m_impl(make_unique<Impl>(sig, sigLen))
  , m_keyType(key.getKeyType())
{
  init(algo, key.getEvpPkey());
}

VerifierFilter::VerifierFilter(DigestAlgorithm algo, const PrivateKey& key,
                               const uint8_t* sig, size_t sigLen)
  : m_impl(make_unique<Impl>(sig, sigLen))
  , m_keyType(key.getKeyType())
{
  if (m_keyType != KeyType::HMAC)
    NDN_THROW(Error(getIndex(), "VerifierFilter only supports private keys of HMAC type"));

  init(algo, key.getEvpPkey());
}

VerifierFilter::~VerifierFilter() = default;

void
VerifierFilter::init(DigestAlgorithm algo, void* pkey)
{
  const EVP_MD* md = detail::digestAlgorithmToEvpMd(algo);
  if (md == nullptr)
    NDN_THROW(Error(getIndex(), "Unsupported digest algorithm " +
                    boost::lexical_cast<std::string>(algo)));

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
VerifierFilter::convert(const uint8_t* buf, size_t size)
{
  int ret;
  if (m_keyType == KeyType::HMAC)
    ret = EVP_DigestSignUpdate(m_impl->ctx, buf, size);
  else
    ret = EVP_DigestVerifyUpdate(m_impl->ctx, buf, size);

  if (ret != 1)
    NDN_THROW(Error(getIndex(), "Failed to accept more input"));

  return size;
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

    ok = CRYPTO_memcmp(hmacBuf->data(), m_impl->sig, std::min(hmacLen, m_impl->siglen)) == 0;
  }
  else {
    ok = EVP_DigestVerifyFinal(m_impl->ctx, m_impl->sig, m_impl->siglen) == 1;
  }

  auto buffer = make_unique<OBuffer>(1);
  (*buffer)[0] = ok ? 1 : 0;
  setOutputBuffer(std::move(buffer));

  flushAllOutput();
}

unique_ptr<Transform>
verifierFilter(DigestAlgorithm algo, const PublicKey& key, const uint8_t* sig, size_t sigLen)
{
  return make_unique<VerifierFilter>(algo, key, sig, sigLen);
}

unique_ptr<Transform>
verifierFilter(DigestAlgorithm algo, const PrivateKey& key, const uint8_t* sig, size_t sigLen)
{
  return make_unique<VerifierFilter>(algo, key, sig, sigLen);
}

} // namespace transform
} // namespace security
} // namespace ndn

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

#include "openssl-helper.hpp"

namespace ndn {
namespace security {
namespace detail {

const EVP_MD*
toDigestEvpMd(DigestAlgorithm algo)
{
  switch (algo) {
  case DigestAlgorithm::SHA256:
    return EVP_sha256();
  default:
    return nullptr;
  }
}

EvpPkey::EvpPkey()
  : m_key(nullptr)
{
}

EvpPkey::~EvpPkey()
{
  EVP_PKEY_free(m_key);
}

EvpPkeyCtx::EvpPkeyCtx(EVP_PKEY* key)
  : m_ctx(EVP_PKEY_CTX_new(key, nullptr))
{
  BOOST_ASSERT(m_ctx != nullptr);
}

EvpPkeyCtx::EvpPkeyCtx(int id)
  : m_ctx(EVP_PKEY_CTX_new_id(id, nullptr))
{
  BOOST_ASSERT(m_ctx != nullptr);
}

EvpPkeyCtx::~EvpPkeyCtx()
{
  EVP_PKEY_CTX_free(m_ctx);
}

#if OPENSSL_VERSION_NUMBER < 0x1010000fL
Bio::Bio(BIO_METHOD* method)
#else
Bio::Bio(const BIO_METHOD* method)
#endif // OPENSSL_VERSION_NUMBER < 0x1010000fL
  : m_bio(BIO_new(method))
{
  BOOST_ASSERT(m_bio != nullptr);
}

Bio::~Bio()
{
  BIO_free_all(m_bio);
}

} // namespace detail
} // namespace security
} // namespace ndn

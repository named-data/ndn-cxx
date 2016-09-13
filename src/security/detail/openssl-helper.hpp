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

#ifndef NDN_CXX_SECURITY_DETAIL_OPENSSL_HELPER_HPP
#define NDN_CXX_SECURITY_DETAIL_OPENSSL_HELPER_HPP

#include "../security-common.hpp"
#include "openssl.hpp"

namespace ndn {
namespace security {
namespace detail {

const EVP_MD*
toDigestEvpMd(DigestAlgorithm algo);

class EvpPkey
{
public:
  EvpPkey();

  ~EvpPkey();

  EVP_PKEY*
  get() const
  {
    return m_key;
  }

  EVP_PKEY**
  operator&()
  {
    return &m_key;
  }

private:
  EVP_PKEY* m_key;
};

class EvpPkeyCtx
{
public:
  explicit
  EvpPkeyCtx(EVP_PKEY* key);

  explicit
  EvpPkeyCtx(int id);

  ~EvpPkeyCtx();

  EVP_PKEY_CTX*
  get() const
  {
    return m_ctx;
  }

private:
  EVP_PKEY_CTX* m_ctx;
};

class Bio
{
public:
  explicit
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
  Bio(BIO_METHOD* method);
#else
  Bio(const BIO_METHOD* method);
#endif // OPENSSL_VERSION_NUMBER < 0x1010000fL

  ~Bio();

  BIO*
  get() const
  {
    return m_bio;
  }

private:
  BIO* m_bio;
};

} // namespace detail
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_DETAIL_OPENSSL_HELPER_HPP

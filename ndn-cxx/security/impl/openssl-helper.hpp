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

#ifndef NDN_CXX_SECURITY_IMPL_OPENSSL_HELPER_HPP
#define NDN_CXX_SECURITY_IMPL_OPENSSL_HELPER_HPP

#include "ndn-cxx/security/impl/openssl.hpp"
#include "ndn-cxx/security/security-common.hpp"

namespace ndn {
namespace security {
namespace detail {

NDN_CXX_NODISCARD const EVP_MD*
digestAlgorithmToEvpMd(DigestAlgorithm algo);

NDN_CXX_NODISCARD int
getEvpPkeyType(const EVP_PKEY* key);

class EvpMdCtx : noncopyable
{
public:
  EvpMdCtx();

  ~EvpMdCtx();

  operator EVP_MD_CTX*() const
  {
    return m_ctx;
  }

private:
  EVP_MD_CTX* m_ctx;
};

class EvpPkeyCtx : noncopyable
{
public:
  explicit
  EvpPkeyCtx(EVP_PKEY* key);

  explicit
  EvpPkeyCtx(int id);

  ~EvpPkeyCtx();

  operator EVP_PKEY_CTX*() const
  {
    return m_ctx;
  }

private:
  EVP_PKEY_CTX* m_ctx;
};

class Bio : noncopyable
{
public:
  explicit
  Bio(const BIO_METHOD* method);

  ~Bio();

  operator BIO*() const
  {
    return m_bio;
  }

  NDN_CXX_NODISCARD bool
  read(span<uint8_t> buf) const noexcept;

  NDN_CXX_NODISCARD bool
  write(span<const uint8_t> buf) noexcept;

private:
  BIO* m_bio;
};

} // namespace detail
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_IMPL_OPENSSL_HELPER_HPP

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

#include "hmac-filter.hpp"
#include "../detail/openssl-helper.hpp"

namespace ndn {
namespace security {
namespace transform {

class HmacFilter::Impl
{
public:
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
  Impl()
  {
    HMAC_CTX_init(&m_context);
  }

  ~Impl()
  {
    HMAC_CTX_cleanup(&m_context);
  }

  operator HMAC_CTX*()
  {
    return &m_context;
  }

private:
  HMAC_CTX m_context;
#else
  Impl()
    : m_context(HMAC_CTX_new())
  {
  }

  ~Impl()
  {
    HMAC_CTX_free(m_context);
  }

  operator HMAC_CTX*()
  {
    return m_context;
  }

private:
  HMAC_CTX* m_context;
#endif // OPENSSL_VERSION_NUMBER < 0x1010000fL
};

HmacFilter::HmacFilter(DigestAlgorithm algo, const uint8_t* key, size_t keyLen)
  : m_impl(new Impl)
{
  BOOST_ASSERT(key != nullptr);
  BOOST_ASSERT(keyLen > 0);

  const EVP_MD* algorithm = detail::toDigestEvpMd(algo);
  if (algorithm == nullptr)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Unsupported digest algorithm"));

  if (HMAC_Init_ex(*m_impl, key, keyLen, algorithm, nullptr) == 0)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Cannot initialize HMAC"));
}

size_t
HmacFilter::convert(const uint8_t* buf, size_t size)
{
  if (HMAC_Update(*m_impl, buf, size) == 0)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to update HMAC"));

  return size;
}

void
HmacFilter::finalize()
{
  auto buffer = make_unique<OBuffer>(EVP_MAX_MD_SIZE);
  unsigned int mdLen = 0;

  if (HMAC_Final(*m_impl, &(*buffer)[0], &mdLen) == 0)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to finalize HMAC"));

  buffer->erase(buffer->begin() + mdLen, buffer->end());
  setOutputBuffer(std::move(buffer));

  flushAllOutput();
}

unique_ptr<Transform>
hmacFilter(DigestAlgorithm algo, const uint8_t* key, size_t keyLen)
{
  return make_unique<HmacFilter>(algo, key, keyLen);
}

} // namespace transform
} // namespace security
} // namespace ndn

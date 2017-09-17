/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace transform {

class HmacFilter::Impl
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
  detail::EvpMdCtx ctx;
  EVP_PKEY* key;
};


HmacFilter::HmacFilter(DigestAlgorithm algo, const uint8_t* key, size_t keyLen)
  : m_impl(make_unique<Impl>())
{
  BOOST_ASSERT(key != nullptr);
  BOOST_ASSERT(keyLen > 0);

  const EVP_MD* md = detail::digestAlgorithmToEvpMd(algo);
  if (md == nullptr)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Unsupported digest algorithm " +
                                boost::lexical_cast<std::string>(algo)));

  m_impl->key = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, nullptr, key, static_cast<int>(keyLen));
  if (m_impl->key == nullptr)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to create HMAC key"));

  if (EVP_DigestSignInit(m_impl->ctx, nullptr, md, nullptr, m_impl->key) != 1)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to initialize HMAC context with " +
                                boost::lexical_cast<std::string>(algo) + " digest"));
}

HmacFilter::~HmacFilter() = default;

size_t
HmacFilter::convert(const uint8_t* buf, size_t size)
{
  if (EVP_DigestSignUpdate(m_impl->ctx, buf, size) != 1)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to accept more input"));

  return size;
}

void
HmacFilter::finalize()
{
  auto buffer = make_unique<OBuffer>(EVP_MAX_MD_SIZE);
  size_t hmacLen = 0;

  if (EVP_DigestSignFinal(m_impl->ctx, buffer->data(), &hmacLen) != 1)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to finalize HMAC"));

  buffer->erase(buffer->begin() + hmacLen, buffer->end());
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

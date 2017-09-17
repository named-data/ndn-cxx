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

#include "signer-filter.hpp"
#include "private-key.hpp"
#include "../detail/openssl-helper.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace transform {

class SignerFilter::Impl
{
public:
  detail::EvpMdCtx ctx;
};


SignerFilter::SignerFilter(DigestAlgorithm algo, const PrivateKey& key)
  : m_impl(make_unique<Impl>())
{
  const EVP_MD* md = detail::digestAlgorithmToEvpMd(algo);
  if (md == nullptr)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Unsupported digest algorithm " +
                                boost::lexical_cast<std::string>(algo)));

  if (EVP_DigestSignInit(m_impl->ctx, nullptr, md, nullptr,
                         reinterpret_cast<EVP_PKEY*>(key.getEvpPkey())) != 1)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to initialize signing context with " +
                                boost::lexical_cast<std::string>(algo) + " digest and " +
                                boost::lexical_cast<std::string>(key.getKeyType()) + " key"));
}

SignerFilter::~SignerFilter() = default;

size_t
SignerFilter::convert(const uint8_t* buf, size_t size)
{
  if (EVP_DigestSignUpdate(m_impl->ctx, buf, size) != 1)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to accept more input"));

  return size;
}

void
SignerFilter::finalize()
{
  size_t sigLen = 0;
  if (EVP_DigestSignFinal(m_impl->ctx, nullptr, &sigLen) != 1)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to estimate buffer length"));

  auto buffer = make_unique<OBuffer>(sigLen);
  if (EVP_DigestSignFinal(m_impl->ctx, buffer->data(), &sigLen) != 1)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to finalize signature"));

  buffer->erase(buffer->begin() + sigLen, buffer->end());
  setOutputBuffer(std::move(buffer));

  flushAllOutput();
}

unique_ptr<Transform>
signerFilter(DigestAlgorithm algo, const PrivateKey& key)
{
  return make_unique<SignerFilter>(algo, key);
}

} // namespace transform
} // namespace security
} // namespace ndn

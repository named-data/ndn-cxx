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

#include "verifier-filter.hpp"
#include "public-key.hpp"
#include "../detail/openssl-helper.hpp"

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
{
  const EVP_MD* md = detail::digestAlgorithmToEvpMd(algo);
  if (md == nullptr)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Unsupported digest algorithm " +
                                boost::lexical_cast<std::string>(algo)));

  if (EVP_DigestVerifyInit(m_impl->ctx, nullptr, md, nullptr,
                           reinterpret_cast<EVP_PKEY*>(key.getEvpPkey())) != 1)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to initialize verification context with " +
                                boost::lexical_cast<std::string>(algo) + " digest and " +
                                boost::lexical_cast<std::string>(key.getKeyType()) + " key"));
}

VerifierFilter::~VerifierFilter() = default;

size_t
VerifierFilter::convert(const uint8_t* buf, size_t size)
{
  if (EVP_DigestVerifyUpdate(m_impl->ctx, buf, size) != 1)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to accept more input"));

  return size;
}

void
VerifierFilter::finalize()
{
  int res = EVP_DigestVerifyFinal(m_impl->ctx,
#if OPENSSL_VERSION_NUMBER < 0x1000200fL
                                  const_cast<uint8_t*>(m_impl->sig),
#else
                                  m_impl->sig,
#endif
                                  m_impl->siglen);

  auto buffer = make_unique<OBuffer>(1);
  (*buffer)[0] = (res == 1) ? 1 : 0;
  setOutputBuffer(std::move(buffer));

  flushAllOutput();
}

unique_ptr<Transform>
verifierFilter(DigestAlgorithm algo, const PublicKey& key,
               const uint8_t* sig, size_t sigLen)
{
  return make_unique<VerifierFilter>(algo, key, sig, sigLen);
}

} // namespace transform
} // namespace security
} // namespace ndn

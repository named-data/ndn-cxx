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

#include "verifier-filter.hpp"
#include "../detail/openssl.hpp"

namespace ndn {
namespace security {
namespace transform {

class VerifierFilter::Impl
{
public:
  Impl(const PublicKey& key, const uint8_t* sig, size_t sigLen)
    : m_key(key)
    , m_md(BIO_new(BIO_f_md()))
    , m_sink(BIO_new(BIO_s_null()))
    , m_sig(sig)
    , m_sigLen(sigLen)
  {
    BIO_push(m_md, m_sink);
  }

  ~Impl()
  {
    BIO_free_all(m_md);
  }

public:
  const PublicKey& m_key;

  BIO* m_md;
  BIO* m_sink;

  const uint8_t* m_sig;
  size_t m_sigLen;
};

VerifierFilter::VerifierFilter(DigestAlgorithm algo, const PublicKey& key,
                               const uint8_t* sig, size_t sigLen)
  : m_impl(new Impl(key, sig, sigLen))
{
  switch (algo) {
    case DigestAlgorithm::SHA256: {
      if (!BIO_set_md(m_impl->m_md, EVP_sha256()))
        BOOST_THROW_EXCEPTION(Error(getIndex(), "Cannot set digest"));
      break;
    }

    default:
      BOOST_THROW_EXCEPTION(Error(getIndex(), "Digest algorithm is not supported"));
  }
}

size_t
VerifierFilter::convert(const uint8_t* buf, size_t size)
{
  int wLen = BIO_write(m_impl->m_md, buf, size);

  if (wLen <= 0) { // fail to write data
    if (!BIO_should_retry(m_impl->m_md)) {
      // we haven't written everything but some error happens, and we cannot retry
      BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to accept more input"));
    }
    return 0;
  }
  else { // update number of bytes written
    return wLen;
  }
}

void
VerifierFilter::finalize()
{
  EVP_PKEY* key = reinterpret_cast<EVP_PKEY*>(m_impl->m_key.getEvpPkey());
  auto buffer = make_unique<OBuffer>(1);

  EVP_MD_CTX* ctx = nullptr;
  BIO_get_md_ctx(m_impl->m_md, &ctx);
  int res = EVP_VerifyFinal(ctx, m_impl->m_sig, m_impl->m_sigLen, key);

  if (res < 0)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Verification error"));

  (*buffer)[0] = (res != 0) ? 1 : 0;
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

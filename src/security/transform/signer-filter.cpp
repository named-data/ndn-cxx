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

#include "signer-filter.hpp"
#include "../../encoding/buffer.hpp"
#include "../detail/openssl.hpp"

namespace ndn {
namespace security {
namespace transform {

class SignerFilter::Impl
{
public:
  Impl(const PrivateKey& key)
    : m_key(key)
    , m_md(BIO_new(BIO_f_md()))
    , m_sink(BIO_new(BIO_s_null()))
  {
    BIO_push(m_md, m_sink);
  }

  ~Impl()
  {
    BIO_free_all(m_md);
  }

public:
  const PrivateKey& m_key;

  BIO* m_md;
  BIO* m_sink;
};

SignerFilter::SignerFilter(DigestAlgorithm algo, const PrivateKey& key)
  : m_impl(new Impl(key))
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
SignerFilter::convert(const uint8_t* buf, size_t size)
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
SignerFilter::finalize()
{
  EVP_PKEY* key = reinterpret_cast<EVP_PKEY*>(m_impl->m_key.getEvpPkey());
  auto buffer = make_unique<OBuffer>(EVP_PKEY_size(key));
  unsigned int sigLen = 0;

  EVP_MD_CTX* ctx = nullptr;
  BIO_get_md_ctx(m_impl->m_md, &ctx);
  EVP_SignFinal(ctx, &(*buffer)[0], &sigLen, key); // should be ok, enough space is allocated in buffer

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

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

#include "digest-filter.hpp"
#include "../../encoding/buffer.hpp"
#include "../detail/openssl-helper.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace transform {

/**
 * @brief The implementation class which contains the internal state of
 *        the digest calculator which includes openssl specific structures.
 */
class DigestFilter::Impl
{
public:
  Impl()
    : m_md(BIO_new(BIO_f_md()))
    , m_sink(BIO_new(BIO_s_null()))
  {
    BIO_push(m_md, m_sink);
  }

  ~Impl()
  {
    BIO_free_all(m_md);
  }

public:
  BIO* m_md;
  BIO* m_sink;
};

DigestFilter::DigestFilter(DigestAlgorithm algo)
  : m_impl(new Impl)
{
  const EVP_MD* md = detail::toDigestEvpMd(algo);
  if (md == nullptr) {
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Unsupported digest algorithm " +
                                boost::lexical_cast<std::string>(algo)));
  }

  if (!BIO_set_md(m_impl->m_md, md)) {
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Cannot set digest"+
                                boost::lexical_cast<std::string>(algo)));
  }
}

size_t
DigestFilter::convert(const uint8_t* buf, size_t size)
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
DigestFilter::finalize()
{
  auto buffer = make_unique<OBuffer>(EVP_MAX_MD_SIZE);

  int mdLen = BIO_gets(m_impl->m_md, reinterpret_cast<char*>(&(*buffer)[0]), EVP_MAX_MD_SIZE);
  if (mdLen <= 0)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to compute digest"));

  buffer->erase(buffer->begin() + mdLen, buffer->end());
  setOutputBuffer(std::move(buffer));

  flushAllOutput();
}

unique_ptr<Transform>
digestFilter(DigestAlgorithm algo)
{
  return make_unique<DigestFilter>(algo);
}

} // namespace transform
} // namespace security
} // namespace ndn

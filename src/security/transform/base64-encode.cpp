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

#include "base64-encode.hpp"
#include "../detail/openssl.hpp"

namespace ndn {
namespace security {
namespace transform {

/**
 * @brief The implementation class which contains the internal state of the filter
 *        which includes openssl specific structures.
 */
class Base64Encode::Impl
{
public:
  Impl()
    : m_base64(BIO_new(BIO_f_base64()))
    , m_sink(BIO_new(BIO_s_mem()))
  {
    // connect base64 transform to the data sink.
    BIO_push(m_base64, m_sink);
  }

  ~Impl()
  {
    BIO_free_all(m_base64);
  }

public:
  BIO* m_base64;
  BIO* m_sink; // BIO_f_base64 alone does not work without a sink
};

Base64Encode::Base64Encode(bool needBreak)
  : m_impl(make_unique<Impl>())
{
  if (!needBreak)
    BIO_set_flags(m_impl->m_base64, BIO_FLAGS_BASE64_NO_NL);
}

Base64Encode::~Base64Encode() = default;

void
Base64Encode::preTransform()
{
  fillOutputBuffer();
}

size_t
Base64Encode::convert(const uint8_t* data, size_t dataLen)
{
  if (dataLen == 0)
    return 0;

  int wLen = BIO_write(m_impl->m_base64, data, dataLen);

  if (wLen <= 0) { // fail to write data
    if (!BIO_should_retry(m_impl->m_base64)) {
      // we haven't written everything but some error happens, and we cannot retry
      BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to accept more input"));
    }
    return 0;
  }
  else { // update number of bytes written
    fillOutputBuffer();
    return wLen;
  }
}

void
Base64Encode::finalize()
{
  if (BIO_flush(m_impl->m_base64) != 1)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to flush"));

  while (!isConverterEmpty()) {
    fillOutputBuffer();
    while (!isOutputBufferEmpty()) {
      flushOutputBuffer();
    }
  }
}

void
Base64Encode::fillOutputBuffer()
{
  int nRead = BIO_pending(m_impl->m_sink);
  if (nRead <= 0)
    return;

  // there is something to read from BIO
  auto buffer = make_unique<OBuffer>(nRead);
  int rLen = BIO_read(m_impl->m_sink, buffer->data(), nRead);
  if (rLen < 0)
    return;

  if (rLen < nRead)
    buffer->erase(buffer->begin() + rLen, buffer->end());
  setOutputBuffer(std::move(buffer));
}

bool
Base64Encode::isConverterEmpty()
{
  return (BIO_pending(m_impl->m_sink) <= 0);
}

unique_ptr<Transform>
base64Encode(bool needBreak)
{
  return make_unique<Base64Encode>(needBreak);
}

} // namespace transform
} // namespace security
} // namespace ndn

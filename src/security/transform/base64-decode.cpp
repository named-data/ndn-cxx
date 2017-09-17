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

#include "base64-decode.hpp"
#include "../detail/openssl.hpp"

namespace ndn {
namespace security {
namespace transform {

/**
 * @brief The implementation class which contains the internal state of the filter
 *        which includes openssl specific structures.
 */
class Base64Decode::Impl
{
public:
  Impl()
    : m_base64(BIO_new(BIO_f_base64()))
    , m_source(BIO_new(BIO_s_mem()))
  {
    // Input may not be written in a single time.
    // Do not return EOF when source is empty unless explicitly requested
    BIO_set_mem_eof_return(m_source, -1);

    // connect base64 transform to the data source.
    BIO_push(m_base64, m_source);
  }

  ~Impl()
  {
    BIO_free_all(m_base64);
  }

public:
  BIO* m_base64;
  BIO* m_source; // BIO_f_base64 alone does not work without a source
};

static const size_t BUFFER_LENGTH = 1024;

Base64Decode::Base64Decode(bool expectNewlineEvery64Bytes)
  : m_impl(make_unique<Impl>())
{
  if (!expectNewlineEvery64Bytes)
    BIO_set_flags(m_impl->m_base64, BIO_FLAGS_BASE64_NO_NL);
}

Base64Decode::~Base64Decode() = default;

void
Base64Decode::preTransform()
{
  while (isOutputBufferEmpty()) {
    fillOutputBuffer();
    if (isOutputBufferEmpty()) // nothing to read from BIO, return
      return;

    flushOutputBuffer();
  }
}

size_t
Base64Decode::convert(const uint8_t* buf, size_t size)
{
  int wLen = BIO_write(m_impl->m_source, buf, size);

  if (wLen <= 0) { // fail to write data
    if (!BIO_should_retry(m_impl->m_source)) {
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
Base64Decode::finalize()
{
  BIO_set_mem_eof_return(m_impl->m_source, 0);

  fillOutputBuffer();

  while (!isOutputBufferEmpty()) {
    flushOutputBuffer();
    if (isOutputBufferEmpty())
      fillOutputBuffer();
  }
}

void
Base64Decode::fillOutputBuffer()
{
  // OpenSSL base64 BIO cannot give us the number bytes of partial decoded result,
  // so we just try to read a chunk.
  auto buffer = make_unique<OBuffer>(BUFFER_LENGTH);
  int rLen = BIO_read(m_impl->m_base64, buffer->data(), buffer->size());
  if (rLen <= 0)
    return;

  if (static_cast<size_t>(rLen) < buffer->size())
    buffer->erase(buffer->begin() + rLen, buffer->end());

  setOutputBuffer(std::move(buffer));
}

unique_ptr<Transform>
base64Decode(bool expectNewlineEvery64Bytes)
{
  return make_unique<Base64Decode>(expectNewlineEvery64Bytes);
}

} // namespace transform
} // namespace security
} // namespace ndn

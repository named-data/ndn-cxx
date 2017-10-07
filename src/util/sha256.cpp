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

#include "sha256.hpp"
#include "string-helper.hpp"
#include "../security/detail/openssl.hpp"
#include "../security/transform/digest-filter.hpp"
#include "../security/transform/stream-sink.hpp"
#include "../security/transform/stream-source.hpp"

namespace ndn {
namespace util {

const size_t Sha256::DIGEST_SIZE;

Sha256::Sha256()
{
  reset();
}

Sha256::Sha256(std::istream& is)
  : m_output(make_unique<OBufferStream>())
  , m_isEmpty(false)
  , m_isFinalized(true)
{
  namespace tr = security::transform;

  tr::streamSource(is) >> tr::digestFilter(DigestAlgorithm::SHA256) >> tr::streamSink(*m_output);
}

void
Sha256::reset()
{
  namespace tr = security::transform;

  m_input = make_unique<tr::StepSource>();
  m_output = make_unique<OBufferStream>();
  m_isEmpty = true;
  m_isFinalized = false;

  *m_input >> tr::digestFilter(DigestAlgorithm::SHA256) >> tr::streamSink(*m_output);
}

ConstBufferPtr
Sha256::computeDigest()
{
  if (!m_isFinalized) {
    BOOST_ASSERT(m_input != nullptr);
    m_input->end();
    m_isFinalized = true;
  }

  return m_output->buf();
}

bool
Sha256::operator==(Sha256& digest)
{
  const Buffer& lhs = *computeDigest();
  const Buffer& rhs = *digest.computeDigest();

  if (lhs.size() != rhs.size()) {
    return false;
  }

  // constant-time buffer comparison to mitigate timing attacks
  return CRYPTO_memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
}

Sha256&
Sha256::operator<<(Sha256& src)
{
  auto buf = src.computeDigest();
  update(buf->data(), buf->size());
  return *this;
}

Sha256&
Sha256::operator<<(const std::string& str)
{
  update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
  return *this;
}

Sha256&
Sha256::operator<<(const Block& block)
{
  update(block.wire(), block.size());
  return *this;
}

Sha256&
Sha256::operator<<(uint64_t value)
{
  update(reinterpret_cast<const uint8_t*>(&value), sizeof(uint64_t));
  return *this;
}

void
Sha256::update(const uint8_t* buffer, size_t size)
{
  if (m_isFinalized)
    BOOST_THROW_EXCEPTION(Error("Digest has been already finalized"));

  BOOST_ASSERT(m_input != nullptr);
  m_input->write(buffer, size);
  m_isEmpty = false;
}

std::string
Sha256::toString()
{
  auto buf = computeDigest();
  return toHex(*buf);
}

ConstBufferPtr
Sha256::computeDigest(const uint8_t* buffer, size_t size)
{
  Sha256 sha256;
  sha256.update(buffer, size);
  return sha256.computeDigest();
}

std::ostream&
operator<<(std::ostream& os, Sha256& digest)
{
  auto buf = digest.computeDigest();
  printHex(os, *buf);
  return os;
}

} // namespace util
} // namespace ndn

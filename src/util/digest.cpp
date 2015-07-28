/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "digest.hpp"
#include "string-helper.hpp"
#include <sstream>

namespace ndn {
namespace util {

template<typename Hash>
Digest<Hash>::Digest()
{
  reset();
}

template<typename Hash>
Digest<Hash>::Digest(std::istream& is)
  : m_isInProcess(false)
  , m_isFinalized(true)
{
  using namespace CryptoPP;

  m_buffer = make_shared<Buffer>(m_hash.DigestSize());
  FileSource(is, true,
             new HashFilter(m_hash,
                            new ArraySink(m_buffer->get(), m_buffer->size())));
}

template<typename Hash>
void
Digest<Hash>::reset()
{
  m_hash.Restart();
  m_buffer = make_shared<Buffer>(m_hash.DigestSize());
  m_isInProcess = false;
  m_isFinalized = false;
}

template<typename Hash>
void
Digest<Hash>::finalize()
{
  // return immediately if Digest is finalized already.
  if (m_isFinalized)
    return;

  m_hash.Final(m_buffer->get());

  m_isFinalized = true;
}

template<typename Hash>
ConstBufferPtr
Digest<Hash>::computeDigest()
{
  finalize();
  return m_buffer;
}

template<typename Hash>
bool
Digest<Hash>::operator==(Digest<Hash>& digest)
{
  return *computeDigest() == *digest.computeDigest();
}

template<typename Hash>
Digest<Hash>&
Digest<Hash>::operator<<(Digest<Hash>& src)
{
  ConstBufferPtr buffer = src.computeDigest();
  update(buffer->get(), buffer->size());

  return *this;
}

template<typename Hash>
Digest<Hash>&
Digest<Hash>::operator<<(const std::string& str)
{
  update(reinterpret_cast<const uint8_t*>(str.c_str()), str.size());

  return *this;
}

template<typename Hash>
Digest<Hash>&
Digest<Hash>::operator<<(const Block& block)
{
  update(block.wire(), block.size());

  return *this;
}

template<typename Hash>
Digest<Hash>&
Digest<Hash>::operator<<(uint64_t value)
{
  update(reinterpret_cast<const uint8_t*>(&value), sizeof(uint64_t));

  return *this;
}

template<typename Hash>
void
Digest<Hash>::update(const uint8_t* buffer, size_t size)
{
  // cannot update Digest when it has been finalized
  if (m_isFinalized)
    BOOST_THROW_EXCEPTION(Error("Digest has been already finalized"));

  m_hash.Update(buffer, size);

  m_isInProcess = true;
}

template<typename Hash>
ConstBufferPtr
Digest<Hash>::computeDigest(const uint8_t* buffer, size_t size)
{
  Hash hash;
  BufferPtr result = make_shared<Buffer>(hash.DigestSize());
  hash.Update(buffer, size);
  hash.Final(result->get());

  return result;
}

template<typename Hash>
std::string
Digest<Hash>::toString()
{
  std::ostringstream os;
  os << *this;

  return os.str();
}

template<typename Hash>
std::ostream&
operator<<(std::ostream& os, Digest<Hash>& digest)
{
  ConstBufferPtr buffer = digest.computeDigest();
  printHex(os, buffer->buf(), buffer->size());

  return os;
}

template
class Digest<CryptoPP::SHA256>;

template
std::ostream&
operator<<(std::ostream& os, Digest<CryptoPP::SHA256>& digest);


} // namespace util
} // namespace ndn

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

#include "buffer-source.hpp"

namespace ndn {
namespace security {
namespace transform {

BufferSource::BufferSource(const uint8_t* buf, size_t size)
  : m_buf(buf)
  , m_size(size)
{
}

BufferSource::BufferSource(const std::string& string)
  : m_buf(reinterpret_cast<const uint8_t*>(string.data()))
  , m_size(string.size())
{
}

BufferSource::BufferSource(const Buffer& buffer)
  : m_buf(buffer.data())
  , m_size(buffer.size())
{
}

void
BufferSource::doPump()
{
  BOOST_ASSERT(m_next != nullptr);

  const uint8_t* buf = m_buf;
  size_t size = m_size;

  while (0 < size) {
    size_t nBytesWritten = m_next->write(buf, size);
    buf += nBytesWritten;
    size -= nBytesWritten;
  }

  m_next->end();
}

} // namespace transform
} // namespace security
} // namespace ndn

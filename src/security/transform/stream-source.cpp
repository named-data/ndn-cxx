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

#include "stream-source.hpp"

#include <istream>
#include <vector>

namespace ndn {
namespace security {
namespace transform {

const std::size_t StreamSource::DEFAULT_BUFFER_LEN = 1024;

StreamSource::StreamSource(std::istream& is, size_t bufferSize)
  : Source()
  , m_is(is)
  , m_bufferSize(bufferSize)
{
  BOOST_ASSERT(bufferSize > 0);
}

void
StreamSource::doPump()
{
  BOOST_ASSERT(m_next != nullptr);

  std::vector<uint8_t> buffer(m_bufferSize);
  size_t dataOffset = 0;
  size_t dataLen = 0;

  while (dataLen > 0 || !m_is.eof()) {
    if (dataLen > 0) {
      // we have some leftover, handle them first
      size_t nBytesWritten = m_next->write(&buffer[dataOffset], dataLen);

      dataOffset += nBytesWritten;
      dataLen -= nBytesWritten;
    }
    else if (!m_is) {
      BOOST_THROW_EXCEPTION(Error(getIndex(), "Input stream in bad state"));
    }
    else if (m_is.good()) {
      m_is.read(reinterpret_cast<char*>(&buffer.front()), buffer.size());
      dataOffset = 0;
      dataLen = m_is.gcount();
    }
  }
  m_next->end();
}

} // namespace transform
} // namespace security
} // namespace ndn

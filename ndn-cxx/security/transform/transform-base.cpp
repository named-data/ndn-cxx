/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/security/transform/transform-base.hpp"

namespace ndn {
namespace security {
namespace transform {

Error::Error(size_t index, const std::string& what)
  : std::runtime_error("Error in module " + to_string(index) + ": " + what)
  , m_index(index)
{
}

size_t
Downstream::write(span<const uint8_t> buf)
{
  if (m_isEnd)
    NDN_THROW(Error(getIndex(), "Module is closed, no more input"));

  size_t nBytesWritten = doWrite(buf);
  BOOST_ASSERT(nBytesWritten <= buf.size());
  return nBytesWritten;
}

void
Downstream::end()
{
  if (m_isEnd)
    return;

  m_isEnd = true;
  doEnd();
}

void
Upstream::appendChain(unique_ptr<Downstream> tail)
{
  if (m_next == nullptr) {
    m_next = std::move(tail);
  }
  else {
    BOOST_ASSERT(dynamic_cast<Transform*>(m_next.get()) != nullptr);
    static_cast<Transform*>(m_next.get())->appendChain(std::move(tail));
  }
}

void
Transform::flushOutputBuffer()
{
  if (isOutputBufferEmpty())
    return;

  size_t nWritten = m_next->write(make_span(*m_oBuffer).subspan(m_outputOffset));
  m_outputOffset += nWritten;
}

void
Transform::flushAllOutput()
{
  while (!isOutputBufferEmpty()) {
    flushOutputBuffer();
  }
}

void
Transform::setOutputBuffer(unique_ptr<OBuffer> buffer)
{
  BOOST_ASSERT(isOutputBufferEmpty());
  m_oBuffer = std::move(buffer);
  m_outputOffset = 0;
}

bool
Transform::isOutputBufferEmpty() const
{
  return m_oBuffer == nullptr || m_oBuffer->size() == m_outputOffset;
}

size_t
Transform::doWrite(span<const uint8_t> data)
{
  flushOutputBuffer();
  if (!isOutputBufferEmpty())
    return 0;

  preTransform();
  flushOutputBuffer();
  if (!isOutputBufferEmpty())
    return 0;

  size_t nConverted = convert(data);
  flushOutputBuffer();
  return nConverted;
}

void
Transform::doEnd()
{
  finalize();
  m_next->end();
}

void
Transform::preTransform()
{
}

void
Transform::finalize()
{
  flushAllOutput();
}

void
Source::pump()
{
  doPump();
}

Source&
Source::operator>>(unique_ptr<Transform> transform)
{
  transform->setIndex(m_nModules);
  m_nModules++;
  appendChain(std::move(transform));

  return *this;
}

void
Source::operator>>(unique_ptr<Sink> sink)
{
  sink->setIndex(m_nModules);
  m_nModules++;
  appendChain(std::move(sink));

  pump();
}

} // namespace transform
} // namespace security
} // namespace ndn

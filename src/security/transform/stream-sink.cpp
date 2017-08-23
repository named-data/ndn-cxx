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

#include "stream-sink.hpp"

#include <ostream>

namespace ndn {
namespace security {
namespace transform {

StreamSink::StreamSink(std::ostream& os)
  : m_os(os)
{
}

size_t
StreamSink::doWrite(const uint8_t* buf, size_t size)
{
  m_os.write(reinterpret_cast<const char*>(buf), size);

  if (m_os.bad())
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Fail to write data into output stream"));

  return size;
}

void
StreamSink::doEnd()
{
  m_os.flush();
}

unique_ptr<Sink>
streamSink(std::ostream& os)
{
  return make_unique<StreamSink>(os);
}


} // namespace transform
} // namespace security
} // namespace ndn

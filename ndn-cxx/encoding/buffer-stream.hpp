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
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#ifndef NDN_CXX_ENCODING_BUFFER_STREAM_HPP
#define NDN_CXX_ENCODING_BUFFER_STREAM_HPP

#include "ndn-cxx/encoding/buffer.hpp"

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>

namespace ndn {
namespace detail {

/**
 * @brief (implementation detail) A Boost.Iostreams.Sink that appends to a Buffer.
 */
class BufferSink : public boost::iostreams::sink
{
public:
  explicit
  BufferSink(Buffer& container);

  std::streamsize
  write(const char_type* s, std::streamsize n);

private:
  Buffer& m_container;
};

} // namespace detail

/**
 * @brief An output stream that writes to a Buffer.
 *
 * The benefit of using stream interface is that it provides automatic buffering of
 * written data and eliminates (or reduces) overhead of resizing the underlying buffer
 * when writing small pieces of data.
 *
 * Usage example:
 * @code
 * OBufferStream obuf;
 * obuf << "foo";
 * obuf.put(0);
 * obuf.write(anotherBuffer, anotherBufferSize);
 * std::shared_ptr<Buffer> buf = obuf.buf();
 * @endcode
 */
class OBufferStream : public boost::iostreams::stream<detail::BufferSink>
{
public:
  OBufferStream();

  ~OBufferStream() noexcept override;

  /**
   * @brief Return a shared pointer to the underlying buffer.
   */
  shared_ptr<Buffer>
  buf();

private:
  shared_ptr<Buffer> m_buffer;
};

} // namespace ndn

#endif // NDN_CXX_ENCODING_BUFFER_STREAM_HPP

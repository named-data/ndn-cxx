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

#ifndef NDN_CXX_SECURITY_TRANSFORM_STREAM_SINK_HPP
#define NDN_CXX_SECURITY_TRANSFORM_STREAM_SINK_HPP

#include "transform-base.hpp"

namespace ndn {
namespace security {
namespace transform {

/**
 * @brief A sink which directs output to an std::ostream.
 */
class StreamSink : public Sink
{
public:
  /**
   * @brief Create a stream sink which outputs to @p os
   */
  explicit
  StreamSink(std::ostream& os);

private:
  /**
   * @brief Write data into the stream
   *
   * @return number of bytes that have been written into the stream
   */
  size_t
  doWrite(const uint8_t* buf, size_t size) final;

  /**
   * @brief Finalize sink processing
   */
  void
  doEnd() final;

private:
  std::ostream& m_os;
};

unique_ptr<Sink>
streamSink(std::ostream& os);

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_STREAM_SINK_HPP

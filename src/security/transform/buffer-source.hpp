/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_TRANSFORM_BUFFER_SOURCE_HPP
#define NDN_CXX_SECURITY_TRANSFORM_BUFFER_SOURCE_HPP

#include "transform-base.hpp"
#include "../../encoding/buffer.hpp"

namespace ndn {
namespace security {
namespace transform {

/**
 * @brief A source taking a memory buffer as input
 */
class BufferSource : public Source
{
public:
  /**
   * @brief Take a buffer @p buf with size of @p size as input.
   *
   * Caller must not destroy the buffer before transformation is done
   */
  BufferSource(const uint8_t* buf, size_t size);

  /**
   * @brief Take @p string as input.
   *
   * Caller must not destroy the string before transformation is done
   */
  explicit
  BufferSource(const std::string& string);

  /**
   * @brief Take @p buffer as input.
   *
   * Caller must not destroy the buffer before transformation is done
   */
  explicit
  BufferSource(const Buffer& buffer);

private:
  /**
   * @brief Write the whole buffer into the next module.
   */
  void
  doPump() final;

private:
  const uint8_t* m_buf;
  size_t m_size;
};

typedef BufferSource bufferSource;

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_BUFFER_SOURCE_HPP

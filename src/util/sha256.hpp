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

#ifndef NDN_UTIL_SHA256_HPP
#define NDN_UTIL_SHA256_HPP

#include "../encoding/block.hpp"
#include "../encoding/buffer-stream.hpp"
#include "../security/transform/step-source.hpp"

namespace ndn {
namespace util {

/**
 * @brief Provides stateful SHA-256 digest calculation.
 *
 * Example:
 * @code
 * Sha256 digest;
 * digest.update(buf1, size1);
 * digest.update(buf2, size2);
 * ...
 * ConstBufferPtr result = digest.computeDigest();
 * @endcode
 */
class Sha256
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  /**
   * @brief Length in bytes of a SHA-256 digest.
   */
  static const size_t DIGEST_SIZE = 32;

  /**
   * @brief Create an empty SHA-256 digest.
   */
  Sha256();

  /**
   * @brief Calculate SHA-256 digest of the input stream @p is.
   */
  explicit
  Sha256(std::istream& is);

  /**
   * @brief Check if digest is empty.
   *
   * An empty digest means nothing has been taken into calculation.
   */
  bool
  empty() const
  {
    return m_isEmpty;
  }

  /**
   * @brief Discard the current state and start a new digest calculation.
   */
  void
  reset();

  /**
   * @brief Finalize and return the digest based on all previously supplied inputs.
   */
  ConstBufferPtr
  computeDigest();

  /**
   * @brief Check if the supplied digest is equal to this digest.
   * @note This method invokes computeDigest() on both operands, finalizing the digest.
   */
  bool
  operator==(Sha256& digest);

  /**
   * @brief Check if the supplied digest is not equal to this digest.
   * @note This method invokes computeDigest() on both operands, finalizing the digest.
   */
  bool
  operator!=(Sha256& digest)
  {
    return !(*this == digest);
  }

  /**
   * @brief Add existing digest to the digest calculation.
   * @param src digest to combine with
   *
   * The result of this combination is `sha256(sha256(...))`
   *
   * @note This method invokes computeDigest() on @p src, finalizing the digest.
   * @throw Error the digest has already been finalized
   */
  Sha256&
  operator<<(Sha256& src);

  /**
   * @brief Add a string to the digest calculation.
   * @throw Error the digest has already been finalized
   */
  Sha256&
  operator<<(const std::string& str);

  /**
   * @brief Add a block to the digest calculation.
   * @throw Error the digest has already been finalized
   */
  Sha256&
  operator<<(const Block& block);

  /**
   * @brief Add a uint64_t value to the digest calculation.
   * @throw Error the digest has already been finalized
   */
  Sha256&
  operator<<(uint64_t value);

  /**
   * @brief Add a raw buffer to the digest calculation.
   * @param buffer the input buffer
   * @param size the size of the input buffer
   * @throw Error the digest has already been finalized
   */
  void
  update(const uint8_t* buffer, size_t size);

  /**
   * @brief Convert digest to std::string.
   * @note This method invokes computeDigest(), finalizing the digest.
   */
  std::string
  toString();

  /**
   * @brief Stateless SHA-256 digest calculation.
   * @param buffer the input buffer
   * @param size the size of the input buffer
   * @return SHA-256 digest of the input buffer
   */
  static ConstBufferPtr
  computeDigest(const uint8_t* buffer, size_t size);

private:
  unique_ptr<security::transform::StepSource> m_input;
  unique_ptr<OBufferStream> m_output;
  bool m_isEmpty;
  bool m_isFinalized;
};

std::ostream&
operator<<(std::ostream& os, Sha256& digest);

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_SHA256_HPP

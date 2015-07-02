/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_UTIL_DIGEST_HPP
#define NDN_UTIL_DIGEST_HPP

#include "../encoding/buffer.hpp"
#include "../encoding/block.hpp"
#include "../security/cryptopp.hpp"
#include "concepts.hpp"

namespace ndn {
namespace util {

/**
 * @brief provides a  digest calculation
 *
 * Take SHA256 as an example:
 *
 *   Digest<CryptoPP::SHA256> digest;
 *   digest.update(buf1, size1);
 *   digest.update(buf2, size2);
 *   ...
 *   ConstBufferPtr result = digest.computeDigest();
 *
 * @sa http://redmine.named-data.net/issues/1934
 */
template<typename Hash>
class Digest
{
public:
  BOOST_CONCEPT_ASSERT((Hashable<Hash>));

  typedef Hash HashFunction;

  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  Digest();

  /**
   * @brief Create digest of the input stream @p
   * @param is input stream
   */
  explicit
  Digest(std::istream& is);

  /**
   * @brief Discard the current state and start a new digest.
   */
  void
  reset();

  /**
   * @brief Check if digest is empty.
   *
   * An empty digest means nothing has been taken into calculation.
   */
  bool
  empty() const
  {
    return !m_isInProcess;
  }

  /**
   * @brief Obtain the digest
   *
   * Note this digest is finalized once this method is invoked.
   */
  ConstBufferPtr
  computeDigest();

  /**
   * @brief Check if supplied digest equal to this digest
   *
   * Note that this method will invoke computeDigest().
   * Once this method is invoked, both this digest and the supplied digest are finalized.
   *
   * @warning This method cannot be used in security related context
   *          because it is vulnerable to timing attack
   */
  bool
  operator==(Digest<Hash>& digest);

  /**
   * @brief Check if supplied digest is not equal to this digest
   *
   * Note that this method will invoke computeDigest().
   * Once this method is invoked, both this digest and the supplied digest are finalized.
   *
   * @warning This method cannot be used in security related context
   *          because it is vulnerable to timing attack
   */
  bool
  operator!=(Digest<Hash>& digest)
  {
    return !(*this == digest);
  }

  /**
   * @brief Add existing digest to digest calculation
   * @param src digest to combine with
   *
   * The result of this combination is  hash (hash (...))
   * Note that this method will invoke computeDigest().
   * Once this method is invoked, the supplied digest is fixed.
   */
  Digest<Hash>&
  operator<<(Digest<Hash>& src);

  /**
   * @brief Add string to digest calculation
   * @param str string to put into digest
   */
  Digest<Hash>&
  operator<<(const std::string& str);

  /**
   * @brief Add block to digest calculation
   * @param block to put into digest
   */
  Digest<Hash>&
  operator<<(const Block& block);

  /**
   * @brief Add uint64_t value to digest calculation
   * @param value uint64_t value to put into digest
   */
  Digest<Hash>&
  operator<<(uint64_t value);

  /**
   * @brief Add a buffer to digest calculation
   *
   * Update the state of the digest if it is not finalized
   * and mark the digest as InProcess.
   *
   * @param buffer the input buffer
   * @param size the size of the input buffer.
   * @throws Error if the digest has been finalized.
   */
  void
  update(const uint8_t* buffer, size_t size);

  /**
   * @brief Compute one-time digest
   * @param buffer the input buffer
   * @param size the size of the input buffer.
   * @return digest computed according to the HashAlgorithm
   */
  static ConstBufferPtr
  computeDigest(const uint8_t* buffer, size_t size);

  /**
   * @brief Convert digest to std::string
   *
   * Note that this method will invoke computeDigest().
   * Once this method is invoked, the digest is finalized.
   */
  std::string
  toString();

private:
  /**
   * @brief Finalize digest.
   *
   * All subsequent calls to "operator<<" will throw an exception
   */
  void
  finalize();

private:
  Hash m_hash;
  BufferPtr m_buffer;
  bool m_isInProcess;
  bool m_isFinalized;
};

template<typename Hash>
std::ostream&
operator<<(std::ostream& os, Digest<Hash>& digest);

/**
 * @brief A digest using SHA256 as the hash function.
 */
typedef Digest<CryptoPP::SHA256> Sha256;

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_DIGEST_HPP

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

#ifndef NDN_ENCODING_ENCODER_HPP
#define NDN_ENCODING_ENCODER_HPP

#include "block.hpp"

namespace ndn {
namespace encoding {

/**
 * @brief Helper class to perform TLV encoding
 * Interface of this class (mostly) matches interface of Estimator class
 * @sa Estimator
 */
class Encoder
{
public: // common interface between Encoder and Estimator
  /**
   * @brief Create instance of the encoder with the specified reserved sizes
   * @param totalReserve    initial buffer size to reserve
   * @param reserveFromBack number of bytes to reserve for append* operations
   */
  explicit
  Encoder(size_t totalReserve = MAX_NDN_PACKET_SIZE, size_t reserveFromBack = 400);

  Encoder(const Encoder&) = delete;

  Encoder&
  operator=(const Encoder&) = delete;

  /**
   * @brief Prepend a byte
   */
  size_t
  prependByte(uint8_t value);

  /**
   * @brief Append a byte
   */
  size_t
  appendByte(uint8_t value);

  /**
   * @brief Prepend a byte array @p array of length @p length
   */
  size_t
  prependByteArray(const uint8_t* array, size_t length);

  /**
   * @brief Append a byte array @p array of length @p length
   */
  size_t
  appendByteArray(const uint8_t* array, size_t length);

  /**
   * @brief Prepend range of bytes from the range [@p first, @p last)
   */
  template<class Iterator>
  size_t
  prependRange(Iterator first, Iterator last);

  /**
   * @brief Append range of bytes from the range [@p first, @p last)
   */
  template<class Iterator>
  size_t
  appendRange(Iterator first, Iterator last);

  /**
   * @brief Prepend VarNumber @p varNumber of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  prependVarNumber(uint64_t varNumber);

  /**
   * @brief Prepend VarNumber @p varNumber of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  appendVarNumber(uint64_t varNumber);

  /**
   * @brief Prepend non-negative integer @p integer of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  prependNonNegativeInteger(uint64_t integer);

  /**
   * @brief Append non-negative integer @p integer of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  appendNonNegativeInteger(uint64_t integer);

  /**
   * @brief Prepend TLV block of type @p type and value from buffer @p array of size @p arraySize
   */
  size_t
  prependByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize);

  /**
   * @brief Append TLV block of type @p type and value from buffer @p array of size @p arraySize
   */
  size_t
  appendByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize);

  /**
   * @brief Prepend TLV block @p block
   */
  size_t
  prependBlock(const Block& block);

  /**
   * @brief Append TLV block @p block
   */
  size_t
  appendBlock(const Block& block);

public: // unique interface to the Encoder
  using value_type = Buffer::value_type;
  using iterator = Buffer::iterator;
  using const_iterator = Buffer::const_iterator;

  /**
   * @brief Create EncodingBlock from existing block
   *
   * This is a dangerous constructor and should be used with caution.
   * It will modify contents of the buffer that is used by block and may
   * impact data in other blocks.
   *
   * The primary purpose for this method is to be used to extend Block
   * after sign operation.
   */
  explicit
  Encoder(const Block& block);

  /**
   * @brief Reserve @p size bytes for the underlying buffer
   * @param size amount of bytes to reserve in the underlying buffer
   * @param addInFront if true, then @p size bytes will be available in front (i.e., subsequent call
   *        to prepend* will not need to allocate memory).  If false, then reservation will be done
   *        at the end of the buffer (i.d., for subsequent append* calls)
   * @note Reserve size is exact, unlike reserveFront and reserveBack methods
   * @sa reserveFront, reserveBack
   */
  void
  reserve(size_t size, bool addInFront);

  /**
   * @brief Reserve at least @p size bytes at the back of the underlying buffer
   * @note the actual reserve size can be (and in most cases is) larger than specified reservation
   *       length
   */
  void
  reserveBack(size_t size);

  /**
   * @brief Reserve at least @p isze bytes at the beginning of the underlying buffer
   * @note the actual reserve size can be (and in most cases is) larger than specified reservation
   *       length
   */
  void
  reserveFront(size_t size);

  /**
   * @brief Get size of the underlying buffer
   */
  size_t
  capacity() const;

  /**
   * @brief Get underlying buffer
   */
  shared_ptr<Buffer>
  getBuffer();

public: // accessors

  /**
   * @brief Get an iterator pointing to the first byte of the encoded buffer
   */
  iterator
  begin();

  /**
   * @brief Get an iterator pointing to the past-the-end byte of the encoded buffer
   */
  iterator
  end();

  /**
   * @brief Get an iterator pointing to the first byte of the encoded buffer
   */
  const_iterator
  begin() const;

  const_iterator
  end() const;

  /**
   * @brief Get a pointer to the first byte of the encoded buffer
   */
  uint8_t*
  buf();

  /**
   * @brief Get a pointer to the first byte of the encoded buffer
   */
  const uint8_t*
  buf() const;

  /**
   * @brief Get size of the encoded buffer
   */
  size_t
  size() const;

  /**
   * @brief Create Block from the underlying buffer
   *
   * @param verifyLength If this parameter set to true, Block's constructor
   *                     will be requested to verify consistency of the encoded
   *                     length in the Block, otherwise ignored
   */
  Block
  block(bool verifyLength = true) const;

private:
  shared_ptr<Buffer> m_buffer;

  // invariant: m_begin always points to the position of last-written byte (if prepending data)
  iterator m_begin;
  // invariant: m_end always points to the position of next unwritten byte (if appending data)
  iterator m_end;
};


inline size_t
Encoder::size() const
{
  return m_end - m_begin;
}

inline shared_ptr<Buffer>
Encoder::getBuffer()
{
  return m_buffer;
}

inline size_t
Encoder::capacity() const
{
  return m_buffer->size();
}

inline Buffer::iterator
Encoder::begin()
{
  return m_begin;
}

inline Buffer::iterator
Encoder::end()
{
  return m_end;
}

inline Buffer::const_iterator
Encoder::begin() const
{
  return m_begin;
}

inline Buffer::const_iterator
Encoder::end() const
{
  return m_end;
}

inline uint8_t*
Encoder::buf()
{
  return &(*m_begin);
}

inline const uint8_t*
Encoder::buf() const
{
  return &(*m_begin);
}

template<class Iterator>
inline size_t
Encoder::prependRange(Iterator first, Iterator last)
{
  using ValueType = typename std::iterator_traits<Iterator>::value_type;
  static_assert(sizeof(ValueType) == 1 && !std::is_same<ValueType, bool>::value, "");

  size_t length = std::distance(first, last);
  reserveFront(length);

  m_begin -= length;
  std::copy(first, last, m_begin);
  return length;
}


template<class Iterator>
inline size_t
Encoder::appendRange(Iterator first, Iterator last)
{
  using ValueType = typename std::iterator_traits<Iterator>::value_type;
  static_assert(sizeof(ValueType) == 1 && !std::is_same<ValueType, bool>::value, "");

  size_t length = std::distance(first, last);
  reserveBack(length);

  std::copy(first, last, m_end);
  m_end += length;
  return length;
}


} // namespace encoding
} // namespace ndn

#endif // NDN_ENCODING_ENCODER_HPP

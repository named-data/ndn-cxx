/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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
 * @author Wentao Shang <http://irl.cs.ucla.edu/~wentao/>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#ifndef NDN_ENCODING_ENCODING_BUFFER_HPP
#define NDN_ENCODING_ENCODING_BUFFER_HPP

#include "../common.hpp"

#include "buffer.hpp"
#include "block.hpp"

namespace ndn {

namespace encoding {
static const bool Buffer = true;
static const bool Estimator = false;
} // encoding

template<bool isRealEncoderNotEstimator>
class EncodingImpl;

typedef EncodingImpl<encoding::Buffer> EncodingBuffer;
typedef EncodingImpl<encoding::Estimator> EncodingEstimator;

/**
 * @brief Class representing wire element of the NDN packet
 */
template<>
class EncodingImpl<encoding::Buffer>
{
public:
  /**
   * @brief Constructor to create a EncodingImpl with specified reserved sizes
   *
   * The caller should make sure that that reserveFromBack does not exceed totalReserve,
   * otherwise behavior is undefined.
   */
  EncodingImpl(size_t totalReserve = 8800,
               size_t reserveFromBack = 400)
    : m_buffer(new Buffer(totalReserve))
  {
    m_begin = m_end = m_buffer->end() - (reserveFromBack < totalReserve ? reserveFromBack : 0);
  }

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
  EncodingImpl(const Block& block)
    : m_buffer(const_pointer_cast<Buffer>(block.m_buffer))
    , m_begin(m_buffer->begin() + (block.begin() - m_buffer->begin()))
    , m_end(m_buffer->begin()   + (block.end()   - m_buffer->begin()))
  {
  }

  inline size_t
  size() const;

  inline size_t
  capacity() const;

  inline uint8_t*
  buf();

  inline const uint8_t*
  buf() const;

  /**
   * @brief Create Block from the underlying EncodingBuffer
   *
   * @param verifyLength If this parameter set to true, Block's constructor
   *                     will be requested to verify consistency of the encoded
   *                     length in the Block, otherwise ignored
   */
  inline Block
  block(bool verifyLength = true) const;

  inline void
  resize(size_t size, bool addInFront);

  inline Buffer::iterator
  begin();

  inline Buffer::iterator
  end();

  inline Buffer::const_iterator
  begin() const;

  inline Buffer::const_iterator
  end() const;

  inline size_t
  prependByte(uint8_t value);

  inline size_t
  prependByteArray(const uint8_t* array, size_t length);

  inline size_t
  prependNonNegativeInteger(uint64_t varNumber);

  inline size_t
  prependVarNumber(uint64_t varNumber);

  inline size_t
  prependBlock(const Block& block);

  inline size_t
  appendByte(uint8_t value);

  inline size_t
  appendByteArray(const uint8_t* array, size_t length);

  inline size_t
  appendNonNegativeInteger(uint64_t varNumber);

  inline size_t
  appendVarNumber(uint64_t varNumber);

  // inline void
  // removeByteFromFront();

  // inline void
  // removeByteFromEnd();

  // inline void
  // removeVarNumberFromFront(uint64_t varNumber);

  // inline void
  // removeVarNumberFromBack(uint64_t varNumber);

private:
  BufferPtr m_buffer;

  // invariant: m_begin always points to the position of last-written byte (if prepending data)
  Buffer::iterator m_begin;
  // invariant: m_end always points to the position of next unwritten byte (if appending data)
  Buffer::iterator m_end;

  friend class Block;
};


/**
 * @brief Class representing wire element of the NDN packet
 */
template<>
class EncodingImpl<encoding::Estimator>
{
public:
  EncodingImpl(size_t totalReserve = 8800,
               size_t reserveFromBack = 400)
  {
  }

  inline size_t
  prependByte(uint8_t value);

  inline size_t
  prependByteArray(const uint8_t* array, size_t length);

  inline size_t
  prependNonNegativeInteger(uint64_t varNumber);

  inline size_t
  prependVarNumber(uint64_t varNumber);

  inline size_t
  prependBlock(const Block& block);

  inline size_t
  appendByte(uint8_t value);

  inline size_t
  appendByteArray(const uint8_t* array, size_t length);

  inline size_t
  appendNonNegativeInteger(uint64_t varNumber);

  inline size_t
  appendVarNumber(uint64_t varNumber);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// helper methods

template<bool P>
inline size_t
prependNonNegativeIntegerBlock(EncodingImpl<P>& encoder, uint32_t type, uint64_t number)
{
  size_t valueLength = encoder.prependNonNegativeInteger(number);
  size_t totalLength = valueLength;
  totalLength += encoder.prependVarNumber(valueLength);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}

template<bool P>
inline size_t
prependByteArrayBlock(EncodingImpl<P>& encoder, uint32_t type,
                      const uint8_t* array, size_t arraySize)
{
  size_t valueLength = encoder.prependByteArray(array, arraySize);
  size_t totalLength = valueLength;
  totalLength += encoder.prependVarNumber(valueLength);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}

template<bool P>
inline size_t
prependBooleanBlock(EncodingImpl<P>& encoder, uint32_t type)
{
  size_t totalLength = encoder.prependVarNumber(0);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}


template<bool P, class U>
inline size_t
prependNestedBlock(EncodingImpl<P>& encoder, uint32_t type, const U& nestedBlock)
{
  size_t valueLength = nestedBlock.wireEncode(encoder);
  size_t totalLength = valueLength;
  totalLength += encoder.prependVarNumber(valueLength);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}

template<bool P>
inline size_t
prependBlock(EncodingImpl<P>& encoder, const Block& block)
{
  return encoder.prependByteArray(block.wire(), block.size());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline size_t
EncodingImpl<encoding::Buffer>::size() const
{
  return m_end - m_begin;
}

inline size_t
EncodingImpl<encoding::Buffer>::capacity() const
{
  return m_buffer->size();
}

inline uint8_t*
EncodingImpl<encoding::Buffer>::buf()
{
  return &(*m_begin);
}

inline const uint8_t*
EncodingImpl<encoding::Buffer>::buf() const
{
  return &(*m_begin);
}

inline Block
EncodingImpl<encoding::Buffer>::block(bool verifyLength/* = true*/) const
{
  return Block(m_buffer,
               m_begin, m_end,
               verifyLength);
}

inline void
EncodingImpl<encoding::Buffer>::resize(size_t size, bool addInFront)
{
  if (addInFront)
    {
      size_t diff_end = m_buffer->end() - m_end;
      size_t diff_begin = m_buffer->end() - m_begin;

      Buffer* buf = new Buffer(size);
      std::copy_backward(m_buffer->begin(), m_buffer->end(), buf->end());

      m_buffer.reset(buf);

      m_end = m_buffer->end() - diff_end;
      m_begin = m_buffer->end() - diff_begin;
    }
  else
    {
      size_t diff_end = m_end - m_buffer->begin();
      size_t diff_begin = m_begin - m_buffer->begin();

      Buffer* buf = new Buffer(size);
      std::copy(m_buffer->begin(), m_buffer->end(), buf->begin());

      m_buffer.reset(buf);

      m_end = m_buffer->begin() + diff_end;
      m_begin = m_buffer->begin() + diff_begin;
    }
}

inline Buffer::iterator
EncodingImpl<encoding::Buffer>::begin()
{
  return m_begin;
}

inline Buffer::iterator
EncodingImpl<encoding::Buffer>::end()
{
  return m_end;
}

inline Buffer::const_iterator
EncodingImpl<encoding::Buffer>::begin() const
{
  return m_begin;
}

inline Buffer::const_iterator
EncodingImpl<encoding::Buffer>::end() const
{
  return m_end;
}


//////////////////////////////////////////////////////////
// Prepend to the back of the buffer. Resize if needed. //
//////////////////////////////////////////////////////////

inline size_t
EncodingImpl<encoding::Buffer>::prependByte(uint8_t value)
{
  if (m_begin == m_buffer->begin())
    resize(m_buffer->size() * 2, true);

  m_begin--;
  *m_begin = value;
  return 1;
}

inline size_t
EncodingImpl<encoding::Estimator>::prependByte(uint8_t value)
{
  return 1;
}

inline size_t
EncodingImpl<encoding::Buffer>::prependByteArray(const uint8_t* array, size_t length)
{
  if ((m_buffer->begin() + length) > m_begin)
    resize(m_buffer->size() * 2 + length, true);

  m_begin -= length;
  std::copy(array, array + length, m_begin);
  return length;
}

inline size_t
EncodingImpl<encoding::Estimator>::prependByteArray(const uint8_t* array, size_t length)
{
  return length;
}

inline size_t
EncodingImpl<encoding::Buffer>::prependNonNegativeInteger(uint64_t varNumber)
{
  if (varNumber <= std::numeric_limits<uint8_t>::max()) {
    return prependByte(static_cast<uint8_t>(varNumber));
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    uint16_t value = htobe16(static_cast<uint16_t>(varNumber));
    return prependByteArray(reinterpret_cast<const uint8_t*>(&value), 2);
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    uint32_t value = htobe32(static_cast<uint32_t>(varNumber));
    return prependByteArray(reinterpret_cast<const uint8_t*>(&value), 4);
  }
  else {
    uint64_t value = htobe64(varNumber);
    return prependByteArray(reinterpret_cast<const uint8_t*>(&value), 8);
  }
}

inline size_t
EncodingImpl<encoding::Estimator>::prependNonNegativeInteger(uint64_t varNumber)
{
  if (varNumber <= std::numeric_limits<uint8_t>::max()) {
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    return 2;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    return 4;
  }
  else {
    return 8;
  }
}

inline size_t
EncodingImpl<encoding::Buffer>::prependVarNumber(uint64_t varNumber)
{
  if (varNumber < 253) {
    prependByte(static_cast<uint8_t>(varNumber));
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    uint16_t value = htobe16(static_cast<uint16_t>(varNumber));
    prependByteArray(reinterpret_cast<const uint8_t*>(&value), 2);
    prependByte(253);
    return 3;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    uint32_t value = htobe32(static_cast<uint32_t>(varNumber));
    prependByteArray(reinterpret_cast<const uint8_t*>(&value), 4);
    prependByte(254);
    return 5;
  }
  else {
    uint64_t value = htobe64(varNumber);
    prependByteArray(reinterpret_cast<const uint8_t*>(&value), 8);
    prependByte(255);
    return 9;
  }
}

inline size_t
EncodingImpl<encoding::Estimator>::prependVarNumber(uint64_t varNumber)
{
  if (varNumber < 253) {
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    return 3;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    return 5;
  }
  else {
    return 9;
  }
}

inline size_t
EncodingImpl<encoding::Buffer>::prependBlock(const Block& block)
{
  if (block.hasWire()) {
    return prependByteArray(block.wire(), block.size());
  }
  else {
    return prependByteArrayBlock(*this, block.type(), block.value(), block.value_size());
  }
}

inline size_t
EncodingImpl<encoding::Estimator>::prependBlock(const Block& block)
{
  if (block.hasWire()) {
    return block.size();
  }
  else {
    return prependByteArrayBlock(*this, block.type(), block.value(), block.value_size());
  }
}

/////////////////////////////////////////////////////////
// Append to the back of the buffer. Resize if needed. //
/////////////////////////////////////////////////////////

inline size_t
EncodingImpl<encoding::Buffer>::appendByte(uint8_t value)
{
  if (m_end == m_buffer->end())
    resize(m_buffer->size() * 2, false);

  *m_end = value;
  m_end++;
  return 1;
}

inline size_t
EncodingImpl<encoding::Estimator>::appendByte(uint8_t value)
{
  return 1;
}

inline size_t
EncodingImpl<encoding::Buffer>::appendByteArray(const uint8_t* array, size_t length)
{
  if ((m_end + length) > m_buffer->end())
    resize(m_buffer->size() * 2 + length, false);

  std::copy(array, array + length, m_end);
  m_end += length;
  return length;
}

inline size_t
EncodingImpl<encoding::Estimator>::appendByteArray(const uint8_t* array, size_t length)
{
  return prependByteArray(array, length);
}

inline size_t
EncodingImpl<encoding::Buffer>::appendNonNegativeInteger(uint64_t varNumber)
{
  if (varNumber <= std::numeric_limits<uint8_t>::max()) {
    return appendByte(static_cast<uint8_t>(varNumber));
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    uint16_t value = htobe16(static_cast<uint16_t>(varNumber));
    return appendByteArray(reinterpret_cast<const uint8_t*>(&value), 2);
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    uint32_t value = htobe32(static_cast<uint32_t>(varNumber));
    return appendByteArray(reinterpret_cast<const uint8_t*>(&value), 4);
  }
  else {
    uint64_t value = htobe64(varNumber);
    return appendByteArray(reinterpret_cast<const uint8_t*>(&value), 8);
  }
}

inline size_t
EncodingImpl<encoding::Estimator>::appendNonNegativeInteger(uint64_t varNumber)
{
  return prependNonNegativeInteger(varNumber);
}

inline size_t
EncodingImpl<encoding::Buffer>::appendVarNumber(uint64_t varNumber)
{
  if (varNumber < 253) {
    appendByte(static_cast<uint8_t>(varNumber));
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    appendByte(253);
    uint16_t value = htobe16(static_cast<uint16_t>(varNumber));
    appendByteArray(reinterpret_cast<const uint8_t*>(&value), 2);
    return 3;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    appendByte(254);
    uint32_t value = htobe32(static_cast<uint32_t>(varNumber));
    appendByteArray(reinterpret_cast<const uint8_t*>(&value), 4);
    return 5;
  }
  else {
    appendByte(255);
    uint64_t value = htobe64(varNumber);
    appendByteArray(reinterpret_cast<const uint8_t*>(&value), 8);
    return 9;
  }
}

inline size_t
EncodingImpl<encoding::Estimator>::appendVarNumber(uint64_t varNumber)
{
  return prependVarNumber(varNumber);
}

} // ndn

#endif // NDN_ENCODING_ENCODING_BUFFER_HPP

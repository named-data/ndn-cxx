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

#include "encoder.hpp"

namespace ndn {
namespace encoding {

Encoder::Encoder(size_t totalReserve/* = 8800*/, size_t reserveFromBack/* = 400*/)
  : m_buffer(new Buffer(totalReserve))
{
  m_begin = m_end = m_buffer->end() - (reserveFromBack < totalReserve ? reserveFromBack : 0);
}


Encoder::Encoder(const Block& block)
  : m_buffer(const_pointer_cast<Buffer>(block.getBuffer()))
  , m_begin(m_buffer->begin() + (block.begin() - m_buffer->begin()))
  , m_end(m_buffer->begin()   + (block.end()   - m_buffer->begin()))
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
Encoder::reserveBack(size_t size)
{
  if ((m_end + size) > m_buffer->end())
    reserve(m_buffer->size() * 2 + size, false);
}

void
Encoder::reserveFront(size_t size)
{
  if ((m_buffer->begin() + size) > m_begin)
    reserve(m_buffer->size() * 2 + size, true);
}


Block
Encoder::block(bool verifyLength/* = true*/) const
{
  return Block(m_buffer,
               m_begin, m_end,
               verifyLength);
}

void
Encoder::reserve(size_t size, bool addInFront)
{
  if (size < m_buffer->size()) {
    size = m_buffer->size();
  }

  if (addInFront) {
    size_t diffEnd = m_buffer->end() - m_end;
    size_t diffBegin = m_buffer->end() - m_begin;

    Buffer* buf = new Buffer(size);
    std::copy_backward(m_buffer->begin(), m_buffer->end(), buf->end());

    m_buffer.reset(buf);

    m_end = m_buffer->end() - diffEnd;
    m_begin = m_buffer->end() - diffBegin;
  }
  else {
    size_t diffEnd = m_end - m_buffer->begin();
    size_t diffBegin = m_begin - m_buffer->begin();

    Buffer* buf = new Buffer(size);
    std::copy(m_buffer->begin(), m_buffer->end(), buf->begin());

    m_buffer.reset(buf);

    m_end = m_buffer->begin() + diffEnd;
    m_begin = m_buffer->begin() + diffBegin;
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

size_t
Encoder::prependByte(uint8_t value)
{
  reserveFront(1);

  m_begin--;
  *m_begin = value;
  return 1;
}

size_t
Encoder::appendByte(uint8_t value)
{
  reserveBack(1);

  *m_end = value;
  m_end++;
  return 1;
}


size_t
Encoder::prependByteArray(const uint8_t* array, size_t length)
{
  reserveFront(length);

  m_begin -= length;
  std::copy(array, array + length, m_begin);
  return length;
}

size_t
Encoder::appendByteArray(const uint8_t* array, size_t length)
{
  reserveBack(length);

  std::copy(array, array + length, m_end);
  m_end += length;
  return length;
}


size_t
Encoder::prependVarNumber(uint64_t varNumber)
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

size_t
Encoder::appendVarNumber(uint64_t varNumber)
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


size_t
Encoder::prependNonNegativeInteger(uint64_t varNumber)
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

size_t
Encoder::appendNonNegativeInteger(uint64_t varNumber)
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

size_t
Encoder::prependByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize)
{
  size_t totalLength = prependByteArray(array, arraySize);
  totalLength += prependVarNumber(arraySize);
  totalLength += prependVarNumber(type);

  return totalLength;
}

size_t
Encoder::appendByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize)
{
  size_t totalLength = appendVarNumber(type);
  totalLength += appendVarNumber(arraySize);
  totalLength += appendByteArray(array, arraySize);

  return totalLength;
}

size_t
Encoder::prependBlock(const Block& block)
{
  if (block.hasWire()) {
    return prependByteArray(block.wire(), block.size());
  }
  else {
    return prependByteArrayBlock(block.type(), block.value(), block.value_size());
  }
}

size_t
Encoder::appendBlock(const Block& block)
{
  if (block.hasWire()) {
    return appendByteArray(block.wire(), block.size());
  }
  else {
    return appendByteArrayBlock(block.type(), block.value(), block.value_size());
  }
}

} // namespace encoding
} // namespace ndn

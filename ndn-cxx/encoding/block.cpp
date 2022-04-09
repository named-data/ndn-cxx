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

#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/encoding/encoding-buffer.hpp"
#include "ndn-cxx/encoding/tlv.hpp"
#include "ndn-cxx/security/transform.hpp"
#include "ndn-cxx/util/ostream-joiner.hpp"
#include "ndn-cxx/util/string-helper.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <cstring>

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Block>));

const size_t MAX_SIZE_OF_BLOCK_FROM_STREAM = MAX_NDN_PACKET_SIZE;

// ---- constructor, creation, assignment ----

Block::Block() = default;

Block::Block(const Block&) = default;

Block&
Block::operator=(const Block&) = default;

Block::Block(span<const uint8_t> buffer)
{
  auto pos = buffer.begin();
  const auto end = buffer.end();

  m_type = tlv::readType(pos, end);
  uint64_t length = tlv::readVarNumber(pos, end);
  // pos now points to TLV-VALUE

  BOOST_ASSERT(pos <= end);
  if (length > static_cast<uint64_t>(std::distance(pos, end))) {
    NDN_THROW(Error("Not enough bytes in the buffer to fully parse TLV"));
  }
  std::advance(pos, length);
  // pos now points to the end of the TLV

  m_buffer = std::make_shared<Buffer>(buffer.begin(), pos);
  m_begin = m_buffer->begin();
  m_end = m_buffer->end();
  m_valueBegin = std::prev(m_end, length);
  m_valueEnd = m_buffer->end();
  m_size = m_buffer->size();
}

Block::Block(const EncodingBuffer& buffer)
  : Block(buffer.getBuffer(), buffer.begin(), buffer.end(), true)
{
}

Block::Block(const ConstBufferPtr& buffer)
  : Block(buffer, buffer->begin(), buffer->end(), true)
{
}

Block::Block(ConstBufferPtr buffer, Buffer::const_iterator begin, Buffer::const_iterator end,
             bool verifyLength)
  : m_buffer(std::move(buffer))
  , m_begin(begin)
  , m_end(end)
  , m_valueBegin(m_begin)
  , m_valueEnd(m_end)
  , m_size(static_cast<size_t>(std::distance(m_begin, m_end)))
{
  if (m_buffer->empty()) {
    NDN_THROW(std::invalid_argument("Buffer is empty"));
  }

  const uint8_t* bufferBegin = &m_buffer->front();
  const uint8_t* bufferEnd = bufferBegin + m_buffer->size();
  if (&*begin < bufferBegin || &*begin > bufferEnd ||
      &*end   < bufferBegin || &*end   > bufferEnd) {
    NDN_THROW(std::invalid_argument("Begin/end iterators point outside the buffer"));
  }

  m_type = tlv::readType(m_valueBegin, m_valueEnd);
  uint64_t length = tlv::readVarNumber(m_valueBegin, m_valueEnd);
  // m_valueBegin now points to TLV-VALUE

  if (verifyLength && length != static_cast<uint64_t>(m_valueEnd - m_valueBegin)) {
    NDN_THROW(Error("TLV-LENGTH does not match buffer size"));
  }
}

Block::Block(const Block& block, Block::const_iterator begin, Block::const_iterator end,
             bool verifyLength)
  : Block(block.m_buffer, begin, end, verifyLength)
{
}

Block::Block(ConstBufferPtr buffer, uint32_t type,
             Buffer::const_iterator begin, Buffer::const_iterator end,
             Buffer::const_iterator valueBegin, Buffer::const_iterator valueEnd)
  : m_buffer(std::move(buffer))
  , m_begin(begin)
  , m_end(end)
  , m_valueBegin(valueBegin)
  , m_valueEnd(valueEnd)
  , m_type(type)
  , m_size(static_cast<size_t>(std::distance(m_begin, m_end)))
{
}

Block::Block(uint32_t type)
  : m_type(type)
  , m_size(tlv::sizeOfVarNumber(m_type) + tlv::sizeOfVarNumber(0))
{
}

Block::Block(uint32_t type, ConstBufferPtr value)
  : m_buffer(std::move(value))
  , m_begin(m_buffer->end())
  , m_end(m_buffer->end())
  , m_valueBegin(m_buffer->begin())
  , m_valueEnd(m_buffer->end())
  , m_type(type)
{
  m_size = tlv::sizeOfVarNumber(m_type) + tlv::sizeOfVarNumber(value_size()) + value_size();
}

Block::Block(uint32_t type, const Block& value)
  : m_buffer(value.m_buffer)
  , m_begin(m_buffer->end())
  , m_end(m_buffer->end())
  , m_valueBegin(value.begin())
  , m_valueEnd(value.end())
  , m_type(type)
{
  m_size = tlv::sizeOfVarNumber(m_type) + tlv::sizeOfVarNumber(value_size()) + value_size();
}

std::tuple<bool, Block>
Block::fromBuffer(ConstBufferPtr buffer, size_t offset)
{
  auto begin = std::next(buffer->begin(), offset);
  auto pos = begin;
  const auto end = buffer->end();

  uint32_t type = 0;
  bool isOk = tlv::readType(pos, end, type);
  if (!isOk) {
    return std::make_tuple(false, Block());
  }

  uint64_t length = 0;
  isOk = tlv::readVarNumber(pos, end, length);
  if (!isOk) {
    return std::make_tuple(false, Block());
  }
  // pos now points to TLV-VALUE

  BOOST_ASSERT(pos <= end);
  if (length > static_cast<uint64_t>(std::distance(pos, end))) {
    return std::make_tuple(false, Block());
  }

  return std::make_tuple(true, Block(std::move(buffer), type, begin, pos + length, pos, pos + length));
}

std::tuple<bool, Block>
Block::fromBuffer(span<const uint8_t> buffer)
{
  auto pos = buffer.begin();
  const auto end = buffer.end();

  uint32_t type = 0;
  bool isOk = tlv::readType(pos, end, type);
  if (!isOk) {
    return std::make_tuple(false, Block());
  }
  uint64_t length = 0;
  isOk = tlv::readVarNumber(pos, end, length);
  if (!isOk) {
    return std::make_tuple(false, Block());
  }
  // pos now points to TLV-VALUE

  BOOST_ASSERT(pos <= end);
  if (length > static_cast<uint64_t>(std::distance(pos, end))) {
    return std::make_tuple(false, Block());
  }
  std::advance(pos, length);
  // pos now points to the end of the TLV

  auto b = std::make_shared<Buffer>(buffer.begin(), pos);
  return std::make_tuple(true, Block(b, type, b->begin(), b->end(),
                                     std::prev(b->end(), length), b->end()));
}

Block
Block::fromStream(std::istream& is)
{
  std::istream_iterator<uint8_t> begin(is >> std::noskipws);
  std::istream_iterator<uint8_t> end;

  uint32_t type = tlv::readType(begin, end);
  uint64_t length = tlv::readVarNumber(begin, end);
  if (begin != end) {
    is.putback(*begin);
  }

  size_t tlSize = tlv::sizeOfVarNumber(type) + tlv::sizeOfVarNumber(length);
  if (tlSize + length > MAX_SIZE_OF_BLOCK_FROM_STREAM) {
    NDN_THROW(Error("TLV-LENGTH from stream exceeds limit"));
  }

  EncodingBuffer eb(tlSize + length, length);
  uint8_t* valueBuf = eb.data();
  is.read(reinterpret_cast<char*>(valueBuf), length);
  if (length != static_cast<uint64_t>(is.gcount())) {
    NDN_THROW(Error("Not enough bytes from stream to fully parse TLV"));
  }

  eb.prependVarNumber(length);
  eb.prependVarNumber(type);

  // TLV-VALUE is directly written into eb.buf(), eb.end() is not incremented, but eb.getBuffer()
  // has the correct layout.
  return Block(eb.getBuffer());
}

// ---- wire format ----

void
Block::reset() noexcept
{
  *this = {};
}

void
Block::resetWire() noexcept
{
  m_buffer.reset(); // discard underlying buffer by resetting shared_ptr
  m_begin = m_end = m_valueBegin = m_valueEnd = {};
}

Block::const_iterator
Block::begin() const
{
  if (!hasWire())
    NDN_THROW(Error("Underlying wire buffer is empty"));

  return m_begin;
}

Block::const_iterator
Block::end() const
{
  if (!hasWire())
    NDN_THROW(Error("Underlying wire buffer is empty"));

  return m_end;
}

const uint8_t*
Block::data() const
{
  if (!hasWire())
    NDN_THROW(Error("Underlying wire buffer is empty"));

  return &*m_begin;
}

size_t
Block::size() const
{
  if (!isValid()) {
    NDN_THROW(Error("Cannot determine size of invalid block"));
  }

  return m_size;
}

// ---- value ----

const uint8_t*
Block::value() const noexcept
{
  return value_size() > 0 ? &*m_valueBegin : nullptr;
}

Block
Block::blockFromValue() const
{
  if (value_size() == 0) {
    NDN_THROW(Error("Cannot construct block from empty TLV-VALUE"));
  }

  return Block(*this, m_valueBegin, m_valueEnd, true);
}

// ---- sub elements ----

void
Block::parse() const
{
  if (!m_elements.empty() || value_size() == 0)
    return;

  auto begin = value_begin();
  auto end = value_end();

  while (begin != end) {
    auto pos = begin;
    uint32_t type = tlv::readType(pos, end);
    uint64_t length = tlv::readVarNumber(pos, end);
    if (length > static_cast<uint64_t>(end - pos)) {
      m_elements.clear();
      NDN_THROW(Error("TLV-LENGTH of sub-element of type " + to_string(type) +
                      " exceeds TLV-VALUE boundary of parent block"));
    }
    // pos now points to TLV-VALUE of sub element

    auto subEnd = std::next(pos, length);
    m_elements.emplace_back(m_buffer, type, begin, subEnd, pos, subEnd);

    begin = subEnd;
  }
}

void
Block::encode()
{
  if (hasWire())
    return;

  EncodingEstimator estimator;
  size_t estimatedSize = encode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  encode(buffer);
}

size_t
Block::encode(EncodingEstimator& estimator) const
{
  if (hasValue()) {
    return m_size;
  }

  size_t len = encodeValue(estimator);
  len += estimator.prependVarNumber(len);
  len += estimator.prependVarNumber(m_type);
  return len;
}

size_t
Block::encodeValue(EncodingEstimator& estimator) const
{
  size_t len = 0;
  for (const Block& element : m_elements | boost::adaptors::reversed) {
    len += element.encode(estimator);
  }
  return len;
}

size_t
Block::encode(EncodingBuffer& encoder)
{
  size_t len = 0;
  m_end = encoder.begin();
  if (hasValue()) {
    len += encoder.prependRange(m_valueBegin, m_valueEnd);
  }
  else {
    for (Block& element : m_elements | boost::adaptors::reversed) {
      len += element.encode(encoder);
    }
  }
  m_valueEnd = m_end;
  m_valueBegin = encoder.begin();

  len += encoder.prependVarNumber(len);
  len += encoder.prependVarNumber(m_type);
  m_begin = encoder.begin();

  m_buffer = encoder.getBuffer();
  m_size = len;
  return len;
}

const Block&
Block::get(uint32_t type) const
{
  auto it = this->find(type);
  if (it != m_elements.end()) {
    return *it;
  }

  NDN_THROW(Error("No sub-element of type " + to_string(type) +
                  " found in block of type " + to_string(m_type)));
}

Block::element_const_iterator
Block::find(uint32_t type) const
{
  return std::find_if(m_elements.begin(), m_elements.end(),
                      [type] (const Block& subBlock) { return subBlock.type() == type; });
}

void
Block::remove(uint32_t type)
{
  resetWire();

  auto it = std::remove_if(m_elements.begin(), m_elements.end(),
                           [type] (const Block& subBlock) { return subBlock.type() == type; });
  m_elements.erase(it, m_elements.end());
}

Block::element_iterator
Block::erase(Block::element_const_iterator position)
{
  resetWire();
  return m_elements.erase(position);
}

Block::element_iterator
Block::erase(Block::element_const_iterator first, Block::element_const_iterator last)
{
  resetWire();
  return m_elements.erase(first, last);
}

void
Block::push_back(const Block& element)
{
  resetWire();
  m_elements.push_back(element);
}

void
Block::push_back(Block&& element)
{
  resetWire();
  m_elements.push_back(std::move(element));
}

Block::element_iterator
Block::insert(Block::element_const_iterator pos, const Block& element)
{
  resetWire();
  return m_elements.insert(pos, element);
}

// ---- misc ----

Block::operator boost::asio::const_buffer() const
{
  return {data(), size()};
}

bool
operator==(const Block& lhs, const Block& rhs)
{
  return lhs.type() == rhs.type() &&
         lhs.value_size() == rhs.value_size() &&
         (lhs.value_size() == 0 ||
          std::memcmp(lhs.value(), rhs.value(), lhs.value_size()) == 0);
}

std::ostream&
operator<<(std::ostream& os, const Block& block)
{
  auto oldFmt = os.flags(std::ios_base::dec);

  if (!block.isValid()) {
    os << "[invalid]";
  }
  else if (!block.m_elements.empty()) {
    EncodingEstimator estimator;
    size_t tlvLength = block.encodeValue(estimator);
    os << block.type() << '[' << tlvLength << "]={";
    std::copy(block.elements_begin(), block.elements_end(), make_ostream_joiner(os, ','));
    os << '}';
  }
  else if (block.value_size() > 0) {
    os << block.type() << '[' << block.value_size() << "]=";
    printHex(os, block.value_bytes(), true);
  }
  else {
    os << block.type() << "[empty]";
  }

  os.flags(oldFmt);
  return os;
}

Block
operator ""_block(const char* input, std::size_t len)
{
  namespace t = security::transform;
  t::StepSource ss;
  OBufferStream os;
  ss >> t::hexDecode() >> t::streamSink(os);

  for (const char* end = input + len; input != end; ++input) {
    if (std::strchr("0123456789ABCDEF", *input) != nullptr) {
      ss.write({reinterpret_cast<const uint8_t*>(input), 1});
    }
  }

  try {
    ss.end();
  }
  catch (const t::Error&) {
    NDN_THROW(std::invalid_argument("Input has odd number of hexadecimal digits"));
  }

  return Block(os.buf());
}

} // namespace ndn

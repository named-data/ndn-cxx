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
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#ifndef NDN_ENCODING_BLOCK_HPP
#define NDN_ENCODING_BLOCK_HPP

#include "../common.hpp"

#include "buffer.hpp"
#include "tlv.hpp"
#include "encoding-buffer-fwd.hpp"

namespace boost {
namespace asio {
class const_buffer;
} // namespace asio
} // namespace boost

namespace ndn {

/** @brief Class representing a wire element of NDN-TLV packet format
 */
class Block
{
public:
  typedef std::vector<Block>                 element_container;
  typedef element_container::iterator        element_iterator;
  typedef element_container::const_iterator  element_const_iterator;

  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

public: // constructor, creation, assignment
  /** @brief Create an empty Block
   */
  Block();

  /** @brief Create a Block based on EncodingBuffer object
   */
  explicit
  Block(const EncodingBuffer& buffer);

  /** @brief Create a Block from the raw buffer with Type-Length parsing
   */
  explicit
  Block(const ConstBufferPtr& buffer);

  /** @brief Create a Block from a buffer, directly specifying boundaries
   *         of the block within the buffer
   *
   *  This overload will automatically detect type and position of the value within the block
   */
  Block(const ConstBufferPtr& buffer,
        const Buffer::const_iterator& begin, const Buffer::const_iterator& end,
        bool verifyLength = true);

  /** @brief Create a Block from existing block (reusing the underlying buffer), directly
   *         specifying boundaries of the block within the buffer
   *
   *  This overload will automatically detect type and position of the value within the block
   */
  Block(const Block& block,
        const Buffer::const_iterator& begin, const Buffer::const_iterator& end,
        bool verifyLength = true);

  /** @brief Create a Block from the raw buffer with Type-Length parsing
   */
  Block(const uint8_t* buffer, size_t maxlength);

  /** @brief Create a Block from the raw buffer with Type-Length parsing
   */
  Block(const void* buffer, size_t maxlength);

  /** @brief Create a Block from the wire buffer (no parsing)
   *
   *  This overload does not do any parsing
   */
  Block(const ConstBufferPtr& wire,
        uint32_t type,
        const Buffer::const_iterator& begin, const Buffer::const_iterator& end,
        const Buffer::const_iterator& valueBegin, const Buffer::const_iterator& valueEnd);

  /** @brief Create Block of a specific type with empty wire buffer
   */
  explicit
  Block(uint32_t type);

  /** @brief Create a Block of a specific type with the specified value
   *
   *  The underlying buffer holds only value Additional operations are needed
   *  to construct wire encoding, one need to prepend the wire buffer with type
   *  and value-length VAR-NUMBERs
   */
  Block(uint32_t type, const ConstBufferPtr& value);

  /** @brief Create a nested Block of a specific type with the specified value
   *
   *  The underlying buffer holds only value. Additional operations are needed
   *  to construct wire encoding, one need to prepend the wire buffer with type
   *  and value-length VAR-NUMBERs
   */
  Block(uint32_t type, const Block& value);

  /** @brief Create a Block from an input stream
   */
  static Block
  fromStream(std::istream& is);

  /** @brief Try to construct block from Buffer
   *  @param buffer the buffer to construct block from
   *  @note buffer is passed by value because the constructed block
   *        takes shared ownership of the buffer
   *  @param offset offset from beginning of \p buffer to construct Block from
   *
   *  This method does not throw upon decoding error.
   *  This method does not copy the bytes.
   *
   *  @return true and the Block, if Block is successfully created; otherwise false
   */
  static std::tuple<bool, Block>
  fromBuffer(ConstBufferPtr buffer, size_t offset);

  /** @deprecated use fromBuffer(ConstBufferPtr, size_t)
   */
  DEPRECATED(
  static bool
  fromBuffer(const ConstBufferPtr& buffer, size_t offset, Block& block))
  {
    bool isOk = false;
    std::tie(isOk, block) = Block::fromBuffer(buffer, offset);
    return isOk;
  }

  /** @brief Try to construct block from raw buffer
   *  @param buffer the raw buffer to copy bytes from
   *  @param maxSize the maximum size of constructed block;
   *                 @p buffer must have a size of at least @p maxSize
   *
   *  This method does not throw upon decoding error.
   *  This method copies the bytes into a new Buffer.
   *
   *  @return true and the Block, if Block is successfully created; otherwise false
   */
  static std::tuple<bool, Block>
  fromBuffer(const uint8_t* buffer, size_t maxSize);

  /** @deprecated use fromBuffer(const uint8_t*, size_t)
   */
  DEPRECATED(
  static bool
  fromBuffer(const uint8_t* buffer, size_t maxSize, Block& block))
  {
    bool isOk = false;
    std::tie(isOk, block) = Block::fromBuffer(buffer, maxSize);
    return isOk;
  }

public: // wire format
  /** @brief Check if the Block is empty
   */
  bool
  empty() const;

  /** @brief Check if the Block has fully encoded wire
   */
  bool
  hasWire() const;

  /** @brief Reset wire buffer of the element
   */
  void
  reset();

  /** @brief Reset wire buffer but keep sub elements (if any)
   */
  void
  resetWire();

  Buffer::const_iterator
  begin() const;

  Buffer::const_iterator
  end() const;

  const uint8_t*
  wire() const;

  size_t
  size() const;

public: // type and value
  uint32_t
  type() const;

  /** @brief Check if the Block has value block (no type and length are encoded)
   */
  bool
  hasValue() const;

  Buffer::const_iterator
  value_begin() const;

  Buffer::const_iterator
  value_end() const;

  const uint8_t*
  value() const;

  size_t
  value_size() const;

public: // sub elements
  /** @brief Parse wire buffer into subblocks
   *
   *  This method is not really const, but it does not modify any data.  It simply
   *  parses contents of the buffer into subblocks
   */
  void
  parse() const;

  /** @brief Encode subblocks into wire buffer
   */
  void
  encode();

  /** @brief Get the first subelement of the requested type
   */
  const Block&
  get(uint32_t type) const;

  element_const_iterator
  find(uint32_t type) const;

  /**
   * @brief remove all subelements of \p type
   * @param type TLV-TYPE of subelements to remove
   * @pre parse() has been invoked
   */
  void
  remove(uint32_t type);

  element_iterator
  erase(element_const_iterator position);

  element_iterator
  erase(element_const_iterator first, element_const_iterator last);

  void
  push_back(const Block& element);

  /**
   * @brief insert Insert a new element in a specific position
   * @param pos Position to insert the new element
   * @param element Element to be inserted
   * @return An iterator that points to the first of the newly inserted elements.
   */
  element_iterator
  insert(element_const_iterator pos, const Block& element);

  /** @brief Get all subelements
   */
  const element_container&
  elements() const;

  element_const_iterator
  elements_begin() const;

  element_const_iterator
  elements_end() const;

  size_t
  elements_size() const;

  Block
  blockFromValue() const;

  /**
   * @brief Get underlying buffer
   */
  shared_ptr<const Buffer>
  getBuffer() const;

public: // EqualityComparable concept
  bool
  operator==(const Block& other) const;

  bool
  operator!=(const Block& other) const;

public: // ConvertibleToConstBuffer
  operator boost::asio::const_buffer() const;

protected:
  shared_ptr<const Buffer> m_buffer;

  uint32_t m_type;

  Buffer::const_iterator m_begin;
  Buffer::const_iterator m_end;
  uint32_t m_size;

  Buffer::const_iterator m_value_begin;
  Buffer::const_iterator m_value_end;

  mutable element_container m_subBlocks;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline shared_ptr<const Buffer>
Block::getBuffer() const
{
  return m_buffer;
}

inline uint32_t
Block::type() const
{
  return m_type;
}

inline Buffer::const_iterator
Block::value_begin() const
{
  return m_value_begin;
}

inline Buffer::const_iterator
Block::value_end() const
{
  return m_value_end;
}

inline const Block::element_container&
Block::elements() const
{
  return m_subBlocks;
}

} // namespace ndn

#endif // NDN_ENCODING_BLOCK_HPP

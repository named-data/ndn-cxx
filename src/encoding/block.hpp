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
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#ifndef NDN_ENCODING_BLOCK_HPP
#define NDN_ENCODING_BLOCK_HPP

#include "../common.hpp"

#include "buffer.hpp"
#include "tlv.hpp"

namespace ndn {

template<bool> class EncodingImpl;
typedef EncodingImpl<true> EncodingBuffer;

/**
 * @brief Class representing wire element of the NDN packet
 */
class Block
{
public:
  typedef std::vector<Block>                 element_container;
  typedef element_container::iterator        element_iterator;
  typedef element_container::const_iterator  element_const_iterator;

  /// @brief Error that can be thrown from Block
  class Error : public Tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Tlv::Error(what)
    {
    }
  };

  /**
   * @brief Default constructor to create an empty Block
   */
  Block();

  /**
   * @brief Create block based on EncodingBuffer object
   */
  explicit
  Block(const EncodingBuffer& buffer);

  /**
   * @brief A helper version of a constructor to create Block from the raw buffer (type
   * and value-length parsing)
   *
   * This constructor provides ability of implicit conversion from ConstBufferPtr to Block
   */
  Block(const ConstBufferPtr& buffer);

  /**
   * @brief Another helper to create block from a buffer, directly specifying boundaries
   *        of the block within the buffer
   *
   * This version will automatically detect type and position of the value within the block
   */
  Block(const ConstBufferPtr& buffer,
        const Buffer::const_iterator& begin, const Buffer::const_iterator& end,
        bool verifyLength = true);

  /**
   * @brief A helper version of a constructor to create Block from the raw buffer (type
   * and value-length parsing)
   */
  Block(const uint8_t* buffer, size_t maxlength);

  Block(const void* buffer, size_t maxlength);

  /**
   * @brief Create Block from the wire buffer (no parsing)
   *
   * This version of the constructor does not do any parsing
   */
  Block(const ConstBufferPtr& wire,
        uint32_t type,
        const Buffer::const_iterator& begin, const Buffer::const_iterator& end,
        const Buffer::const_iterator& valueBegin, const Buffer::const_iterator& valueEnd);

  /**
   * @brief Create Block of a specific type with empty wire buffer
   */
  explicit
  Block(uint32_t type);

  /**
   * @brief Create Block of a specific type with the specified value
   *
   * The underlying buffer hold only value, additional operations are needed
   * to construct wire encoding, one need to prepend the wire buffer with type
   * and value-length VAR-NUMBERs
   */
  Block(uint32_t type, const ConstBufferPtr& value);

  /**
   * @brief Create nested Block of a specific type with the specified value
   *
   * The underlying buffer hold only value, additional operations are needed
   * to construct wire encoding, one need to prepend the wire buffer with type
   * and value-length VAR-NUMBERs
   */
  explicit
  Block(uint32_t type, const Block& value);

  /*
   * @brief A helper version of a constructor to create Block from the stream
   *
   * @deprecated Use Block::fromStream instead
   */
  explicit
  DEPRECATED(Block(std::istream& is))
  {
    *this = Block::fromStream(is);
  }

  /*
   * @brief Constructor Block from the stream
   */
  static Block
  fromStream(std::istream& is);

  /**
   * @brief Try to construct block from Buffer, referencing data block pointed by wire
   *
   * @throws This method never throws an exception
   *
   * @returns true if Block successfully created, false if block cannot be created
   */
  static bool
  fromBuffer(const ConstBufferPtr& wire, size_t offset, Block& block);

  /**
   * @brief Try to construct block from Buffer, referencing data block pointed by wire
   *
   * @throws This method never throws an exception
   *
   * @returns true if Block successfully created, false if block cannot be created
   */
  static bool
  fromBuffer(const uint8_t* buffer, size_t maxSize, Block& block);

  /**
   * @brief Check if the Block is empty
   */
  bool
  empty() const;

  /**
   * @brief Check if the Block has fully encoded wire
   */
  bool
  hasWire() const;

  /**
   * @brief Check if the Block has value block (no type and length are encoded)
   */
  bool
  hasValue() const;

  /**
   * @brief Reset wire buffer of the element
   */
  void
  reset();

  /**
   * @brief Reset wire buffer but keep sub elements (if any)
   */
  void
  resetWire();

  /**
   * @brief Parse wire buffer into subblocks
   *
   * This method is not really const, but it does not modify any data.  It simply
   * parses contents of the buffer into subblocks
   */
  void
  parse() const;

  /**
   * @brief Encode subblocks into wire buffer
   */
  void
  encode();

  uint32_t
  type() const;

  /**
   * @brief Get the first subelement of the requested type
   */
  const Block&
  get(uint32_t type) const;

  element_const_iterator
  find(uint32_t type) const;

  void
  remove(uint32_t type);

  element_iterator
  erase(element_iterator position);

  element_iterator
  erase(element_iterator first, element_iterator last);

  void
  push_back(const Block& element);

  Buffer::const_iterator
  begin() const;

  Buffer::const_iterator
  end() const;

  const uint8_t*
  wire() const;

  size_t
  size() const;

  Buffer::const_iterator
  value_begin() const;

  Buffer::const_iterator
  value_end() const;

  const uint8_t*
  value() const;

  size_t
  value_size() const;

  /**
   * @brief Get all subelements
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

public: // EqualityComparable concept
  bool
  operator==(const Block& other) const;

  bool
  operator!=(const Block& other) const;

protected:
  ConstBufferPtr m_buffer;

  uint32_t m_type;

  Buffer::const_iterator m_begin;
  Buffer::const_iterator m_end;
  uint32_t m_size;

  Buffer::const_iterator m_value_begin;
  Buffer::const_iterator m_value_end;

  mutable element_container m_subBlocks;
  friend class EncodingImpl<true>;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline bool
Block::empty() const
{
  return m_type == std::numeric_limits<uint32_t>::max();
}


inline bool
Block::hasWire() const
{
  return m_buffer && (m_begin != m_end);
}

inline bool
Block::hasValue() const
{
  return static_cast<bool>(m_buffer);
}

inline void
Block::reset()
{
  m_buffer.reset(); // reset of the shared_ptr
  m_subBlocks.clear(); // remove all parsed subelements

  m_type = std::numeric_limits<uint32_t>::max();
  m_begin = m_end = m_value_begin = m_value_end = Buffer::const_iterator();
}

inline void
Block::resetWire()
{
  m_buffer.reset(); // reset of the shared_ptr
  // keep subblocks

  // keep type
  m_begin = m_end = m_value_begin = m_value_end = Buffer::const_iterator();
}

inline uint32_t
Block::type() const
{
  return m_type;
}

inline Block::element_const_iterator
Block::find(uint32_t type) const
{
  for (element_const_iterator i = m_subBlocks.begin();
       i != m_subBlocks.end();
       i++)
    {
      if (i->type() == type)
        {
          return i;
        }
    }
  return m_subBlocks.end();
}

inline void
Block::remove(uint32_t type)
{
  resetWire();

  element_container newContainer;
  newContainer.reserve(m_subBlocks.size());
  for (element_iterator i = m_subBlocks.begin();
       i != m_subBlocks.end();
       ++i)
  {
      if (i->type() != type)
        newContainer.push_back(*i);
  }
  m_subBlocks.swap(newContainer);
}

inline Block::element_iterator
Block::erase(Block::element_iterator position)
{
  resetWire();
  return m_subBlocks.erase(position);
}

inline Block::element_iterator
Block::erase(Block::element_iterator first, Block::element_iterator last)
{
  resetWire();
  return m_subBlocks.erase(first, last);
}


inline void
Block::push_back(const Block& element)
{
  resetWire();
  m_subBlocks.push_back(element);
}

inline Buffer::const_iterator
Block::begin() const
{
  if (!hasWire())
      throw Error("Underlying wire buffer is empty");

  return m_begin;
}

inline Buffer::const_iterator
Block::end() const
{
  if (!hasWire())
      throw Error("Underlying wire buffer is empty");

  return m_end;
}

inline size_t
Block::size() const
{
  if (hasWire() || hasValue()) {
    return m_size;
  }
  else
    throw Error("Block size cannot be determined (undefined block size)");
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

inline const uint8_t*
Block::wire() const
{
  if (!hasWire())
      throw Error("(Block::wire) Underlying wire buffer is empty");

  return &*m_begin;
}

inline const uint8_t*
Block::value() const
{
  if (!hasValue())
    return 0;

  return &*m_value_begin;
}

inline size_t
Block::value_size() const
{
  if (!hasValue())
    return 0;

  return m_value_end - m_value_begin;
}

inline const Block::element_container&
Block::elements() const
{
  return m_subBlocks;
}

inline Block::element_const_iterator
Block::elements_begin() const
{
  return m_subBlocks.begin();
}

inline Block::element_const_iterator
Block::elements_end() const
{
  return m_subBlocks.end();
}

inline size_t
Block::elements_size() const
{
  return m_subBlocks.size();
}

inline bool
Block::operator==(const Block& other) const
{
  return (this->size() == other.size()) &&
         std::equal(this->begin(), this->end(), other.begin());
}

inline bool
Block::operator!=(const Block& other) const
{
  return !this->operator==(other);
}

} // ndn

#endif // NDN_ENCODING_BLOCK_HPP

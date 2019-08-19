/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/encoding/buffer.hpp"
#include "ndn-cxx/encoding/encoding-buffer-fwd.hpp"
#include "ndn-cxx/encoding/tlv.hpp"

namespace boost {
namespace asio {
class const_buffer;
} // namespace asio
} // namespace boost

namespace ndn {

/** @brief Represents a TLV element of NDN packet format
 *  @sa https://named-data.net/doc/NDN-packet-spec/current/
 */
class Block
{
public:
  using element_container      = std::vector<Block>;
  using element_iterator       = element_container::iterator;
  using element_const_iterator = element_container::const_iterator;

  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

public: // construction, assignment
  /** @brief Create an invalid Block
   *  @post `isValid() == false`
   */
  Block();

  /** @brief Copy constructor
   */
  Block(const Block&);

  /** @brief Copy assignment operator
   */
  Block&
  operator=(const Block&);

  /** @brief Move constructor
   */
  Block(Block&&) noexcept;

  /** @brief Move assignment operator
   */
  Block&
  operator=(Block&&) noexcept;

  /** @brief Parse Block from an EncodingBuffer
   *  @param buffer an EncodingBuffer containing one TLV element
   *  @throw tlv::Error Type-Length parsing fails, or TLV-LENGTH does not match size of TLV-VALUE
   */
  explicit
  Block(const EncodingBuffer& buffer);

  /** @brief Parse Block from a wire Buffer
   *  @param buffer a Buffer containing one TLV element
   *  @note This constructor takes shared ownership of @p buffer.
   *  @throw tlv::Error Type-Length parsing fails, or TLV-LENGTH does not match size of TLV-VALUE
   */
  explicit
  Block(const ConstBufferPtr& buffer);

  /** @brief Parse Block within boundaries of a wire Buffer
   *  @param buffer a Buffer containing a TLV element at [@p begin,@p end)
   *  @param begin begin position of the TLV element within @p buffer
   *  @param end end position of the TLV element within @p buffer
   *  @param verifyLength if true, check TLV-LENGTH equals size of TLV-VALUE
   *  @throw std::invalid_argument @p buffer is empty, or [@p begin,@p end) range is not within @p buffer
   *  @throw tlv::Error Type-Length parsing fails, or TLV-LENGTH does not match size of TLV-VALUE
   *  @note This overload automatically detects TLV-TYPE and position of TLV-VALUE.
   */
  Block(ConstBufferPtr buffer, Buffer::const_iterator begin, Buffer::const_iterator end,
        bool verifyLength = true);

  /** @brief Parse Block within boundaries of an existing Block, reusing underlying wire Buffer
   *  @param block a Block whose buffer contains a TLV element at [@p begin,@p end)
   *  @param begin begin position of the TLV element within @p block
   *  @param end end position of the TLV element within @p block
   *  @param verifyLength if true, check TLV-LENGTH equals size of TLV-VALUE
   *  @throw std::invalid_argument [@p begin,@p end) range is not within @p block
   *  @throw tlv::Error Type-Length parsing fails, or TLV-LENGTH does not match size of TLV-VALUE
   */
  Block(const Block& block, Buffer::const_iterator begin, Buffer::const_iterator end,
        bool verifyLength = true);

  /** @brief Create a Block from a wire Buffer without parsing
   *  @param buffer a Buffer containing a TLV element at [@p begin,@p end)
   *  @param type TLV-TYPE
   *  @param begin begin position of the TLV element within @p buffer
   *  @param end end position of the TLV element within @p buffer
   *  @param valueBegin begin position of TLV-VALUE within @p buffer
   *  @param valueEnd end position of TLV-VALUE within @p buffer
   */
  Block(ConstBufferPtr buffer, uint32_t type,
        Buffer::const_iterator begin, Buffer::const_iterator end,
        Buffer::const_iterator valueBegin, Buffer::const_iterator valueEnd);

  /** @brief Parse Block from a raw buffer
   *  @param buf pointer to the first octet of a TLV element
   *  @param bufSize size of the raw buffer; may be greater than the actual size of the TLV element
   *  @throw tlv::Error Type-Length parsing fails, or size of TLV-VALUE exceeds @p bufSize
   *  @note This overload copies the TLV element octets into an internal wire buffer.
   */
  Block(const uint8_t* buf, size_t bufSize);

  /** @brief Create a zero-length Block with the specified TLV-TYPE
   *  @param type TLV-TYPE
   */
  explicit
  Block(uint32_t type);

  /** @brief Create a Block with the specified TLV-TYPE and TLV-VALUE
   *  @param type TLV-TYPE
   *  @param value a Buffer containing the TLV-VALUE, must not be nullptr
   */
  Block(uint32_t type, ConstBufferPtr value);

  /** @brief Create a Block with the specified TLV-TYPE and TLV-VALUE
   *  @param type TLV-TYPE
   *  @param value a Block to be nested as TLV-VALUE, must be valid
   */
  Block(uint32_t type, const Block& value);

  /** @brief Parse Block from an input stream
   *  @throw tlv::Error TLV-LENGTH is zero or exceeds upper bound
   *  @warning If decoding fails, bytes are still consumed from the input stream.
   */
  static Block
  fromStream(std::istream& is);

  /** @brief Try to parse Block from a wire buffer
   *  @param buffer a Buffer containing a TLV element at offset @p offset
   *  @param offset begin position of the TLV element within @p buffer
   *  @note This function does not throw upon decoding failure.
   *  @return `true` and the parsed Block if parsing succeeds; otherwise `false` and an invalid Block
   */
  NDN_CXX_NODISCARD static std::tuple<bool, Block>
  fromBuffer(ConstBufferPtr buffer, size_t offset);

  /** @brief Try to parse Block from a raw buffer
   *  @param buf pointer to the first octet of a TLV element
   *  @param bufSize size of the raw buffer; may be greater than the actual size of the TLV element
   *  @note This function does not throw upon decoding failure.
   *  @note This overload copies the TLV element octets into an internal wire buffer.
   *  @return `true` and the parsed Block if parsing succeeds; otherwise `false` and an invalid Block
   */
  NDN_CXX_NODISCARD static std::tuple<bool, Block>
  fromBuffer(const uint8_t* buf, size_t bufSize);

public: // wire format
  /** @brief Check if the Block is valid
   *
   *  A Block is valid unless it has an invalid TLV-TYPE or is default-constructed.
   *  In particular, a Block with zero-length TLV-VALUE *is* valid.
   */
  bool
  isValid() const noexcept
  {
    return m_type != tlv::Invalid;
  }

  /** @brief Check if the Block is empty
   *
   *  A Block is considered empty *iff* it is not valid, i.e., isValid() returns false.
   *
   *  @warning Note that an empty Block is *not* the same as a valid but zero-length Block.
   *  @deprecated Use Block::isValid()
   */
  NDN_CXX_NODISCARD bool
  empty() const noexcept
  {
    return !isValid();
  }

  /** @brief Reset the Block to a default-constructed state
   *
   *  Equivalent to `*this = Block()`.
   *
   *  @post `isValid() == false`
   *  @sa resetWire()
   */
  void
  reset() noexcept;

  /** @brief Reset wire buffer but keep TLV-TYPE and sub-elements (if any)
   *  @post `hasWire() == false`
   *  @post `hasValue() == false`
   *  @sa reset()
   */
  void
  resetWire() noexcept;

  /** @brief Check if the Block contains a fully encoded wire representation
   *
   *  A Block has a fully encoded wire if the underlying buffer exists and contains the full
   *  Type-Length-Value instead of just the TLV-VALUE field.
   */
  bool
  hasWire() const noexcept
  {
    return m_buffer != nullptr && m_begin != m_end;
  }

  /** @brief Get begin iterator of encoded wire
   *  @pre `hasWire() == true`
   */
  Buffer::const_iterator
  begin() const;

  /** @brief Get end iterator of encoded wire
   *  @pre `hasWire() == true`
   */
  Buffer::const_iterator
  end() const;

  /** @brief Return a raw pointer to the beginning of the encoded wire
   *  @pre `hasWire() == true`
   *  @sa value()
   */
  const uint8_t*
  wire() const;

  /** @brief Return the size of the encoded wire, i.e. of the whole TLV
   *  @pre `isValid() == true`
   *  @sa value_size()
   */
  size_t
  size() const;

  /** @brief Get underlying buffer
   */
  ConstBufferPtr
  getBuffer() const
  {
    return m_buffer;
  }

public: // type and value
  /** @brief Return the TLV-TYPE of the Block
   *  @note This will return tlv::Invalid if isValid() is false.
   */
  uint32_t
  type() const
  {
    return m_type;
  }

  /** @brief Check if the Block has a non-empty TLV-VALUE
   *
   *  This property reflects whether the underlying buffer contains a TLV-VALUE. If this is false,
   *  TLV-VALUE has zero-length. If this is true, TLV-VALUE may be zero-length.
   *
   *  @sa value_size()
   */
  bool
  hasValue() const noexcept
  {
    return m_buffer != nullptr;
  }

  /** @brief Get begin iterator of TLV-VALUE
   *  @pre `hasValue() == true`
   */
  Buffer::const_iterator
  value_begin() const
  {
    return m_valueBegin;
  }

  /** @brief Get end iterator of TLV-VALUE
   *  @pre `hasValue() == true`
   */
  Buffer::const_iterator
  value_end() const
  {
    return m_valueEnd;
  }

  /** @brief Return a raw pointer to the beginning of TLV-VALUE
   *  @sa wire()
   */
  const uint8_t*
  value() const noexcept;

  /** @brief Return the size of TLV-VALUE, aka TLV-LENGTH
   *  @sa size()
   */
  size_t
  value_size() const noexcept;

  Block
  blockFromValue() const;

public: // sub-elements
  /** @brief Parse TLV-VALUE into sub-elements
   *  @post elements() reflects sub-elements found in TLV-VALUE
   *  @throw tlv::Error TLV-VALUE is not a sequence of TLV elements
   *  @note This method does not perform recursive parsing.
   *  @note This method has no effect if elements() is already populated.
   *  @note This method is not really const, but it does not modify any data.
   */
  void
  parse() const;

  /** @brief Encode sub-elements into TLV-VALUE
   *  @post TLV-VALUE contains sub-elements from elements()
   */
  void
  encode();

  /** @brief Return the first sub-element of the specified TLV-TYPE
   *  @pre parse() has been executed
   *  @throw tlv::Error a sub-element of the specified type does not exist
   */
  const Block&
  get(uint32_t type) const;

  /** @brief Find the first sub-element of the specified TLV-TYPE
   *  @pre parse() has been executed
   *  @return iterator in elements() to the found sub-element, or elements_end() if no such
   *          sub-element exists in elements()
   */
  element_const_iterator
  find(uint32_t type) const;

  /** @brief Remove all sub-elements of the specified TLV-TYPE
   *  @pre parse() has been executed
   *  @post `find(type) == elements_end()`
   */
  void
  remove(uint32_t type);

  /** @brief Erase a sub-element
   */
  element_iterator
  erase(element_const_iterator position);

  /** @brief Erase a range of sub-elements
   */
  element_iterator
  erase(element_const_iterator first, element_const_iterator last);

  /** @brief Append a sub-element
   */
  void
  push_back(const Block& element);

  /** @brief Insert a sub-element
   *  @param pos position of the new sub-element
   *  @param element new sub-element to insert
   *  @return iterator in elements() to the new sub-element
   */
  element_iterator
  insert(element_const_iterator pos, const Block& element);

  /** @brief Get container of sub-elements
   *  @pre parse() has been executed
   */
  const element_container&
  elements() const
  {
    return m_elements;
  }

  /** @brief Equivalent to elements().begin()
   */
  element_const_iterator
  elements_begin() const
  {
    return m_elements.begin();
  }

  /** @brief Equivalent to elements().end()
   */
  element_const_iterator
  elements_end() const
  {
    return m_elements.end();
  }

  /** @brief Equivalent to elements().size()
   */
  size_t
  elements_size() const
  {
    return m_elements.size();
  }

public: // misc
  /** @brief Implicit conversion to `boost::asio::const_buffer`
   */
  operator boost::asio::const_buffer() const;

private:
  /** @brief Estimate Block size as if sub-elements are encoded into TLV-VALUE
   */
  size_t
  encode(EncodingEstimator& estimator) const;

  /** @brief Estimate TLV-LENGTH as if sub-elements are encoded into TLV-VALUE
   */
  size_t
  encodeValue(EncodingEstimator& estimator) const;

  /** @brief Encode sub-elements into TLV-VALUE and prepend Block to encoder
   *  @post TLV-VALUE contains sub-elements from elements()
   *  @post internal buffer and iterators point to Encoder's buffer
   */
  size_t
  encode(EncodingBuffer& encoder);

protected:
  /** @brief Underlying buffer storing TLV-VALUE and possibly TLV-TYPE and TLV-LENGTH fields
   *
   *  If m_buffer is nullptr, this is an invalid or zero-length Block with TLV-TYPE given in m_type.
   *  Otherwise,
   *  - [m_valueBegin, m_valueEnd) point to the TLV-VALUE inside m_buffer.
   *  - If m_begin != m_end, [m_begin, m_end) point to Type-Length-Value of this Block in m_buffer.
   *    Otherwise, m_buffer does not contain TLV-TYPE and TLV-LENGTH fields.
   */
  shared_ptr<const Buffer> m_buffer;
  Buffer::const_iterator m_begin; ///< @sa m_buffer
  Buffer::const_iterator m_end; ///< @sa m_buffer

  Buffer::const_iterator m_valueBegin; ///< @sa m_buffer
  Buffer::const_iterator m_valueEnd; ///< @sa m_buffer

  uint32_t m_type = tlv::Invalid; ///< TLV-TYPE

  /** @brief Total size including Type-Length-Value
   *
   *  This field is meaningful only if isValid() is true.
   */
  size_t m_size = 0;

  /** @brief Contains the sub-elements
   *
   *  This field is valid only if parse() has been executed.
   */
  mutable element_container m_elements;

  /** @brief Print @p block to @p os.
   *
   *  Default-constructed Block is printed as: `[invalid]`.
   *  Zero-length Block is printed as: `TT[empty]`, where TT is TLV-TYPE in decimal.
   *  Non-zero-length Block on which parse() has not been called is printed as: `TT[LL]=VVVV`,
   *  where LL is TLV-LENGTH in decimal, and VVVV is TLV-VALUE in hexadecimal.
   *  Block on which parse() has been called is printed as: `TT[LL]={SUB,SUB}`,
   *  where each SUB is a sub-element printed using this format.
   */
  friend std::ostream&
  operator<<(std::ostream& os, const Block& block);
};

inline
Block::Block(Block&&) noexcept = default;

inline Block&
Block::operator=(Block&&) noexcept = default;

/** @brief Compare whether two Blocks have same TLV-TYPE, TLV-LENGTH, and TLV-VALUE
 */
bool
operator==(const Block& lhs, const Block& rhs);

inline bool
operator!=(const Block& lhs, const Block& rhs)
{
  return !(lhs == rhs);
}

/** @brief Construct a Block from hexadecimal @p input.
 *  @param input a string containing hexadecimal bytes and comments.
 *               0-9 and upper-case A-F are input; all other characters are comments.
 *  @param len length of @p input.
 *  @throw std::invalid_argument input is empty or has an odd number of hexadecimal digits.
 *  @throw tlv::Error @p input cannot be parsed into a valid Block.
 *
 *  Example
 *  @code
 *  Block nameBlock = "0706 080141 080142"_block;
 *  Block nackBlock = "FD032005 reason(no-route)=FD03210196"_block;
 *  @endcode
 */
Block
operator "" _block(const char* input, std::size_t len);

} // namespace ndn

#endif // NDN_ENCODING_BLOCK_HPP

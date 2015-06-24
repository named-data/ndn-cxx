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

#ifndef NDN_NAME_COMPONENT_HPP
#define NDN_NAME_COMPONENT_HPP

#include "common.hpp"
#include "encoding/block.hpp"
#include "encoding/block-helpers.hpp"
#include "util/time.hpp"

namespace ndn {
namespace name {

/// @brief Segment marker for NDN naming conventions
static const uint8_t SEGMENT_MARKER = 0x00;
/// @brief Segment offset marker for NDN naming conventions
static const uint8_t SEGMENT_OFFSET_MARKER = 0xFB;
/// @brief Version marker for NDN naming conventions
static const uint8_t VERSION_MARKER = 0xFD;
/// @brief Timestamp marker for NDN naming conventions
static const uint8_t TIMESTAMP_MARKER = 0xFC;
/// @brief Sequence number marker for NDN naming conventions
static const uint8_t SEQUENCE_NUMBER_MARKER = 0xFE;

/**
 * @brief Component holds a read-only name component value.
 */
class Component : public Block
{
public:
  /**
   * @brief Error that can be thrown from name::Component
   */
  class Error : public Block::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Block::Error(what)
    {
    }
  };

  /**
   * Create a new name::Component with an empty value
   */
  Component();

  /**
   * @brief Create name::Component from a wire block
   *
   * @param wire tlv::NameComponent Block from which to create name::Component
   * @throws Error if wire.type() is not tlv::NameComponent
   *
   * Any block can be implicitly converted to name::Component
   */
  Component(const Block& wire);

  /**
   * @brief Create a new name::Component from the buffer pointer (buffer pointer will be copied)
   *
   * @param buffer A pointer to an immutable buffer
   *
   * This constructor will create a new tlv::NameComponent Block with `buffer` as a payload.
   * Note that this method **will not** allocate new memory for and copy the payload until
   * toWire() method is called.
   */
  explicit
  Component(const ConstBufferPtr& buffer);

  /**
   * @brief Create a new name::Component from the buffer (data from buffer will be copied)
   * @param buffer A reference to the buffer
   *
   * This constructor will create a new tlv::NameComponent Block with `buffer` as a payload.
   * Note that this method **will** allocate new memory for and copy the payload.
   */
  explicit
  Component(const Buffer& buffer);

  /**
   * @brief Create a new name::Component from the buffer (data from buffer will be copied)
   * @param buffer     A pointer to the first byte of the buffer
   * @param bufferSize Size of the buffer
   *
   * This constructor will create a new tlv::NameComponent Block with `buffer` as a payload.
   * Note that this method **will** allocate new memory for and copy the payload.
   */
  Component(const uint8_t* buffer, size_t bufferSize);

  /**
   * @brief Create a new name::Component frome the range [@p first, @p last) of bytes
   * @param first     Iterator pointing to the beginning of the buffer
   * @param last      Iterator pointing to the ending of the buffer
   * @tparam Iterator iterator type satisfying at least InputIterator concept.  Implementation
   *                  is more optimal when the iterator type satisfies RandomAccessIterator concept.
   *                  It is required that sizeof(std::iterator_traits<Iterator>::value_type) == 1.
   *
   * This constructor will create a new tlv::NameComponent Block with `buffer` as a payload.
   * Note that this method **will** allocate new memory for and copy the payload.
   */
  template<class Iterator>
  Component(Iterator first, Iterator last);

  /**
   * @brief Create a new name::Component from the C string (data from string will be copied)
   *
   * @param str Zero-ended string.  Note that this string will be interpreted as is (i.e.,
   *            it will not be interpreted as URI)
   *
   * This constructor will create a new tlv::NameComponent Block with `buffer` as a payload.
   * Note that this method **will** allocate new memory for and copy the payload.
   */
  explicit
  Component(const char* str);

  /**
   * @brief Create a new name::Component from the STL string (data from string will be copied)
   *
   * @param str Const reference to STL string.  Note that this string will be interpreted
   *            as is (i.e., it will not be interpreted as URI)
   *
   * This constructor will create a new tlv::NameComponent Block with `buffer` as a payload.
   * Note that this method **will** allocate new memory for and copy the payload.
   */
  explicit
  Component(const std::string& str);

  /**
   * @brief Fast encoding or block size estimation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /**
   * @brief Encode to a wire format
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode from the wire format
   */
  void
  wireDecode(const Block& wire);

  /**
   * @brief Create name::Component by decoding the escapedString between beginOffset and
   * endOffset according to the NDN URI Scheme.
   *
   * If the escaped string is "", "." or ".." then return an empty name::Component.  Note
   * that an empty name::Component should not be added to Name and if attempted, an
   * exception will be thrown.
   *
   * @param escapedString String containing NDN URI-encoded name
   *                      component. [escapedString+beginOffset, beginOffset+endOffset)
   *                      must be a valid memory buffer.
   * @param beginOffset The offset in escapedString of the beginning of the portion to decode.
   * @param endOffset   The offset in escapedString of the end of the portion to decode.
   */
  static Component
  fromEscapedString(const char* escapedString, size_t beginOffset, size_t endOffset);

  /**
   * @brief Create name::Component by decoding the escapedString according to the NDN URI Scheme
   *
   * This overload is a convenience wrapper for fromEscapedString(char*,size_t,size)
   */
  static Component
  fromEscapedString(const char* escapedString)
  {
    return fromEscapedString(escapedString, 0, std::char_traits<char>::length(escapedString));
  }

  /**
   * @brief Create name::Component by decoding the escapedString according to the NDN URI Scheme
   *
   * This overload is a convenience wrapper for fromEscapedString(char*,size_t,size)
   */
  static Component
  fromEscapedString(const std::string& escapedString)
  {
    return fromEscapedString(escapedString.c_str(), 0, escapedString.size());
  }

  /**
   * @brief Write *this to the output stream, escaping characters according to the NDN URI Scheme
   *
   * @deprecated Use toUri(std::ostream&) instead
   *
   * This also adds "..." to a value with zero or more "."
   *
   * @param os The output stream to where write the URI escaped version *this
   */
  DEPRECATED(
  void
  toEscapedString(std::ostream& os) const)
  {
    return toUri(os);
  }

  /**
   * @brief Convert *this by escaping characters according to the NDN URI Scheme
   *
   * @deprecated Use toUri() instead
   *
   * This also adds "..." to a value with zero or more "."
   *
   * @return The escaped string
   */
  DEPRECATED(
  std::string
  toEscapedString() const)
  {
    return toUri();
  }

  /**
   * @brief Write *this to the output stream, escaping characters according to the NDN URI Scheme
   *
   * This also adds "..." to a value with zero or more "."
   *
   * @param os The output stream to where write the URI escaped version *this
   */
  void
  toUri(std::ostream& os) const;

  /**
   * @brief Convert *this by escaping characters according to the NDN URI Scheme
   *
   * This also adds "..." to a value with zero or more "."
   *
   * @return The escaped string
   */
  std::string
  toUri() const;

  ////////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Check if the component is nonNegativeInteger
   * @see http://named-data.net/doc/ndn-tlv/tlv.html#non-negative-integer-encoding
   */
  bool
  isNumber() const;

  /**
   * @brief Check if the component is NameComponentWithMarker per NDN naming conventions
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isNumberWithMarker(uint8_t marker) const;

  /**
   * @brief Check if the component is version per NDN naming conventions
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isVersion() const;

  /**
   * @brief Check if the component is segment number per NDN naming conventions
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isSegment() const;

  /**
   * @brief Check if the component is segment offset per NDN naming conventions
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isSegmentOffset() const;

  /**
   * @brief Check if the component is timestamp per NDN naming conventions
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isTimestamp() const;

  /**
   * @brief Check if the component is sequence number per NDN naming conventions
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isSequenceNumber() const;

  ////////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Interpret this name component as nonNegativeInteger
   *
   * @see http://named-data.net/doc/ndn-tlv/tlv.html#non-negative-integer-encoding
   *
   * @return The integer number.
   */
  uint64_t
  toNumber() const;

  /**
   * @brief Interpret this name component as NameComponentWithMarker
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @param marker 1-byte octet of the marker
   * @return The integer number.
   * @throws Error if name component does not have the specified marker.
   *         tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  uint64_t
  toNumberWithMarker(uint8_t marker) const;

  /**
   * @brief Interpret as version component using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @throws Error if name component does not have the specified marker.
   *         tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  uint64_t
  toVersion() const;

  /**
   * @brief Interpret as segment number component using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @throws Error if name component does not have the specified marker.
   *         tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  uint64_t
  toSegment() const;

  /**
   * @brief Interpret as segment offset component using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @throws Error if name component does not have the specified marker.
   *         tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  uint64_t
  toSegmentOffset() const;

  /**
   * @brief Interpret as timestamp component using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @throws Error if name component does not have the specified marker.
   *         tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  time::system_clock::TimePoint
  toTimestamp() const;

  /**
   * @brief Interpret as sequence number component using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @throws Error if name component does not have the specified marker.
   *         tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  uint64_t
  toSequenceNumber() const;

  ////////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Create a component encoded as nonNegativeInteger
   *
   * @see http://named-data.net/doc/ndn-tlv/tlv.html#non-negative-integer-encoding
   *
   * @param number The non-negative number
   * @return The component value.
   */
  static Component
  fromNumber(uint64_t number);

  /**
   * @brief Create a component encoded as NameComponentWithMarker
   *
   * NameComponentWithMarker is defined as:
   *
   *     NameComponentWithMarker ::= NAME-COMPONENT-TYPE TLV-LEGTH
   *                                   Marker
   *                                   includedNonNegativeInteger
   *     Marker ::= BYTE
   *     includedNonNegativeInteger ::= BYTE{1,2,4,8}
   *     NDN-TLV := TLV-TYPE TLV-LENGTH TLV-VALUE?
   *     TLV-TYPE := VAR-NUMBER
   *     TLV-LENGTH := VAR-NUMBER
   *     TLV-VALUE := BYTE+
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @param marker 1-byte marker octet
   * @param number The non-negative number
   * @return The component value.
   */
  static Component
  fromNumberWithMarker(uint8_t marker, uint64_t number);

  /**
   * @brief Create version component using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  static Component
  fromVersion(uint64_t version);

  /**
   * @brief Create segment number component using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  static Component
  fromSegment(uint64_t segmentNo);

  /**
   * @brief Create segment offset component using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  static Component
  fromSegmentOffset(uint64_t offset);

  /**
   * @brief Create sequence number component using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  static Component
  fromTimestamp(const time::system_clock::TimePoint& timePoint);

  /**
   * @brief Create sequence number component using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  static Component
  fromSequenceNumber(uint64_t seqNo);

  ////////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Check if the component is GenericComponent
   */
  bool
  isGeneric() const;

  /**
   * @brief Check if the component is ImplicitSha256DigestComponent
   */
  bool
  isImplicitSha256Digest() const;

  /**
   * @brief Create ImplicitSha256DigestComponent component
   */
  static Component
  fromImplicitSha256Digest(const ConstBufferPtr& digest);

  /**
   * @brief Create ImplicitSha256DigestComponent component
   */
  static Component
  fromImplicitSha256Digest(const uint8_t* digest, size_t digestSize);

  ////////////////////////////////////////////////////////////////////////////////

  bool
  empty() const
  {
    return !hasValue() || value_size() == 0;
  }

  Component
  getSuccessor() const;

  /**
   * @brief Check if this is the same component as other
   *
   * @param other The other Component to compare with
   * @return true if the components are equal, otherwise false.
   */
  bool
  equals(const Component& other) const
  {
    if (value_size() != other.value_size())
      return false;
    if (value_size() == 0 /* == other.value_size()*/)
      return true;

    // somehow, behavior is wrong on OSX 10.9 when component is empty
    // (probably some bug in STL...)
    return std::equal(value_begin(), value_end(), other.value_begin());
  }

  /**
   * @brief Compare this to the other Component using NDN canonical ordering
   *
   * @param other The other Component to compare with.
   * @retval negative this comes before other in canonical ordering
   * @retval zero this equals other
   * @retval positive this comes after other in canonical ordering
   *
   * @see http://named-data.net/doc/ndn-tlv/name.html#canonical-order
   */
  int
  compare(const Component& other) const;

  /**
   * @brief Check if this is the same component as other
   *
   * @param other The other Component to compare with.
   * @return true if the components are equal, otherwise false.
   */
  bool
  operator==(const Component& other) const
  {
    return equals(other);
  }

  /**
   * @brief Check if this is not the same component as other
   * @param other The other Component to compare with
   * @return true if the components are not equal, otherwise false
   */
  bool
  operator!=(const Component& other) const
  {
    return !equals(other);
  }

  /**
   * @brief Check if the *this is less than or equal to the other in NDN canonical ordering
   * @param other The other Component to compare with
   *
   * @see http://named-data.net/doc/ndn-tlv/name.html#canonical-order
   */
  bool
  operator<=(const Component& other) const
  {
    return compare(other) <= 0;
  }

  /**
   * @brief Check if the *this is less than the other in NDN canonical ordering
   * @param other The other Component to compare with
   *
   * @see http://named-data.net/doc/ndn-tlv/name.html#canonical-order
   */
  bool
  operator<(const Component& other) const
  {
    return compare(other) < 0;
  }

  /**
   * @brief Check if the *this is greater or equal than the other in NDN canonical ordering
   * @param other The other Component to compare with
   *
   * @see http://named-data.net/doc/ndn-tlv/name.html#canonical-order
   */
  bool
  operator>=(const Component& other) const
  {
    return compare(other) >= 0;
  }

  /**
   * @brief Check if the *this is greater than the other in NDN canonical ordering
   * @param other The other Component to compare with
   *
   * @see http://named-data.net/doc/ndn-tlv/name.html#canonical-order
   */
  bool
  operator>(const Component& other) const
  {
    return compare(other) > 0;
  }

  // !!! NOTE TO IMPLEMENTOR !!!
  //
  // This class MUST NOT contain any data fields.
  // Block can be reinterpret_cast'ed as Component type.
};

inline std::ostream&
operator<<(std::ostream& os, const Component& component)
{
  component.toUri(os);
  return os;
}

template<class Iterator>
inline
Component::Component(Iterator first, Iterator last)
  : Block(makeBinaryBlock(tlv::NameComponent, first, last))
{
}

} // namespace name
} // namespace ndn

#endif // NDN_NAME_COMPONENT_HPP

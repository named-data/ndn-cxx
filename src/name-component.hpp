/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

/** @brief Represents a name component.
 *
 *  The @c Component class provides a read-only view of a @c Block interpreted as a name component.
 *  Although it inherits mutation methods from @c Block base class, they must not be used, because
 *  the enclosing @c Name would not be updated correctly.
 */
class Component : public Block
{
public:
  class Error : public Block::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Block::Error(what)
    {
    }
  };

public: // constructors
  /**
   * @brief Construct a NameComponent of TLV-TYPE @p type, using empty TLV-VALUE.
   * @throw Error the NameComponent is invalid (see @c ensureValid).
   */
  explicit
  Component(uint32_t type = tlv::GenericNameComponent);

  /**
   * @brief Construct a NameComponent from @p block.
   * @throw Error the NameComponent is invalid (see @c ensureValid).
   *
   * This contructor enables implicit conversion from @c Block.
   */
  Component(const Block& wire);

  /**
   * @brief Construct a NameComponent of TLV-TYPE @p type, using TLV-VALUE from @p buffer.
   * @throw Error the NameComponent is invalid (see @c ensureValid).
   *
   * This constructor does not copy the underlying buffer, but retains a pointer to it.
   * Therefore, the caller must not change the underlying buffer.
   */
  Component(uint32_t type, ConstBufferPtr buffer);

  /**
   * @brief Construct a GenericNameComponent, using TLV-VALUE from @p buffer.
   * @throw Error the NameComponent is invalid (see @c ensureValid).
   *
   * This constructor does not copy the underlying buffer, but retains a pointer to it.
   * Therefore, the caller must not change the underlying buffer.
   */
  explicit
  Component(ConstBufferPtr buffer)
    : Component(tlv::GenericNameComponent, std::move(buffer))
  {
  }

  /**
   * @brief Construct a NameComponent of TLV-TYPE @p type, copying TLV-VALUE from @p buffer.
   */
  Component(uint32_t type, const Buffer& buffer)
    : Component(type, buffer.data(), buffer.size())
  {
  }

  /**
   * @brief Construct a GenericNameComponent, copying TLV-VALUE from @p buffer.
   */
  explicit
  Component(const Buffer& buffer)
    : Component(tlv::GenericNameComponent, buffer)
  {
  }

  /**
   * @brief Construct a NameComponent of TLV-TYPE @p type, copying @p count bytes at @p value as
   *        TLV-VALUE.
   */
  Component(uint32_t type, const uint8_t* value, size_t count);

  /**
   * @brief Construct a GenericNameComponent, copying @p count bytes at @p value as TLV-VALUE.
   */
  Component(const uint8_t* value, size_t count)
    : Component(tlv::GenericNameComponent, value, count)
  {
  }

  /**
   * @brief Construct a NameComponent of TLV-TYPE @p type, copying TLV-VALUE from a range.
   * @tparam Iterator an @c InputIterator dereferencing to a one-octet value type. More efficient
   *                  implementation is available when it is a @c RandomAccessIterator.
   * @param type      the TLV-TYPE.
   * @param first     beginning of the range.
   * @param last      past-end of the range.
   */
  template<class Iterator>
  Component(uint32_t type, Iterator first, Iterator last)
    : Block(makeBinaryBlock(type, first, last))
  {
  }

  /**
   * @brief Construct a GenericNameComponent, copying TLV-VALUE from a range.
   */
  template<class Iterator>
  Component(Iterator first, Iterator last)
    : Component(tlv::GenericNameComponent, first, last)
  {
  }

  /**
   * @brief Construct a GenericNameComponent, copying TLV-VALUE from a null-terminated string.
   *
   * Bytes from the string are copied as is, and not interpreted as URI component.
   */
  explicit
  Component(const char* str);

  /**
   * @brief Construct a GenericNameComponent, copying TLV-VALUE from a string.
   *
   * Bytes from the string are copied as is, and not interpreted as URI component.
   */
  explicit
  Component(const std::string& str);

public: // encoding and URI
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
   * @brief Decode NameComponent from a URI component.
   *
   * The URI component is read from `[input+beginOffset, input+endOffset)` range.
   *
   * @throw Error URI component does not represent a valid NameComponent.
   */
  static Component
  fromEscapedString(const char* input, size_t beginOffset, size_t endOffset)
  {
    return fromEscapedString(std::string(input + beginOffset, input + endOffset));
  }

  /**
   * @brief Decode NameComponent from a URI component.
   * @throw Error URI component does not represent a valid NameComponent.
   */
  static Component
  fromEscapedString(const char* input)
  {
    return fromEscapedString(std::string(input));
  }

  /**
   * @brief Decode NameComponent from a URI component.
   * @throw Error URI component does not represent a valid NameComponent.
   */
  static Component
  fromEscapedString(std::string input);

  /**
   * @brief Write *this to the output stream, escaping characters according to the NDN URI Scheme
   *
   * This also adds "..." to a value with zero or more "."
   *
   * @param os The output stream where to write the URI escaped version of *this
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

public: // naming conventions
  /**
   * @brief Check if the component is nonNegativeInteger
   * @sa https://named-data.net/doc/NDN-packet-spec/current/tlv.html#non-negative-integer-encoding
   */
  bool
  isNumber() const;

  /**
   * @brief Check if the component is NameComponentWithMarker per NDN naming conventions
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isNumberWithMarker(uint8_t marker) const;

  /**
   * @brief Check if the component is version per NDN naming conventions
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isVersion() const;

  /**
   * @brief Check if the component is segment number per NDN naming conventions
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isSegment() const;

  /**
   * @brief Check if the component is segment offset per NDN naming conventions
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isSegmentOffset() const;

  /**
   * @brief Check if the component is timestamp per NDN naming conventions
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isTimestamp() const;

  /**
   * @brief Check if the component is sequence number per NDN naming conventions
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  bool
  isSequenceNumber() const;

  /**
   * @brief Interpret this name component as nonNegativeInteger
   *
   * @sa https://named-data.net/doc/NDN-packet-spec/current/tlv.html#non-negative-integer-encoding
   *
   * @return The integer number.
   */
  uint64_t
  toNumber() const;

  /**
   * @brief Interpret this name component as NameComponentWithMarker
   *
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
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
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @throws Error if name component does not have the specified marker.
   *         tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  uint64_t
  toVersion() const;

  /**
   * @brief Interpret as segment number component using NDN naming conventions
   *
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @throws Error if name component does not have the specified marker.
   *         tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  uint64_t
  toSegment() const;

  /**
   * @brief Interpret as segment offset component using NDN naming conventions
   *
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @throws Error if name component does not have the specified marker.
   *         tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  uint64_t
  toSegmentOffset() const;

  /**
   * @brief Interpret as timestamp component using NDN naming conventions
   *
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @throws Error if name component does not have the specified marker.
   *         tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  time::system_clock::TimePoint
  toTimestamp() const;

  /**
   * @brief Interpret as sequence number component using NDN naming conventions
   *
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @throws Error if name component does not have the specified marker.
   *         tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  uint64_t
  toSequenceNumber() const;

  /**
   * @brief Create a component encoded as nonNegativeInteger
   *
   * @sa https://named-data.net/doc/NDN-packet-spec/current/tlv.html#non-negative-integer-encoding
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
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
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
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  static Component
  fromVersion(uint64_t version);

  /**
   * @brief Create segment number component using NDN naming conventions
   *
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  static Component
  fromSegment(uint64_t segmentNo);

  /**
   * @brief Create segment offset component using NDN naming conventions
   *
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  static Component
  fromSegmentOffset(uint64_t offset);

  /**
   * @brief Create sequence number component using NDN naming conventions
   *
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  static Component
  fromTimestamp(const time::system_clock::TimePoint& timePoint);

  /**
   * @brief Create sequence number component using NDN naming conventions
   *
   * @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  static Component
  fromSequenceNumber(uint64_t seqNo);

public: // commonly used TLV-TYPEs
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

public: // operators
  bool
  empty() const
  {
    return value_size() == 0;
  }

  /**
   * @brief Check if this is the same component as other
   *
   * @param other The other Component to compare with
   * @return true if the components are equal, otherwise false.
   */
  bool
  equals(const Component& other) const;

  /**
   * @brief Compare this to the other Component using NDN canonical ordering
   *
   * @param other The other Component to compare with.
   * @retval negative this comes before other in canonical ordering
   * @retval zero this equals other
   * @retval positive this comes after other in canonical ordering
   *
   * @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#canonical-order
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
   * @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#canonical-order
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
   * @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#canonical-order
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
   * @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#canonical-order
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
   * @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#canonical-order
   */
  bool
  operator>(const Component& other) const
  {
    return compare(other) > 0;
  }

  Component
  getSuccessor() const;

private:
  /**
   * @brief Throw @c Error if this NameComponent is invalid.
   *
   * A NameComponent is invalid if its TLV-TYPE is outside the 1-65535 range.
   * Additionally, if this is an ImplicitSha256DigestComponent, the TLV-LENGTH must be 32.
   */
  void
  ensureValid() const;

  // !!! NOTE TO IMPLEMENTOR !!!
  //
  // This class MUST NOT contain any data fields.
  // Block can be reinterpret_cast'ed as Component type.
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(Component);

inline std::ostream&
operator<<(std::ostream& os, const Component& component)
{
  component.toUri(os);
  return os;
}

} // namespace name
} // namespace ndn

#endif // NDN_NAME_COMPONENT_HPP

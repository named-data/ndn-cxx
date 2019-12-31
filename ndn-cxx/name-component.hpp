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
 */

#ifndef NDN_NAME_COMPONENT_HPP
#define NDN_NAME_COMPONENT_HPP

#include "ndn-cxx/detail/common.hpp"
#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"
#include "ndn-cxx/util/time.hpp"

namespace ndn {
namespace name {

/** @brief Identify a format of URI representation.
 */
enum class UriFormat {
  DEFAULT,   ///< ALTERNATE, unless `NDN_NAME_ALT_URI` environment variable is set to '0'
  CANONICAL, ///< always use <type-number>=<percent-encoded-value> format
  ALTERNATE, ///< prefer alternate format when available
};

/** @brief Identify a style of NDN Naming Conventions.
 *  @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
 */
enum class Convention {
  MARKER = 1 << 0, ///< component markers (revision 1)
  TYPED  = 1 << 1, ///< typed name components (revision 2)
  EITHER = MARKER | TYPED,
};

/** @brief Markers in Naming Conventions rev1
 */
enum : uint8_t {
  SEGMENT_MARKER = 0x00,
  SEGMENT_OFFSET_MARKER = 0xFB,
  VERSION_MARKER = 0xFD,
  TIMESTAMP_MARKER = 0xFC,
  SEQUENCE_NUMBER_MARKER = 0xFE,
};

/** @brief Return which Naming Conventions style to use while encoding.
 *
 *  The current library default is Convention::MARKER, but this will change in the future.
 */
Convention
getConventionEncoding();

/** @brief Set which Naming Conventions style to use while encoding.
 *  @param convention either Convention::MARKER or Convention::TYPED.
 */
void
setConventionEncoding(Convention convention);

/** @brief Return which Naming Conventions style(s) to accept while decoding.
 *
 *  The current library default is Convention::EITHER, but this will change in the future.
 */
Convention
getConventionDecoding();

/** @brief Set which Naming Conventions style(s) to accept while decoding.
 *  @param convention Convention::MARKER or Convention::TYPED accepts the specified style only;
 *                    Convention::EITHER accepts either.
 */
void
setConventionDecoding(Convention convention);

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
    using Block::Error::Error;
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
  fromEscapedString(const std::string& input);

  /**
   * @brief Write *this to the output stream, escaping characters according to the NDN URI format.
   * @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#ndn-uri-scheme
   */
  void
  toUri(std::ostream& os, UriFormat format = UriFormat::DEFAULT) const;

  /**
   * @brief Convert *this by escaping characters according to the NDN URI format.
   * @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#ndn-uri-scheme
   */
  std::string
  toUri(UriFormat format = UriFormat::DEFAULT) const;

public: // naming conventions
  /**
   * @brief Check if the component is a nonNegativeInteger
   * @sa https://named-data.net/doc/NDN-packet-spec/current/tlv.html#non-negative-integer-encoding
   */
  bool
  isNumber() const;

  /**
   * @brief Check if the component is a NameComponentWithMarker per NDN naming conventions rev1
   * @sa NDN Naming Conventions revision 1:
   *     https://named-data.net/wp-content/uploads/2014/08/ndn-tr-22-ndn-memo-naming-conventions.pdf
   */
  bool
  isNumberWithMarker(uint8_t marker) const;

  /**
   * @brief Check if the component is a version per NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   */
  bool
  isVersion() const;

  /**
   * @brief Check if the component is a segment number per NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   */
  bool
  isSegment() const;

  /**
   * @brief Check if the component is a byte offset per NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   */
  bool
  isByteOffset() const;

  /// @deprecated use isByteOffset
  bool
  isSegmentOffset() const
  {
    return isByteOffset();
  }

  /**
   * @brief Check if the component is a timestamp per NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   */
  bool
  isTimestamp() const;

  /**
   * @brief Check if the component is a sequence number per NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
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
   * @sa NDN Naming Conventions revision 1:
   *     https://named-data.net/wp-content/uploads/2014/08/ndn-tr-22-ndn-memo-naming-conventions.pdf
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
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   *
   * @throw tlv::Error not a Version component interpreted by the chosen convention(s).
   */
  uint64_t
  toVersion() const;

  /**
   * @brief Interpret as segment number component using NDN naming conventions
   *
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   *
   * @throw tlv::Error not a Segment component interpreted by the chosen convention(s).
   */
  uint64_t
  toSegment() const;

  /**
   * @brief Interpret as byte offset component using NDN naming conventions
   *
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   *
   * @throw tlv::Error not a ByteOffset component interpreted by the chosen convention(s).
   */
  uint64_t
  toByteOffset() const;

  /// @deprecated use toByteOffset
  uint64_t
  toSegmentOffset() const
  {
    return toByteOffset();
  }

  /**
   * @brief Interpret as timestamp component using NDN naming conventions
   *
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   *
   * @throw tlv::Error not a Timestamp component interpreted by the chosen convention(s).
   */
  time::system_clock::TimePoint
  toTimestamp() const;

  /**
   * @brief Interpret as sequence number component using NDN naming conventions
   *
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   *
   * @throw tlv::Error not a SequenceNumber component interpreted by the chosen convention(s).
   */
  uint64_t
  toSequenceNumber() const;

  /**
   * @brief Create a component encoded as nonNegativeInteger
   *
   * @sa https://named-data.net/doc/NDN-packet-spec/current/tlv.html#non-negative-integer-encoding
   *
   * @param number The non-negative number
   * @param type TLV-TYPE
   */
  static Component
  fromNumber(uint64_t number, uint32_t type = tlv::GenericNameComponent);

  /**
   * @brief Create a component encoded as NameComponentWithMarker
   *
   * NameComponentWithMarker is defined as:
   *
   *     NameComponentWithMarker ::= NAME-COMPONENT-TYPE TLV-LENGTH
   *                                   Marker
   *                                   includedNonNegativeInteger
   *     Marker ::= BYTE
   *     includedNonNegativeInteger ::= BYTE{1,2,4,8}
   *
   * @sa NDN Naming Conventions revision 1:
   *     https://named-data.net/wp-content/uploads/2014/08/ndn-tr-22-ndn-memo-naming-conventions.pdf
   *
   * @param marker 1-byte marker octet
   * @param number The non-negative number
   */
  static Component
  fromNumberWithMarker(uint8_t marker, uint64_t number);

  /**
   * @brief Create version component using NDN naming conventions
   *
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   */
  static Component
  fromVersion(uint64_t version);

  /**
   * @brief Create segment number component using NDN naming conventions
   *
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   */
  static Component
  fromSegment(uint64_t segmentNo);

  /**
   * @brief Create byte offset component using NDN naming conventions
   *
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   */
  static Component
  fromByteOffset(uint64_t offset);

  /// @deprecated use fromByteOffset
  static Component
  fromSegmentOffset(uint64_t offset)
  {
    return fromByteOffset(offset);
  }

  /**
   * @brief Create sequence number component using NDN naming conventions
   *
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
   */
  static Component
  fromTimestamp(const time::system_clock::TimePoint& timePoint);

  /**
   * @brief Create sequence number component using NDN naming conventions
   *
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/
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
  fromImplicitSha256Digest(ConstBufferPtr digest);

  /**
   * @brief Create ImplicitSha256DigestComponent component
   */
  static Component
  fromImplicitSha256Digest(const uint8_t* digest, size_t digestSize);

  /**
   * @brief Check if the component is ParametersSha256DigestComponent
   */
  bool
  isParametersSha256Digest() const;

  /**
   * @brief Create ParametersSha256DigestComponent component
   */
  static Component
  fromParametersSha256Digest(ConstBufferPtr digest);

  /**
   * @brief Create ParametersSha256DigestComponent component
   */
  static Component
  fromParametersSha256Digest(const uint8_t* digest, size_t digestSize);

public: // comparison
  NDN_CXX_NODISCARD bool
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
   * @brief Get the successor of this name component.
   *
   * The successor of a name component is defined as follows:
   *
   *     C represents the set of name components, and X,Y ∈ C.
   *     Operator < is defined by canonical order on C.
   *     Y is the successor of X, if (a) X < Y, and (b) ∄ Z ∈ C s.t. X < Z < Y.
   *
   * In plain words, successor of a name component is the next possible name component.
   *
   * Examples:
   *
   * - successor of `sha256digest=0000000000000000000000000000000000000000000000000000000000000000`
   *   is `sha256digest=0000000000000000000000000000000000000000000000000000000000000001`.
   * - successor of `sha256digest=ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff`
   *   is `params-sha256=0000000000000000000000000000000000000000000000000000000000000000`.
   * - successor of `params-sha256=0000000000000000000000000000000000000000000000000000000000000000`
   *   is `params-sha256=0000000000000000000000000000000000000000000000000000000000000001`.
   * - successor of `params-sha256=ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff`
   *   is `3=...`.
   * - successor of `...` is `%00`.
   * - successor of `A` is `B`.
   * - successor of `%FF` is `%00%00`.
   */
  Component
  getSuccessor() const;

private:
  /**
   * @brief Throw Error if this Component is invalid.
   *
   * A name component is invalid if its TLV-TYPE is outside the [1, 65535] range.
   * Additionally, if it is an ImplicitSha256DigestComponent or a ParametersSha256DigestComponent,
   * its TLV-LENGTH must be 32.
   */
  void
  ensureValid() const;

private: // non-member operators
  // NOTE: the following "hidden friend" operators are available via
  //       argument-dependent lookup only and must be defined inline.

  friend bool
  operator==(const Component& lhs, const Component& rhs)
  {
    return lhs.equals(rhs);
  }

  friend bool
  operator!=(const Component& lhs, const Component& rhs)
  {
    return !lhs.equals(rhs);
  }

  friend bool
  operator<(const Component& lhs, const Component& rhs)
  {
    return lhs.compare(rhs) < 0;
  }

  friend bool
  operator<=(const Component& lhs, const Component& rhs)
  {
    return lhs.compare(rhs) <= 0;
  }

  friend bool
  operator>(const Component& lhs, const Component& rhs)
  {
    return lhs.compare(rhs) > 0;
  }

  friend bool
  operator>=(const Component& lhs, const Component& rhs)
  {
    return lhs.compare(rhs) >= 0;
  }

  friend std::ostream&
  operator<<(std::ostream& os, const Component& component)
  {
    component.toUri(os);
    return os;
  }

  // !!! NOTE TO IMPLEMENTOR !!!
  //
  // This class MUST NOT contain any data fields.
  // Block can be reinterpret_cast'ed as Component type.
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(Component);

} // namespace name
} // namespace ndn

#endif // NDN_NAME_COMPONENT_HPP

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2025 Regents of the University of California.
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

#ifndef NDN_CXX_NAME_COMPONENT_HPP
#define NDN_CXX_NAME_COMPONENT_HPP

#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"
#include "ndn-cxx/util/time.hpp"

namespace ndn::name {

/**
 * @brief Format used for the URI representation of a name.
 * @sa https://docs.named-data.net/NDN-packet-spec/0.3/name.html#ndn-uri-scheme
 */
enum class UriFormat {
  /// Always use `<type-number>=<percent-encoded-value>` format
  CANONICAL,
  /// Always prefer the alternate format when available
  ALTERNATE,
  /// Same as UriFormat::CANONICAL, unless `NDN_NAME_ALT_URI` environment variable is set to '1'
  ENV_OR_CANONICAL,
  /// Same as UriFormat::ALTERNATE, unless `NDN_NAME_ALT_URI` environment variable is set to '0'
  ENV_OR_ALTERNATE,
  /// Use the library's default format; currently equivalent to UriFormat::ENV_OR_ALTERNATE
  DEFAULT = ENV_OR_ALTERNATE,
};

/**
 * @brief Identify a style of NDN Naming Conventions.
 * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
 */
enum class Convention {
  MARKER = 1 << 0, ///< Component markers (revision 1)
  TYPED  = 1 << 1, ///< Typed name components (revision 3)
  EITHER = MARKER | TYPED,
};

/**
 * @brief Name component markers defined in Naming Conventions revision 1 (obsolete).
 * @sa https://named-data.net/publications/techreports/ndn-tr-22-ndn-memo-naming-conventions/
 */
enum : uint8_t {
  SEGMENT_MARKER = 0x00,
  SEGMENT_OFFSET_MARKER = 0xFB,
  VERSION_MARKER = 0xFD,
  TIMESTAMP_MARKER = 0xFC,
  SEQUENCE_NUMBER_MARKER = 0xFE,
};

/**
 * @brief Return which Naming Conventions style to use while encoding.
 *
 * The library default is Convention::TYPED.
 */
Convention
getConventionEncoding() noexcept;

/**
 * @brief Set which Naming Conventions style to use while encoding.
 * @param convention Either Convention::MARKER or Convention::TYPED.
 */
void
setConventionEncoding(Convention convention);

/**
 * @brief Return which Naming Conventions style(s) to accept while decoding.
 *
 * The current library default is Convention::EITHER, but this may change in the future.
 */
Convention
getConventionDecoding() noexcept;

/**
 * @brief Set which Naming Conventions style(s) to accept while decoding.
 * @param convention Convention::MARKER or Convention::TYPED accepts the specified style only;
 *                   Convention::EITHER accepts either.
 */
void
setConventionDecoding(Convention convention);

/**
 * @brief Represents a name component.
 *
 * The Component class provides a read-only view of a Block interpreted as a name component.
 * Although it inherits mutation methods from the Block base class, they must not be used,
 * because the enclosing Name would not be updated correctly.
 *
 * A name component is considered *invalid* if its TLV-TYPE is outside the range `[1, 65535]`,
 * or, if it is an `ImplicitSha256DigestComponent` or a `ParametersSha256DigestComponent`,
 * its TLV-LENGTH is not 32.
 */
class Component : public Block, private boost::less_than_comparable<Component>
{
public:
  class Error : public Block::Error
  {
  public:
    using Block::Error::Error;
  };

public: // constructors
  /**
   * @brief Construct a NameComponent of TLV-TYPE @p type and with empty TLV-VALUE.
   * @throw Error the NameComponent is invalid.
   */
  explicit
  Component(uint32_t type = tlv::GenericNameComponent);

  /**
   * @brief Construct a NameComponent from @p block.
   * @throw Error the NameComponent is invalid.
   */
  explicit
  Component(const Block& block);

  /**
   * @brief Construct a NameComponent of TLV-TYPE @p type, using TLV-VALUE from @p buffer.
   *
   * This constructor does not copy the underlying buffer, but retains a pointer to it.
   * Therefore, the caller must not change the underlying buffer.
   *
   * @throw Error the NameComponent is invalid.
   */
  Component(uint32_t type, ConstBufferPtr buffer);

  /**
   * @brief Construct a GenericNameComponent, using TLV-VALUE from @p buffer.
   *
   * This constructor does not copy the underlying buffer, but retains a pointer to it.
   * Therefore, the caller must not change the underlying buffer.
   *
   * @throw Error the NameComponent is invalid.
   */
  explicit
  Component(ConstBufferPtr buffer)
    : Component(tlv::GenericNameComponent, std::move(buffer))
  {
  }

  /**
   * @brief Construct a NameComponent of TLV-TYPE @p type, copying the TLV-VALUE from @p value.
   */
  Component(uint32_t type, span<const uint8_t> value);

  /**
   * @brief Construct a GenericNameComponent, copying the TLV-VALUE from @p value.
   */
  explicit
  Component(span<const uint8_t> value)
    : Component(tlv::GenericNameComponent, value)
  {
  }

  /**
   * @brief Construct a NameComponent of TLV-TYPE @p type, copying the TLV-VALUE from a range.
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
   * @brief Construct a GenericNameComponent, copying the TLV-VALUE from a range.
   */
  template<class Iterator>
  Component(Iterator first, Iterator last)
    : Component(tlv::GenericNameComponent, first, last)
  {
  }

  /**
   * @brief Construct a GenericNameComponent, copying the TLV-VALUE from a string.
   *
   * Bytes from the string are copied as is, and not interpreted as URI component.
   */
  explicit
  Component(std::string_view str);

public: // encoding and URI
  /**
   * @brief Prepend wire encoding to @p encoder.
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /**
   * @brief Encode to TLV wire format.
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode from TLV wire format.
   */
  void
  wireDecode(const Block& wire);

  /**
   * @brief Construct a NameComponent from its string representation in NDN URI format.
   * @throw Error The input string does not represent a valid NameComponent in NDN URI format.
   * @sa https://docs.named-data.net/NDN-packet-spec/0.3/name.html#ndn-uri-scheme
   */
  static Component
  fromUri(std::string_view input);

  /**
   * @brief Write `*this` to the output stream, escaping characters according to the NDN URI format.
   * @sa https://docs.named-data.net/NDN-packet-spec/0.3/name.html#ndn-uri-scheme
   */
  void
  toUri(std::ostream& os, UriFormat format = UriFormat::DEFAULT) const;

  /**
   * @brief Convert `*this` to a string by escaping characters according to the NDN URI format.
   * @sa https://docs.named-data.net/NDN-packet-spec/0.3/name.html#ndn-uri-scheme
   */
  std::string
  toUri(UriFormat format = UriFormat::DEFAULT) const;

public: // naming conventions
  /**
   * @brief Check if the component is a NonNegativeInteger
   * @sa https://docs.named-data.net/NDN-packet-spec/0.3/tlv.html#non-negative-integer-encoding
   */
  bool
  isNumber() const noexcept;

  /**
   * @brief Check if the component is a NameComponentWithMarker per NDN naming conventions rev1
   * @sa NDN Naming Conventions revision 1 (obsolete)
   *     https://named-data.net/wp-content/uploads/2014/08/ndn-tr-22-ndn-memo-naming-conventions.pdf
   */
  bool
  isNumberWithMarker(uint8_t marker) const noexcept;

  /**
   * @brief Check if the component is a segment number per NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   */
  bool
  isSegment() const noexcept;

  /**
   * @brief Check if the component is a byte offset per NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   */
  bool
  isByteOffset() const noexcept;

  /**
   * @brief Check if the component is a version per NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   */
  bool
  isVersion() const noexcept;

  /**
   * @brief Check if the component is a timestamp per NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   */
  bool
  isTimestamp() const noexcept;

  /**
   * @brief Check if the component is a sequence number per NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   */
  bool
  isSequenceNumber() const noexcept;

  /**
   * @brief Interpret this name component as a NonNegativeInteger
   * @sa https://docs.named-data.net/NDN-packet-spec/0.3/tlv.html#non-negative-integer-encoding
   * @return The decoded non-negative integer.
   */
  uint64_t
  toNumber() const;

  /**
   * @brief Interpret this name component as a NameComponentWithMarker
   *
   * @sa NDN Naming Conventions revision 1 (obsolete)
   *     https://named-data.net/wp-content/uploads/2014/08/ndn-tr-22-ndn-memo-naming-conventions.pdf
   *
   * @param marker 1-byte octet of the marker
   * @return The integer number.
   * @throw Error if name component does not have the specified marker.
   * @throw tlv::Error if format does not follow NameComponentWithMarker specification.
   */
  uint64_t
  toNumberWithMarker(uint8_t marker) const;

  /**
   * @brief Interpret as segment number component using NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   * @throw tlv::Error not a Segment component interpreted by the chosen convention(s).
   */
  uint64_t
  toSegment() const;

  /**
   * @brief Interpret as byte offset component using NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   * @throw tlv::Error not a ByteOffset component interpreted by the chosen convention(s).
   */
  uint64_t
  toByteOffset() const;

  /**
   * @brief Interpret as version component using NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   * @throw tlv::Error not a Version component interpreted by the chosen convention(s).
   */
  uint64_t
  toVersion() const;

  /**
   * @brief Interpret as timestamp component using NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   * @throw tlv::Error not a Timestamp component interpreted by the chosen convention(s).
   */
  time::system_clock::time_point
  toTimestamp() const;

  /**
   * @brief Interpret as sequence number component using NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   * @throw tlv::Error not a SequenceNumber component interpreted by the chosen convention(s).
   */
  uint64_t
  toSequenceNumber() const;

  /**
   * @brief Create a component encoded as NonNegativeInteger
   *
   * @sa https://docs.named-data.net/NDN-packet-spec/0.3/tlv.html#non-negative-integer-encoding
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
   * @sa NDN Naming Conventions revision 1 (obsolete)
   *     https://named-data.net/wp-content/uploads/2014/08/ndn-tr-22-ndn-memo-naming-conventions.pdf
   *
   * @param marker 1-byte marker octet
   * @param number The non-negative number
   */
  static Component
  fromNumberWithMarker(uint8_t marker, uint64_t number);

  /**
   * @brief Create a segment number component using NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   */
  static Component
  fromSegment(uint64_t segmentNo);

  /**
   * @brief Create a byte offset component using NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   */
  static Component
  fromByteOffset(uint64_t offset);

  /**
   * @brief Create a version component using NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   */
  static Component
  fromVersion(uint64_t version);

  /**
   * @brief Create a timestamp component using NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   */
  static Component
  fromTimestamp(const time::system_clock::time_point& timePoint);

  /**
   * @brief Create a sequence number component using NDN naming conventions
   * @sa https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/
   */
  static Component
  fromSequenceNumber(uint64_t seqNo);

public: // commonly used TLV-TYPEs
  /**
   * @brief Check if the component is a GenericNameComponent
   * @sa https://redmine.named-data.net/projects/ndn-tlv/wiki/NameComponentType
   */
  bool
  isGeneric() const noexcept
  {
    return type() == tlv::GenericNameComponent;
  }

  /**
   * @brief Check if the component is an ImplicitSha256DigestComponent
   * @sa https://redmine.named-data.net/projects/ndn-tlv/wiki/NameComponentType
   * @sa https://docs.named-data.net/NDN-packet-spec/0.3/name.html#implicit-digest-component
   */
  bool
  isImplicitSha256Digest() const noexcept;

  /**
   * @brief Check if the component is a ParametersSha256DigestComponent
   * @sa https://redmine.named-data.net/projects/ndn-tlv/wiki/NameComponentType
   * @sa https://docs.named-data.net/NDN-packet-spec/0.3/name.html#parameters-digest-component
   */
  bool
  isParametersSha256Digest() const noexcept;

  /**
   * @brief Check if the component is a KeywordNameComponent
   * @sa https://redmine.named-data.net/projects/ndn-tlv/wiki/NameComponentType
   */
  bool
  isKeyword() const noexcept
  {
    return type() == tlv::KeywordNameComponent;
  }

public: // comparison
  [[nodiscard]] bool
  empty() const noexcept
  {
    return value_size() == 0;
  }

  /**
   * @brief Compare this component to @p other using NDN canonical ordering.
   *
   * @param other The name component to compare with.
   * @retval negative This component comes before @p other in canonical ordering
   * @retval zero This component equals @p other
   * @retval positive This component comes after @p other in canonical ordering
   *
   * @sa https://docs.named-data.net/NDN-packet-spec/0.3/name.html#canonical-order
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
   */
  void
  ensureValid() const;

private: // non-member operators
  // NOTE: the following "hidden friend" operators are available via
  //       argument-dependent lookup only and must be defined inline.
  // Block provides == and != operators.
  // boost::less_than_comparable provides <=, >=, and > operators.

  friend bool
  operator<(const Component& lhs, const Component& rhs)
  {
    return lhs.compare(rhs) < 0;
  }

  friend std::ostream&
  operator<<(std::ostream& os, const Component& component)
  {
    component.toUri(os);
    return os;
  }

  // !!! NOTE TO IMPLEMENTOR !!!
  //
  // This class MUST NOT contain any non-static data members.
  // Block can be reinterpret_cast'ed as Component type.
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(Component);

} // namespace ndn::name

#endif // NDN_CXX_NAME_COMPONENT_HPP

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
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 * @author Zhenkai Zhu <http://irl.cs.ucla.edu/~zhenkai/>
 */

#ifndef NDN_NAME_HPP
#define NDN_NAME_HPP

#include "ndn-cxx/name-component.hpp"

#include <iterator>

namespace ndn {

class Name;

/** @brief Represents an arbitrary sequence of name components
 */
using PartialName = Name;

/** @brief Represents an absolute name
 */
class Name
{
public: // nested types
  using Error = name::Component::Error;

  using Component = name::Component;
  using component_container = std::vector<Component>;

  // Name appears as a container of name components
  using value_type             = Component;
  using allocator_type         = void;
  using reference              = Component&;
  using const_reference        = const Component&;
  using pointer                = Component*;
  using const_pointer          = const Component*;
  using iterator               = const Component*; // disallow modifying via iterator
  using const_iterator         = const Component*;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using difference_type        = component_container::difference_type;
  using size_type              = component_container::size_type;

public: // constructors, encoding, decoding
  /** @brief Create an empty name
   *  @post empty() == true
   */
  Name();

  /** @brief Decode Name from wire encoding
   *  @throw tlv::Error wire encoding is invalid
   *
   *  This is a more efficient equivalent for
   *  @code
   *    Name name;
   *    name.wireDecode(wire);
   *  @endcode
   */
  explicit
  Name(const Block& wire);

  /** @brief Parse name from NDN URI
   *  @param uri a null-terminated URI string
   *  @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#ndn-uri-scheme
   */
  Name(const char* uri);

  /** @brief Create name from NDN URI
   *  @param uri a URI string
   *  @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#ndn-uri-scheme
   */
  Name(std::string uri);

  /** @brief Write URI representation of the name to the output stream
   *  @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#ndn-uri-scheme
   */
  void
  toUri(std::ostream& os, name::UriFormat format = name::UriFormat::DEFAULT) const;

  /** @brief Get URI representation of the name
   *  @return URI representation; "ndn:" scheme identifier is not included
   *  @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#ndn-uri-scheme
   *  @note To print URI representation into a stream, it is more efficient to use ``os << name``.
   */
  std::string
  toUri(name::UriFormat format = name::UriFormat::DEFAULT) const;

  /** @brief Check if this Name instance already has wire encoding
   */
  bool
  hasWire() const
  {
    return m_wire.hasWire();
  }

  /** @brief Fast encoding or block size estimation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** @brief Perform wire encoding, or return existing wire encoding
   *  @post hasWire() == true
   */
  const Block&
  wireEncode() const;

  /** @brief Decode name from wire encoding
   *  @throw tlv::Error wire encoding is invalid
   *  @post hasWire() == true
   */
  void
  wireDecode(const Block& wire);

  /** @brief Make a deep copy of the name, reallocating the underlying memory buffer
   */
  Name
  deepCopy() const;

public: // access
  /** @brief Checks if the name is empty, i.e. has no components.
   */
  NDN_CXX_NODISCARD bool
  empty() const
  {
    return m_wire.elements().empty();
  }

  /** @brief Returns the number of components.
   */
  size_t
  size() const
  {
    return m_wire.elements_size();
  }

  /** @brief Returns an immutable reference to the component at the specified index.
   *  @param i zero-based index of the component to return;
   *           if negative, it is interpreted as offset from the end of the name
   *  @warning No bounds checking is performed, using an out-of-range index is undefined behavior.
   */
  const Component&
  get(ssize_t i) const
  {
    if (i < 0) {
      i += static_cast<ssize_t>(size());
    }
    return reinterpret_cast<const Component&>(m_wire.elements()[i]);
  }

  /** @brief Equivalent to `get(i)`.
   */
  const Component&
  operator[](ssize_t i) const
  {
    return get(i);
  }

  /** @brief Returns an immutable reference to the component at the specified index,
   *         with bounds checking.
   *  @param i zero-based index of the component to return;
   *           if negative, it is interpreted as offset from the end of the name
   *  @throws Error The index is out of bounds.
   */
  const Component&
  at(ssize_t i) const;

  /** @brief Extracts some components as a sub-name (PartialName).
   *  @param iStartComponent zero-based index of the first component;
   *                         if negative, size()+iStartComponent is used instead
   *  @param nComponents number of desired components, starting at @p iStartComponent;
   *                     use @c npos to return all components until the end of the name
   *  @return a new PartialName containing the extracted components
   *
   *  If @p iStartComponent is positive and indexes out of bounds, returns an empty PartialName.
   *  If @p iStartComponent is negative and indexes out of bounds, the sub-name will start from
   *  the beginning of the name instead. If @p nComponents is out of bounds, returns all components
   *  until the end of the name.
   */
  PartialName
  getSubName(ssize_t iStartComponent, size_t nComponents = npos) const;

  /** @brief Returns a prefix of the name.
   *  @param nComponents number of components; if negative, size()+nComponents is used instead
   *
   *  Returns a new PartialName containing a prefix of this name up to `size() - nComponents`.
   *  For example, `getPrefix(-1)` returns the name without the final component.
   */
  PartialName
  getPrefix(ssize_t nComponents) const
  {
    if (nComponents < 0)
      return getSubName(0, size() + nComponents);
    else
      return getSubName(0, nComponents);
  }

public: // iterators
  /** @brief Begin iterator
   */
  const_iterator
  begin() const
  {
    return reinterpret_cast<const_iterator>(m_wire.elements().data());
  }

  /** @brief End iterator
   */
  const_iterator
  end() const
  {
    return reinterpret_cast<const_iterator>(m_wire.elements().data() + m_wire.elements().size());
  }

  /** @brief Reverse begin iterator
   */
  const_reverse_iterator
  rbegin() const
  {
    return const_reverse_iterator(end());
  }

  /** @brief Reverse end iterator
   */
  const_reverse_iterator
  rend() const
  {
    return const_reverse_iterator(begin());
  }

public: // modifiers
  /** @brief Replace the component at the specified index.
   *  @param i zero-based index of the component to replace;
   *           if negative, it is interpreted as offset from the end of the name
   *  @param component the new component to use as a replacement
   *  @return a reference to this name, to allow chaining.
   *  @warning No bounds checking is performed, using an out-of-range index is undefined behavior.
   */
  Name&
  set(ssize_t i, const Component& component);

  /** @brief Replace the component at the specified index.
   *  @param i zero-based index of the component to replace;
   *           if negative, it is interpreted as offset from the end of the name
   *  @param component the new component to use as a replacement
   *  @return a reference to this name, to allow chaining.
   *  @warning No bounds checking is performed, using an out-of-range index is undefined behavior.
   */
  Name&
  set(ssize_t i, Component&& component);

  /** @brief Append a component.
   *  @return a reference to this name, to allow chaining.
   */
  Name&
  append(const Component& component)
  {
    m_wire.push_back(component);
    return *this;
  }

  /** @brief Append a component.
   *  @return a reference to this name, to allow chaining.
   */
  Name&
  append(Component&& component)
  {
    m_wire.push_back(std::move(component));
    return *this;
  }

  /** @brief Append a NameComponent of TLV-TYPE @p type, copying @p count bytes at @p value as
   *         TLV-VALUE.
   *  @return a reference to this name, to allow chaining.
   */
  Name&
  append(uint32_t type, const uint8_t* value, size_t count)
  {
    return append(Component(type, value, count));
  }

  /** @brief Append a GenericNameComponent, copying @p count bytes at @p value as TLV-VALUE.
   *  @return a reference to this name, to allow chaining.
   */
  Name&
  append(const uint8_t* value, size_t count)
  {
    return append(Component(value, count));
  }

  /** @brief Append a NameComponent of TLV-TYPE @p type, copying TLV-VALUE from a range.
   *  @tparam Iterator an @c InputIterator dereferencing to a one-octet value type. More efficient
   *                   implementation is available when it is a @c RandomAccessIterator.
   *  @param type      the TLV-TYPE.
   *  @param first     beginning of the range.
   *  @param last      past-end of the range.
   *  @return a reference to this name, to allow chaining.
   */
  template<class Iterator>
  Name&
  append(uint32_t type, Iterator first, Iterator last)
  {
    return append(Component(type, first, last));
  }

  /** @brief Append a GenericNameComponent, copying TLV-VALUE from a range.
   *  @tparam Iterator an @c InputIterator dereferencing to a one-octet value type. More efficient
   *                   implementation is available when it is a @c RandomAccessIterator.
   *  @param first     beginning of the range.
   *  @param last      past-end of the range.
   *  @return a reference to this name, to allow chaining.
   */
  template<class Iterator>
  Name&
  append(Iterator first, Iterator last)
  {
    return append(Component(first, last));
  }

  /** @brief Append a GenericNameComponent, copying TLV-VALUE from a null-terminated string.
   *  @param str a null-terminated string. Bytes from the string are copied as is, and not
   *             interpreted as URI component.
   *  @return a reference to this name, to allow chaining.
   */
  Name&
  append(const char* str)
  {
    return append(Component(str));
  }

  /** @brief Append a GenericNameComponent from a TLV element.
   *  @param value a TLV element. If its TLV-TYPE is tlv::GenericNameComponent, it is
   *               appended as is. Otherwise, it is nested into a GenericNameComponent.
   *  @return a reference to this name, to allow chaining.
   */
  Name&
  append(Block value)
  {
    if (value.type() == tlv::GenericNameComponent) {
      m_wire.push_back(std::move(value));
    }
    else {
      m_wire.push_back(Block(tlv::GenericNameComponent, std::move(value)));
    }
    return *this;
  }

  /** @brief Append a PartialName.
   *  @param name the components to append
   *  @return a reference to this name, to allow chaining
   */
  Name&
  append(const PartialName& name);

  /** @brief Append a component with a nonNegativeInteger
   *  @sa number the number
   *  @return a reference to this name, to allow chaining
   *  @sa https://named-data.net/doc/NDN-packet-spec/current/tlv.html#non-negative-integer-encoding
   */
  Name&
  appendNumber(uint64_t number)
  {
    return append(Component::fromNumber(number));
  }

  /** @brief Append a component with a marked number
   *  @param marker 1-octet marker
   *  @param number the number
   *
   *  The component is encoded as a 1-octet marker, followed by a nonNegativeInteger.
   *
   *  @return a reference to this name, to allow chaining
   *  @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendNumberWithMarker(uint8_t marker, uint64_t number)
  {
    return append(Component::fromNumberWithMarker(marker, number));
  }

  /** @brief Append a version component
   *  @param version the version number to append; if nullopt, the current UNIX time
   *                 in milliseconds is used
   *  @return a reference to this name, to allow chaining
   *  @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendVersion(optional<uint64_t> version = nullopt);

  /** @brief Append a segment number (sequential) component
   *  @return a reference to this name, to allow chaining
   *  @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendSegment(uint64_t segmentNo)
  {
    return append(Component::fromSegment(segmentNo));
  }

  /** @brief Append a byte offset component
   *  @return a reference to this name, to allow chaining
   *  @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendByteOffset(uint64_t offset)
  {
    return append(Component::fromByteOffset(offset));
  }

  /// @deprecated use appendByteOffset
  Name&
  appendSegmentOffset(uint64_t offset)
  {
    return appendByteOffset(offset);
  }

  /** @brief Append a timestamp component
   *  @param timestamp the timestamp to append; if nullopt, the current system time is used
   *  @return a reference to this name, to allow chaining
   *  @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendTimestamp(optional<time::system_clock::TimePoint> timestamp = nullopt);

  /** @brief Append a sequence number component
   *  @return a reference to this name, to allow chaining
   *  @sa NDN Naming Conventions https://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendSequenceNumber(uint64_t seqNo)
  {
    return append(Component::fromSequenceNumber(seqNo));
  }

  /** @brief Append an ImplicitSha256Digest component.
   *  @return a reference to this name, to allow chaining
   */
  Name&
  appendImplicitSha256Digest(ConstBufferPtr digest)
  {
    return append(Component::fromImplicitSha256Digest(std::move(digest)));
  }

  /** @brief Append an ImplicitSha256Digest component.
   *  @return a reference to this name, to allow chaining
   */
  Name&
  appendImplicitSha256Digest(const uint8_t* digest, size_t digestSize)
  {
    return append(Component::fromImplicitSha256Digest(digest, digestSize));
  }

  /** @brief Append a ParametersSha256Digest component.
   *  @return a reference to this name, to allow chaining
   */
  Name&
  appendParametersSha256Digest(ConstBufferPtr digest)
  {
    return append(Component::fromParametersSha256Digest(std::move(digest)));
  }

  /** @brief Append a ParametersSha256Digest component.
   *  @return a reference to this name, to allow chaining
   */
  Name&
  appendParametersSha256Digest(const uint8_t* digest, size_t digestSize)
  {
    return append(Component::fromParametersSha256Digest(digest, digestSize));
  }

  /** @brief Append a placeholder for a ParametersSha256Digest component.
   *  @return a reference to this name, to allow chaining
   */
  Name&
  appendParametersSha256DigestPlaceholder();

  /** @brief Append a component
   *  @note This makes push_back an alias of append, giving Name a similar API as STL vector.
   */
  template<class T>
  void
  push_back(const T& component)
  {
    append(component);
  }

  /** @brief Erase the component at the specified index.
   *  @param i zero-based index of the component to replace;
   *           if negative, it is interpreted as offset from the end of the name
   *  @warning No bounds checking is performed, using an out-of-range index is undefined behavior.
   */
  void
  erase(ssize_t i);

  /** @brief Remove all components.
   *  @post `empty() == true`
   */
  void
  clear();

public: // algorithms
  /** @brief Get the successor of a name
   *
   *  The successor of a name is defined as follows:
   *
   *      N represents the set of NDN Names, and X,Y ∈ N.
   *      Operator < is defined by canonical order on N.
   *      Y is the successor of X, if (a) X < Y, and (b) ∄ Z ∈ N s.t. X < Z < Y.
   *
   *  In plain words, successor of a name is the same name, but with its last component
   *  advanced to a next possible value.
   *
   *  Examples:
   *
   *  - successor of `/` is
   *    `/sha256digest=0000000000000000000000000000000000000000000000000000000000000000`.
   *  - successor of `/sha256digest=0000000000000000000000000000000000000000000000000000000000000000`
   *    is `/sha256digest=0000000000000000000000000000000000000000000000000000000000000001`.
   *  - successor of `/sha256digest=ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff`
   *    is `/2=...`.
   *  - successor of `/P/A` is `/P/B`.
   *  - successor of `/Q/%FF` is `/Q/%00%00`.
   *
   *  @return a new Name containing the successor
   */
  Name
  getSuccessor() const;

  /** @brief Check if this name is a prefix of another name
   *
   *  This name is a prefix of @p other if the N components of this name are same as the first N
   *  components of @p other.
   *
   *  @retval true this name is a prefix of @p other
   *  @retval false this name is not a prefix of @p other
   */
  bool
  isPrefixOf(const Name& other) const;

  /** @brief Check if this name equals another name
   *
   *  Two names are equal if they have the same number of components, and components at each index
   *  are equal.
   */
  bool
  equals(const Name& other) const;

  /** @brief Compare this to the other Name using NDN canonical ordering.
   *
   *  If the first components of each name are not equal, this returns a negative value if
   *  the first comes before the second using the NDN canonical ordering for name
   *  components, or a positive value if it comes after.  If they are equal, this compares
   *  the second components of each name, etc. If both names are the same up to the size
   *  of the shorter name, this returns a negative value if the first name is shorter than
   *  the second or a positive value if it is longer.  For example, if you std::sort gives:
   *  /a/b/d /a/b/cc /c /c/a /bb .
   *  This is intuitive because all names with the prefix /a are next to each other.
   *  But it may be also be counter-intuitive because /c comes before /bb according
   *  to NDN canonical ordering since it is shorter.
   *
   *  @param other The other Name to compare with.
   *
   *  @retval negative this comes before other in canonical ordering
   *  @retval zero this equals other
   *  @retval positive this comes after other in canonical ordering
   *
   *  @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#canonical-order
   */
  int
  compare(const Name& other) const
  {
    return this->compare(0, npos, other);
  }

  /** @brief compares [pos1, pos1+count1) components in this Name
   *         to [pos2, pos2+count2) components in @p other
   *
   *  Equivalent to `getSubName(pos1, count1).compare(other.getSubName(pos2, count2))`.
   */
  int
  compare(size_t pos1, size_t count1,
          const Name& other, size_t pos2 = 0, size_t count2 = npos) const;

private: // non-member operators
  // NOTE: the following "hidden friend" operators are available via
  //       argument-dependent lookup only and must be defined inline.

  friend bool
  operator==(const Name& lhs, const Name& rhs)
  {
    return lhs.equals(rhs);
  }

  friend bool
  operator!=(const Name& lhs, const Name& rhs)
  {
    return !lhs.equals(rhs);
  }

  friend bool
  operator<(const Name& lhs, const Name& rhs)
  {
    return lhs.compare(rhs) < 0;
  }

  friend bool
  operator<=(const Name& lhs, const Name& rhs)
  {
    return lhs.compare(rhs) <= 0;
  }

  friend bool
  operator>(const Name& lhs, const Name& rhs)
  {
    return lhs.compare(rhs) > 0;
  }

  friend bool
  operator>=(const Name& lhs, const Name& rhs)
  {
    return lhs.compare(rhs) >= 0;
  }

  /** @brief Print URI representation of a name
   *  @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#ndn-uri-scheme
   */
  friend std::ostream&
  operator<<(std::ostream& os, const Name& name)
  {
    name.toUri(os);
    return os;
  }

public:
  /** @brief Indicates "until the end" in getSubName() and compare().
   */
  static const size_t npos;

private:
  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(Name);

/** @brief Parse URI from stream as Name
 *  @sa https://named-data.net/doc/NDN-packet-spec/current/name.html#ndn-uri-scheme
 */
std::istream&
operator>>(std::istream& is, Name& name);

} // namespace ndn

namespace std {

template<>
struct hash<ndn::Name>
{
  size_t
  operator()(const ndn::Name& name) const;
};

} // namespace std

#endif // NDN_NAME_HPP

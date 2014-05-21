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
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 * @author Zhenkai Zhu <http://irl.cs.ucla.edu/~zhenkai/>
 */

#ifndef NDN_NAME_COMPONENT_HPP
#define NDN_NAME_COMPONENT_HPP

#include "common.hpp"
#include "encoding/block.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/encoding-buffer.hpp"
#include "util/string-helper.hpp"

namespace ndn {
namespace name {

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
   * @param wire Tlv::NameComponent Block from which to create name::Component
   * @throws Error if wire.type() is not Tlv::NameComponent
   *
   * Any block can be implicitly converted to name::Component
   */
  Component(const Block& wire);

  /**
   * @brief Create a new name::Component from the buffer pointer (buffer pointer will be copied)
   *
   * @param buffer A pointer to an immutable buffer
   *
   * This constructor will create a new Tlv::NameComponent Block with `buffer` as a payload.
   * Note that this method **will not** allocate new memory for and copy the payload until
   * toWire() method is called.
   */
  explicit
  Component(const ConstBufferPtr& buffer);

  /**
   * @brief Create a new name::Component from the buffer (data from buffer will be copied)
   * @param buffer A reference to the buffer
   *
   * This constructor will create a new Tlv::NameComponent Block with `buffer` as a payload.
   * Note that this method **will** allocate new memory for and copy the payload.
   */
  explicit
  Component(const Buffer& buffer);

  /**
   * @brief Create a new name::Component from the buffer (data from buffer will be copied)
   * @param buffer     A pointer to the first byte of the buffer
   * @param bufferSize Size of the buffer
   *
   * This constructor will create a new Tlv::NameComponent Block with `buffer` as a payload.
   * Note that this method **will** allocate new memory for and copy the payload.
   */
  Component(const uint8_t* buffer, size_t bufferSize);

  /**
   * @brief Create a new name::Component from the buffer (data from buffer will be copied)
   * @param begin Iterator pointing to the beginning of the buffer
   * @param end   Iterator pointing to the ending of the buffer
   *
   * This constructor will create a new Tlv::NameComponent Block with `buffer` as a payload.
   * Note that this method **will** allocate new memory for and copy the payload.
   */
  template<class InputIterator>
  Component(InputIterator begin, InputIterator end);

  /**
   * @brief Create a new name::Component from the C string (data from string will be copied)
   *
   * @param str Zero-ended string.  Note that this string will be interpreted as is (i.e.,
   *            it will not be interpreted as URI)
   *
   * This constructor will create a new Tlv::NameComponent Block with `buffer` as a payload.
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
   * This constructor will create a new Tlv::NameComponent Block with `buffer` as a payload.
   * Note that this method **will** allocate new memory for and copy the payload.
   */
  explicit
  Component(const std::string& str);

  /**
   * @brief Fast encoding or block size estimation
   */
  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& block) const;

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
    return fromEscapedString(escapedString, 0, ::strlen(escapedString));
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
  toUri() const
  {
    std::ostringstream os;
    toUri(os);
    return os.str();
  }

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
   * @brief An alias for toNumber()
   */
  uint64_t
  toVersion() const;

  /**
   * @brief An alias for toNumber()
   */
  uint64_t
  toSegment() const;

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

  bool
  empty() const
  {
    return !hasValue();
  }

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
   * @return 0 If they compare equal, -1 if *this comes before other in the canonical ordering, or
   *         1 if *this comes after other in the canonical ordering.
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

inline
Component::Component()
  : Block(Tlv::NameComponent)
{
}

inline
Component::Component(const Block& wire)
  : Block(wire)
{
  if (type() != Tlv::NameComponent)
    throw Error("Constructing name component from non name component TLV wire block");
}

inline
Component::Component(const ConstBufferPtr& buffer)
  : Block(Tlv::NameComponent, buffer)
{
}

inline
Component::Component(const Buffer& value)
  : Block(dataBlock(Tlv::NameComponent, value.buf(), value.size()))
{
}

inline
Component::Component(const uint8_t* value, size_t valueLen)
  : Block(dataBlock(Tlv::NameComponent, value, valueLen))
{
}

template<class InputIterator>
inline
Component::Component(InputIterator begin, InputIterator end)
  : Block(dataBlock(Tlv::NameComponent, begin, end))
{
}

inline
Component::Component(const char* str)
  : Block(dataBlock(Tlv::NameComponent, str, ::strlen(str)))
{
}

inline
Component::Component(const std::string& str)
  : Block(dataBlock(Tlv::NameComponent, str.c_str(), str.size()))
{
}


inline Component
Component::fromEscapedString(const char* escapedString, size_t beginOffset, size_t endOffset)
{
  std::string trimmedString(escapedString + beginOffset, escapedString + endOffset);
  trim(trimmedString);
  std::string value = unescape(trimmedString);

  if (value.find_first_not_of(".") == std::string::npos) {
    // Special case for component of only periods.
    if (value.size() <= 2)
      // Zero, one or two periods is illegal.  Ignore this component.
      return Component();
    else
      // Remove 3 periods.
      return Component(reinterpret_cast<const uint8_t*>(&value[3]), value.size() - 3);
  }
  else
    return Component(reinterpret_cast<const uint8_t*>(&value[0]), value.size());
}


inline void
Component::toUri(std::ostream& result) const
{
  const uint8_t* valuePtr = value();
  size_t valueSize = value_size();

  bool gotNonDot = false;
  for (unsigned i = 0; i < valueSize; ++i) {
    if (valuePtr[i] != 0x2e) {
      gotNonDot = true;
      break;
    }
  }
  if (!gotNonDot) {
    // Special case for component of zero or more periods.  Add 3 periods.
    result << "...";
    for (size_t i = 0; i < valueSize; ++i)
      result << '.';
  }
  else {
    // In case we need to escape, set to upper case hex and save the previous flags.
    std::ios::fmtflags saveFlags = result.flags(std::ios::hex | std::ios::uppercase);

    for (size_t i = 0; i < valueSize; ++i) {
      uint8_t x = valuePtr[i];
      // Check for 0-9, A-Z, a-z, (+), (-), (.), (_)
      if ((x >= 0x30 && x <= 0x39) || (x >= 0x41 && x <= 0x5a) ||
          (x >= 0x61 && x <= 0x7a) || x == 0x2b || x == 0x2d ||
          x == 0x2e || x == 0x5f)
        result << x;
      else {
        result << '%';
        if (x < 16)
          result << '0';
        result << static_cast<unsigned int>(x);
      }
    }

    // Restore.
    result.flags(saveFlags);
  }
}


inline Component
Component::fromNumber(uint64_t number)
{
  /// \todo Change to Tlv::NumberComponent
  return nonNegativeIntegerBlock(Tlv::NameComponent, number);
}


inline uint64_t
Component::toNumber() const
{
  /// \todo Check if Component is of Tlv::NumberComponent type
  return readNonNegativeInteger(static_cast<const Block&>(*this));
}


inline uint64_t
Component::toVersion() const
{
  return toNumber();
}

inline uint64_t
Component::toSegment() const
{
  return toNumber();
}

inline int
Component::compare(const Component& other) const
{
  // Imitate ndn_Exclude_compareComponents.
  if (value_size() < other.value_size())
    return -1;
  if (value_size() > other.value_size())
    return 1;

  if (value_size() == 0)
    return 0;

  // The components are equal length.  Just do a byte compare.
  return std::memcmp(value(), other.value(), value_size());
}


template<bool T>
inline size_t
Component::wireEncode(EncodingImpl<T>& block) const
{
  size_t totalLength = 0;
  if (value_size() > 0)
    totalLength += block.prependByteArray(value(), value_size());
  totalLength += block.prependVarNumber(value_size());
  totalLength += block.prependVarNumber(Tlv::NameComponent);
  return totalLength;
}

inline const Block&
Component::wireEncode() const
{
  if (this->hasWire())
    return *this;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  const_cast<Component&>(*this) = buffer.block();
  return *this;
}

inline void
Component::wireDecode(const Block& wire)
{
  if (wire.type() != Tlv::NameComponent)
    throw Error("wireDecode name component from non name component TLV wire block");

  *this = wire;
}


} // namespace name
} // namespace ndn

#endif // NDN_NAME_COMPONENT_HPP

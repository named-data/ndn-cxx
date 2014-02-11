/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * @author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NAME_COMPONENT_HPP
#define NDN_NAME_COMPONENT_HPP

#include "common.hpp"
#include "encoding/block.hpp"
#include "encoding/encoding-buffer.hpp"

namespace ndn {
namespace name {

/**
 * A Name::Component holds a read-only name component value.
 */
class Component : public Block
{
public:
  /// @brief Error that can be thrown from the block
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };
  
  /**
   * Create a new Name::Component with a null value.
   */
  Component()
    : Block(Tlv::NameComponent)
  {    
  }

  /**
   * @brief Directly create component from wire block
   *
   * ATTENTION  wire MUST BE of type Tlv::Component. Any other value would cause an exception
   */
  Component(const Block& wire)
    : Block(wire)
  {
    if (type() != Tlv::NameComponent)
      throw Error("Constructing name component from non name component TLV wire block");
  }
  
  /**
   * Create a new Name::Component, taking another pointer to the Blob value.
   * @param value A blob with a pointer to an immutable array.  The pointer is copied.
   */
  Component(const ConstBufferPtr &buffer)
    : Block (Tlv::NameComponent, buffer)
  {
  }
  
  /**
   * Create a new Name::Component, copying the given value.
   * @param value The value byte array.
   */
  Component(const Buffer& value) 
    : Block (Tlv::NameComponent, ConstBufferPtr(new Buffer(value)))
  {
  }

  /**
   * Create a new Name::Component, copying the given value.
   * @param value Pointer to the value byte array.
   * @param valueLen Length of value.
   */
  Component(const uint8_t *value, size_t valueLen) 
    : Block (Tlv::NameComponent, ConstBufferPtr(new Buffer(value, valueLen)))
  {
  }

  template<class InputIterator>
  Component(InputIterator begin, InputIterator end)
    : Block (Tlv::NameComponent, ConstBufferPtr(new Buffer(begin, end)))
  {
  }

  explicit
  Component(const char *str)
    : Block (Tlv::NameComponent, ConstBufferPtr(new Buffer(str, ::strlen(str))))
  {
  }

  explicit
  Component(const std::string& str)
    : Block (Tlv::NameComponent, ConstBufferPtr(new Buffer(str.begin(), str.end())))
  {
  }
  
  /**
   * @brief Fast encoding or block size estimation
   */
  template<bool T>
  size_t
  wireEncode(EncodingImpl<T> &block) const;
  
  /**
   * Make a Blob value by decoding the escapedString between beginOffset and endOffset according to the NDN URI Scheme.
   * If the escaped string is "", "." or ".." then return a Blob with a null pointer, 
   * which means the component should be skipped in a URI name.
   * @param escapedString The escaped string.  It does not need to be null-terminated because we only scan to endOffset.
   * @param beginOffset The offset in escapedString of the beginning of the portion to decode.
   * @param endOffset The offset in escapedString of the end of the portion to decode.
   * @return The Blob value. If the escapedString is not a valid escaped component, then the Blob is a null pointer.
   */
  static Component 
  fromEscapedString(const char *escapedString, size_t beginOffset, size_t endOffset);

  /**
   * Make a Blob value by decoding the escapedString according to the NDN URI Scheme.
   * If the escaped string is "", "." or ".." then return a Blob with a null pointer, 
   * which means the component should be skipped in a URI name.
   * @param escapedString The null-terminated escaped string.
   * @return The Blob value. If the escapedString is not a valid escaped component, then the Blob is a null pointer.
   */
  static Component 
  fromEscapedString(const char *escapedString)
  {
    return fromEscapedString(escapedString, 0, ::strlen(escapedString));
  }

  /**
   * Make a Blob value by decoding the escapedString according to the NDN URI Scheme.
   * If the escaped string is "", "." or ".." then return a Blob with a null pointer, 
   * which means the component should be skipped in a URI name.
   * @param escapedString The escaped string.
   * @return The Blob value. If the escapedString is not a valid escaped component, then the Blob is a null pointer.
   */
  static Component 
  fromEscapedString(const std::string& escapedString)
  {
    return fromEscapedString(escapedString.c_str());
  }

  /**
   * Write the value to result, escaping characters according to the NDN URI Scheme.
   * This also adds "..." to a value with zero or more ".".
   * @param value the buffer with the value to escape
   * @param result the string stream to write to.
   */
  void 
  toEscapedString(std::ostream& result) const;
  
  /**
   * Convert the value by escaping characters according to the NDN URI Scheme.
   * This also adds "..." to a value with zero or more ".".
   * @param value the buffer with the value to escape
   * @return The escaped string.
   */
  inline std::string
  toEscapedString() const
  {
    std::ostringstream result;
    toEscapedString(result);
    return result.str();
  }

  inline void
  toUri(std::ostream& result) const
  {
    return toEscapedString(result);
  }

  inline std::string
  toUri() const
  {
    return toEscapedString();
  }
    
  /**
   * Interpret this name component as a network-ordered number and return an integer.
   * @return The integer number.
   */
  uint64_t
  toNumber() const;

  /**
   * Interpret this name component as a network-ordered number with a marker and return an integer.
   * @param marker The required first byte of the component.
   * @return The integer number.
   * @throw runtime_error If the first byte of the component does not equal the marker.
   */
  uint64_t
  toNumberWithMarker(uint8_t marker) const;
    
  /**
   * Interpret this name component as a segment number according to NDN name conventions (a network-ordered number 
   * where the first byte is the marker 0x00).
   * @return The integer segment number.
   * @throw runtime_error If the first byte of the component is not the expected marker.
   */
  uint64_t
  toSegment() const
  {
    return toNumberWithMarker(0x00);
  }
    
  /**
   * @deprecated Use toSegment.
   */
  uint64_t
  toSeqNum() const
  {
    return toSegment();
  }
        
  /**
   * Interpret this name component as a version number according to NDN name conventions (a network-ordered number 
   * where the first byte is the marker 0xFD).  Note that this returns the exact number from the component
   * without converting it to a time representation.
   * @return The integer segment number.
   * @throw runtime_error If the first byte of the component is not the expected marker.
   */
  uint64_t
  toVersion() const
  {
    return toNumberWithMarker(0xFD);
  }
    
  /**
   * Create a component whose value is the network-ordered encoding of the number.
   * Note: if the number is zero, the result is empty.
   * @param number The number to be encoded.
   * @return The component value.
   */
  static Component 
  fromNumber(uint64_t number);
    
  /**
   * Create a component whose value is the marker appended with the network-ordered encoding of the number.
   * Note: if the number is zero, no bytes are used for the number - the result will have only the marker.
   * @param number The number to be encoded.  
   * @param marker The marker to use as the first byte of the component.
   * @return The component value.
   */
  static Component 
  fromNumberWithMarker(uint64_t number, uint8_t marker);

  /**
   * Check if this is the same component as other.
   * @param other The other Component to compare with.
   * @return true if the components are equal, otherwise false.
   */
  bool
  equals(const Component& other) const
  {
    if (value_size() != other.value_size())
      return false;

    return std::equal(value_begin(), value_end(), other.value_begin());
  }

  bool
  empty() const
  {
    return !hasValue();
  }
    
  /**
   * Check if this is the same component as other.
   * @param other The other Component to compare with.
   * @return true if the components are equal, otherwise false.
   */
  bool
  operator == (const Component& other) const { return equals(other); }

  /**
   * Check if this is not the same component as other.
   * @param other The other Component to compare with.
   * @return true if the components are not equal, otherwise false.
   */
  bool
  operator != (const Component& other) const { return !equals(other); }
    
  /**
   * Compare this to the other Component using NDN canonical ordering.
   * @param other The other Component to compare with.
   * @return 0 If they compare equal, -1 if *this comes before other in the canonical ordering, or
   * 1 if *this comes after other in the canonical ordering.
   *
   * @see http://named-data.net/doc/0.2/technical/CanonicalOrder.html
   */
  int
  compare(const Component& other) const;
  
  /**
   * Return true if this is less than or equal to the other Component in the NDN canonical ordering.
   * @param other The other Component to compare with.
   *
   * @see http://named-data.net/doc/0.2/technical/CanonicalOrder.html
   */
  bool
  operator <= (const Component& other) const { return compare(other) <= 0; }
  
  /**
   * Return true if this is less than the other Component in the NDN canonical ordering.
   * @param other The other Component to compare with.
   *
   * @see http://named-data.net/doc/0.2/technical/CanonicalOrder.html
   */
  bool
  operator < (const Component& other) const { return compare(other) < 0; }

  /**
   * Return true if this is less than or equal to the other Component in the NDN canonical ordering.
   * @param other The other Component to compare with.
   *
   * @see http://named-data.net/doc/0.2/technical/CanonicalOrder.html
   */
  bool
  operator >= (const Component& other) const { return compare(other) >= 0; }

  /**
   * Return true if this is greater than the other Component in the NDN canonical ordering.
   * @param other The other Component to compare with.
   *
   * @see http://named-data.net/doc/0.2/technical/CanonicalOrder.html
   */
  bool
  operator > (const Component& other) const { return compare(other) > 0; }

  // 
  // !!! MUST NOT INCLUDE ANY DATA HERE !!!
  //
  // This class is just a helper and is directly reinterpret_cast'ed from Block
};

inline std::ostream &
operator << (std::ostream &os, const Component &component)
{
  component.toEscapedString(os);
  return os;
}

template<bool T>
inline size_t
Component::wireEncode(EncodingImpl<T>& block) const
{
  size_t total_len = 0;
  if (value_size() > 0)
    total_len += block.prependByteArray (value(), value_size());
  total_len += block.prependVarNumber (value_size());
  total_len += block.prependVarNumber (Tlv::NameComponent);
  return total_len;
}

} // namespace name
} // namespace ndn

#endif // NDN_NAME_COMPONENT_HPP

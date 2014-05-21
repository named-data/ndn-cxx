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

#ifndef NDN_NAME_HPP
#define NDN_NAME_HPP

#include "common.hpp"
#include "name-component.hpp"
#include "util/time.hpp"

#include "encoding/block.hpp"
#include "encoding/encoding-buffer.hpp"

#include <boost/iterator/reverse_iterator.hpp>

namespace ndn {

/**
 * A Name holds an array of Name::Component and represents an NDN name.
 */
class Name : public enable_shared_from_this<Name>
{
public:
  /// @brief Error that can be thrown from Name
  class Error : public name::Component::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : name::Component::Error(what)
    {
    }
  };

  typedef name::Component Component;

  typedef std::vector<Component>  component_container;

  typedef Component               value_type;
  typedef void                    allocator_type;
  typedef Component&              reference;
  typedef const Component         const_reference;
  typedef Component*              pointer;
  typedef const Component*        const_pointer;
  typedef Component*              iterator;
  typedef const Component*        const_iterator;

  typedef boost::reverse_iterator<iterator>       reverse_iterator;
  typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

  typedef component_container::difference_type difference_type;
  typedef component_container::size_type       size_type;

  /**
   * Create a new Name with no components.
   */
  Name()
    : m_nameBlock(Tlv::Name)
  {
  }

  /**
   * @brief Create Name object from wire block
   *
   * This is a more efficient equivalent for
   * @code
   *    Name name;
   *    name.wireDecode(wire);
   * @endcode
   */
  explicit
  Name(const Block& wire)
  {
    m_nameBlock = wire;
    m_nameBlock.parse();
  }

  /**
   * Parse the uri according to the NDN URI Scheme and create the name with the components.
   * @param uri The URI string.
   */
  Name(const char* uri)
  {
    set(uri);
  }

  /**
   * Parse the uri according to the NDN URI Scheme and create the name with the components.
   * @param uri The URI string.
   */
  Name(const std::string& uri)
  {
    set(uri.c_str());
  }

  /**
   * @brief Fast encoding or block size estimation
   */
  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& block) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

  /**
   * @brief Check if already has wire
   */
  bool
  hasWire() const;

  /**
   * Parse the uri according to the NDN URI Scheme and set the name with the components.
   * @param uri The null-terminated URI string.
   */
  void
  set(const char* uri);

  /**
   * Parse the uri according to the NDN URI Scheme and set the name with the components.
   * @param uri The URI string.
   */
  void
  set(const std::string& uri)
  {
    set(uri.c_str());
  }

  /**
   * Append a new component, copying from value of length valueLength.
   * @return This name so that you can chain calls to append.
   */
  Name&
  append(const uint8_t* value, size_t valueLength)
  {
    m_nameBlock.push_back(Component(value, valueLength));
    return *this;
  }

  /**
   * Append a new component, copying from value of length valueLength.
   * @return This name so that you can chain calls to append.
   */
  template<class InputIterator>
  Name&
  append(InputIterator begin, InputIterator end)
  {
    m_nameBlock.push_back(Component(begin, end));
    return *this;
  }

  Name&
  append(const ConstBufferPtr& value)
  {
    m_nameBlock.push_back(value);
    return *this;
  }

  Name&
  append(const Component& value)
  {
    m_nameBlock.push_back(value);
    return *this;
  }

  /**
   * @brief Append name component that represented as a string
   *
   * Note that this method is necessary to ensure correctness and unambiguity of
   * ``append("string")`` operations (both Component and Name can be implicitly
   * converted from string, each having different outcomes
   */
  Name&
  append(const char* value)
  {
    m_nameBlock.push_back(Component(value));
    return *this;
  }

  Name&
  append(const Block& value)
  {
    if (value.type() == Tlv::NameComponent)
      m_nameBlock.push_back(value);
    else
      m_nameBlock.push_back(Block(Tlv::NameComponent, value));

    return *this;
  }

  /**
   * Append the components of the given name to this name.
   * @param name The Name with components to append.
   * @return This name so that you can chain calls to append.
   */
  Name&
  append(const Name& name);

  /**
   * Clear all the components.
   */
  void
  clear()
  {
    m_nameBlock = Block(Tlv::Name);
  }

  /**
   * Get a new name, constructed as a subset of components.
   * @param iStartComponent The index if the first component to get.
   * @param nComponents The number of components starting at iStartComponent.
   * @return A new name.
   */
  Name
  getSubName(size_t iStartComponent, size_t nComponents) const;

  /**
   * Get a new name, constructed as a subset of components starting at iStartComponent until the end of the name.
   * @param iStartComponent The index if the first component to get.
   * @return A new name.
   */
  Name
  getSubName(size_t iStartComponent) const;

  /**
   * Return a new Name with the first nComponents components of this Name.
   * @param nComponents The number of prefix components.  If nComponents is -N then return the prefix up
   * to name.size() - N. For example getPrefix(-1) returns the name without the final component.
   * @return A new Name.
   */
  Name
  getPrefix(int nComponents) const
  {
    if (nComponents < 0)
      return getSubName(0, m_nameBlock.elements_size() + nComponents);
    else
      return getSubName(0, nComponents);
  }

  /**
   * Encode this name as a URI.
   * @return The encoded URI.
   */
  std::string
  toUri() const;

  /**
   * @brief Append a component with the number encoded as nonNegativeInteger
   *
   * @see http://named-data.net/doc/ndn-tlv/tlv.html#non-negative-integer-encoding
   *
   * @param number The non-negative number
   * @return This name so that you can chain calls to append.
   */
  Name&
  appendNumber(uint64_t number)
  {
    m_nameBlock.push_back(Component::fromNumber(number));
    return *this;
  }

  /**
   * @brief An alias for appendNumber(uint64_t)
   */
  Name&
  appendVersion(uint64_t number)
  {
    return appendNumber(number);
  }

  /**
   * @brief Append a component with the encoded version number (current UNIX timestamp
   *        in milliseconds)
   */
  Name&
  appendVersion();

  /**
   * @brief An alias for appendNumber(uint64_t)
   */
  Name&
  appendSegment(uint64_t number)
  {
    return appendNumber(number);
  }

  /**
   * Check if this name has the same component count and components as the given name.
   * @param name The Name to check.
   * @return true if the names are equal, otherwise false.
   */
  bool
  equals(const Name& name) const;

  /**
   * Check if the N components of this name are the same as the first N components of the given name.
   * @param name The Name to check.
   * @return true if this matches the given name, otherwise false.  This always returns true if this name is empty.
   */
  bool
  isPrefixOf(const Name& name) const;

  //
  // vector equivalent interface.
  //

  /**
   * @brief Check if name is emtpy
   */
  bool
  empty() const
  {
    return m_nameBlock.elements().empty();
  }

  /**
   * Get the number of components.
   * @return The number of components.
   */
  size_t
  size() const
  {
    return m_nameBlock.elements_size();
  }

  /**
   * Get the component at the given index.
   * @param i The index of the component, starting from 0.
   * @return The name component at the index.
   */
  const Component&
  get(ssize_t i) const
  {
    if (i >= 0)
      return reinterpret_cast<const Component&>(m_nameBlock.elements()[i]);
    else
      return reinterpret_cast<const Component&>(m_nameBlock.elements()[size() + i]);
  }

  const Component&
  operator[](ssize_t i) const
  {
    return get(i);
  }

  /**
   * @brief Get component at the specified index
   *
   * Unlike get() and operator[] methods, at() checks for out of bounds
   * and will throw Name::Error when it happens
   *
   * @throws Name::Error if index out of bounds
   */
  const Component&
  at(ssize_t i) const
  {
    if ((i >= 0 && static_cast<size_t>(i) >= size()) ||
        (i < 0  && static_cast<size_t>(-i) > size()))
      throw Error("Requested component does not exist (out of bounds)");

    return get(i);
  }

  /**
   * @brief Compare this to the other Name using NDN canonical ordering.
   *
   * If the first components of each name are not equal, this returns -1 if the first comes
   * before the second using the NDN canonical ordering for name components, or 1 if it
   * comes after.  If they are equal, this compares the second components of each name, etc.
   * If both names are the same up to the size of the shorter name, this returns -1 if the
   * first name is shorter than the second or 1 if it is longer.  For example, if you
   * std::sort gives: /a/b/d /a/b/cc /c /c/a /bb .  This is intuitive because all names with
   * the prefix /a are next to each other.  But it may be also be counter-intuitive because
   * /c comes before /bb according to NDN canonical ordering since it is shorter.  @param
   * other The other Name to compare with.  @return 0 If they compare equal, -1 if *this
   * comes before other in the canonical ordering, or 1 if *this comes after other in the
   * canonical ordering.
   *
   * @see http://named-data.net/doc/ndn-tlv/name.html#canonical-order
   */
  int
  compare(const Name& other) const;

  /**
   * Append the component
   * @param component The component of type T.
   */
  template<class T> void
  push_back(const T& component)
  {
    append(component);
  }

  /**
   * Check if this name has the same component count and components as the given name.
   * @param name The Name to check.
   * @return true if the names are equal, otherwise false.
   */
  bool
  operator==(const Name& name) const
  {
    return equals(name);
  }

  /**
   * Check if this name has the same component count and components as the given name.
   * @param name The Name to check.
   * @return true if the names are not equal, otherwise false.
   */
  bool
  operator!=(const Name& name) const
  {
    return !equals(name);
  }

  /**
   * Return true if this is less than or equal to the other Name in the NDN canonical ordering.
   * @param other The other Name to compare with.
   *
   * @see http://named-data.net/doc/ndn-tlv/name.html#canonical-order
   */
  bool
  operator<=(const Name& other) const
  {
    return compare(other) <= 0;
  }

  /**
   * Return true if this is less than the other Name in the NDN canonical ordering.
   * @param other The other Name to compare with.
   *
   * @see http://named-data.net/doc/ndn-tlv/name.html#canonical-order
   */
  bool
  operator<(const Name& other) const
  {
    return compare(other) < 0;
  }

  /**
   * Return true if this is less than or equal to the other Name in the NDN canonical ordering.
   * @param other The other Name to compare with.
   *
   * @see http://named-data.net/doc/ndn-tlv/name.html#canonical-order
   */
  bool
  operator>=(const Name& other) const
  {
    return compare(other) >= 0;
  }

  /**
   * Return true if this is greater than the other Name in the NDN canonical ordering.
   * @param other The other Name to compare with.
   *
   * @see http://named-data.net/doc/ndn-tlv/name.html#canonical-order
   */
  bool
  operator>(const Name& other) const
  {
    return compare(other) > 0;
  }

  //
  // Iterator interface to name components.
  //

  /**
   * Begin iterator (const).
   */
  const_iterator
  begin() const
  {
    return reinterpret_cast<const_iterator>(&*m_nameBlock.elements().begin());
  }

  /**
   * End iterator (const).
   *
   * @todo Check if this crash when there are no elements in the buffer
   */
  const_iterator
  end() const
  {
    return reinterpret_cast<const_iterator>(&*m_nameBlock.elements().end());
  }

  /**
   * Reverse begin iterator (const).
   */
  const_reverse_iterator
  rbegin() const
  {
    return const_reverse_iterator(end());
  }

  /**
   * Reverse end iterator (const).
   */
  const_reverse_iterator
  rend() const
  {
    return const_reverse_iterator(begin());
  }

private:
  mutable Block m_nameBlock;
};

inline std::ostream&
operator<<(std::ostream& os, const Name& name)
{
  if (name.empty())
    {
      os << "/";
    }
  else
    {
      for (Name::const_iterator i = name.begin(); i != name.end(); i++) {
        os << "/";
        i->toUri(os);
      }
    }
  return os;
}

inline std::string
Name::toUri() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

inline std::istream&
operator>>(std::istream& is, Name& name)
{
  std::string inputString;
  is >> inputString;
  name.set(inputString);

  return is;
}


inline void
Name::set(const char* uri_cstr)
{
  clear();

  std::string uri = uri_cstr;
  trim(uri);
  if (uri.size() == 0)
    return;

  size_t iColon = uri.find(':');
  if (iColon != std::string::npos) {
    // Make sure the colon came before a '/'.
    size_t iFirstSlash = uri.find('/');
    if (iFirstSlash == std::string::npos || iColon < iFirstSlash) {
      // Omit the leading protocol such as ndn:
      uri.erase(0, iColon + 1);
      trim(uri);
    }
  }

  // Trim the leading slash and possibly the authority.
  if (uri[0] == '/') {
    if (uri.size() >= 2 && uri[1] == '/') {
      // Strip the authority following "//".
      size_t iAfterAuthority = uri.find('/', 2);
      if (iAfterAuthority == std::string::npos)
        // Unusual case: there was only an authority.
        return;
      else {
        uri.erase(0, iAfterAuthority + 1);
        trim(uri);
      }
    }
    else {
      uri.erase(0, 1);
      trim(uri);
    }
  }

  size_t iComponentStart = 0;

  // Unescape the components.
  while (iComponentStart < uri.size()) {
    size_t iComponentEnd = uri.find("/", iComponentStart);
    if (iComponentEnd == std::string::npos)
      iComponentEnd = uri.size();

    Component component = Component::fromEscapedString(&uri[0], iComponentStart, iComponentEnd);
    // Ignore illegal components.  This also gets rid of a trailing '/'.
    if (!component.empty())
      append(Component(component));

    iComponentStart = iComponentEnd + 1;
  }
}

inline Name&
Name::append(const Name& name)
{
  if (&name == this)
    // Copying from this name, so need to make a copy first.
    return append(Name(name));

  for (size_t i = 0; i < name.size(); ++i)
    append(name.at(i));

  return *this;
}

inline Name&
Name::appendVersion()
{
  appendNumber(time::toUnixTimestamp(time::system_clock::now()).count());
  return *this;
}

inline Name
Name::getSubName(size_t iStartComponent, size_t nComponents) const
{
  Name result;

  size_t iEnd = iStartComponent + nComponents;
  for (size_t i = iStartComponent; i < iEnd && i < size(); ++i)
    result.append(at(i));

  return result;
}

inline Name
Name::getSubName(size_t iStartComponent) const
{
  Name result;

  for (size_t i = iStartComponent; i < size(); ++i)
    result.append(at(i));

  return result;
}

inline bool
Name::equals(const Name& name) const
{
  if (size() != name.size())
    return false;

  for (size_t i = 0; i < size(); ++i) {
    if (at(i) != name.at(i))
      return false;
  }

  return true;
}

inline bool
Name::isPrefixOf(const Name& name) const
{
  // This name is longer than the name we are checking it against.
  if (size() > name.size())
    return false;

  // Check if at least one of given components doesn't match.
  for (size_t i = 0; i < size(); ++i) {
    if (at(i) != name.at(i))
      return false;
  }

  return true;
}


inline int
Name::compare(const Name& other) const
{
  for (size_t i = 0; i < size() && i < other.size(); ++i) {
    int comparison = at(i).compare(other.at(i));
    if (comparison == 0)
      // The components at this index are equal, so check the next components.
      continue;

    // Otherwise, the result is based on the components at this index.
    return comparison;
  }

  // The components up to min(this.size(), other.size()) are equal, so the shorter name is less.
  if (size() < other.size())
    return -1;
  else if (size() > other.size())
    return 1;
  else
    return 0;
}



template<bool T>
inline size_t
Name::wireEncode(EncodingImpl<T>& blk) const
{
  size_t totalLength = 0;

  for (const_reverse_iterator i = rbegin();
       i != rend();
       ++i)
    {
      totalLength += i->wireEncode(blk);
    }

  totalLength += blk.prependVarNumber(totalLength);
  totalLength += blk.prependVarNumber(Tlv::Name);
  return totalLength;
}

inline const Block&
Name::wireEncode() const
{
  if (m_nameBlock.hasWire())
    return m_nameBlock;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_nameBlock = buffer.block();
  m_nameBlock.parse();

  return m_nameBlock;
}

inline void
Name::wireDecode(const Block& wire)
{
  if (wire.type() != Tlv::Name)
    throw Tlv::Error("Unexpected TLV type when decoding Name");

  m_nameBlock = wire;
  m_nameBlock.parse();
}

inline bool
Name::hasWire() const
{
  return m_nameBlock.hasWire();
}

} // namespace ndn

#endif

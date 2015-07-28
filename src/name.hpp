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
 *
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 * @author Zhenkai Zhu <http://irl.cs.ucla.edu/~zhenkai/>
 */

#ifndef NDN_NAME_HPP
#define NDN_NAME_HPP

#include "common.hpp"
#include "name-component.hpp"

#include <boost/iterator/reverse_iterator.hpp>

namespace ndn {

class Name;

/**
 * @brief Partial name abstraction to represent an arbitrary sequence of name components
 */
typedef Name PartialName;

/**
 * @brief Name abstraction to represent an absolute name
 */
class Name : public enable_shared_from_this<Name>
{
public:
  /**
   * @brief Error that can be thrown from Name
   */
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
   * @brief Create a new Name with no components.
   */
  Name();

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
  Name(const Block& wire);

  /**
   * @brief Create name from @p uri (NDN URI scheme)
   * @param uri The null-terminated URI string
   */
  Name(const char* uri);

  /**
   * @brief Create name from @p uri (NDN URI scheme)
   * @param uri The URI string
   */
  Name(const std::string& uri);

  /**
   * @brief Fast encoding or block size estimation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

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
   * @deprecated Use appropriate constructor
   */
  DEPRECATED(
  void
  set(const char* uri));

  /**
   * @deprecated Use appropriate constructor
   */
  DEPRECATED(
  void
  set(const std::string& uri));

  /**
   * @brief Append a new component, copying from value of length valueLength.
   * @return This name so that you can chain calls to append.
   */
  Name&
  append(const uint8_t* value, size_t valueLength)
  {
    m_nameBlock.push_back(Component(value, valueLength));
    return *this;
  }

  /**
   * @brief Append a new component, copying from value frome the range [@p first, @p last) of bytes
   * @param first     Iterator pointing to the beginning of the buffer
   * @param last      Iterator pointing to the ending of the buffer
   * @tparam Iterator iterator type satisfying at least InputIterator concept.  Implementation
   *                  is more optimal when the iterator type satisfies RandomAccessIterator concept.
   *                  It is required that sizeof(std::iterator_traits<Iterator>::value_type) == 1.
   * @return This name so that you can chain calls to append.
   */
  template<class Iterator>
  Name&
  append(Iterator first, Iterator last)
  {
    m_nameBlock.push_back(Component(first, last));
    return *this;
  }

  /**
   * @brief Append component @p value
   */
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
    if (value.type() == tlv::NameComponent)
      m_nameBlock.push_back(value);
    else
      m_nameBlock.push_back(Block(tlv::NameComponent, value));

    return *this;
  }

  /**
   * @brief append a PartialName to this Name.
   * @param name the components to append
   * @return this name
   */
  Name&
  append(const PartialName& name);

  /**
   * Clear all the components.
   */
  void
  clear()
  {
    m_nameBlock = Block(tlv::Name);
  }

  /**
   * @brief Extract a sub-name (PartialName) of @p nComponents components starting
   *        from @p iStartComponent
   * @param iStartComponent index of the first component;
   *        if iStartComponent is negative, size()+iStartComponent is used instead
   * @param nComponents The number of components starting at iStartComponent.
   *                    Use npos to get the Partial Name until the end of this Name.
   * @detail If iStartComponent is out of bounds and is negative, will return the components
   *         starting in the beginning of the Name
   *         If iStartComponent is out of bounds and is positive, will return the component "/"
   *         If nComponents is out of bounds, will return the components until the end of
   *         this Name
   * @return A new partial name
   */
  PartialName
  getSubName(ssize_t iStartComponent, size_t nComponents = npos) const;

  /**
   * @brief Extract a prefix (PartialName) of the name, containing first @p nComponents components
   *
   * @param nComponents The number of prefix components.  If nComponents is -N then return
   *                    the prefix up to name.size() - N. For example getPrefix(-1)
   *                    returns the name without the final component.
   * @return A new partial name
   */
  PartialName
  getPrefix(ssize_t nComponents) const
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
  appendNumber(uint64_t number);

  /**
   * @brief Create a component encoded as NameComponentWithMarker
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   *
   * @param marker 1-byte marker octet
   * @param number The non-negative number
   */
  Name&
  appendNumberWithMarker(uint8_t marker, uint64_t number);

  /**
   * @brief Append version using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendVersion(uint64_t version);

  /**
   * @brief Append version using NDN naming conventions based on current UNIX timestamp
   *        in milliseconds
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendVersion();

  /**
   * @brief Append segment number (sequential) using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendSegment(uint64_t segmentNo);

  /**
   * @brief Append segment byte offset using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendSegmentOffset(uint64_t offset);

  /**
   * @brief Append timestamp using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendTimestamp(const time::system_clock::TimePoint& timePoint = time::system_clock::now());

  /**
   * @brief Append sequence number using NDN naming conventions
   *
   * @see http://named-data.net/doc/tech-memos/naming-conventions.pdf
   */
  Name&
  appendSequenceNumber(uint64_t seqNo);

  /**
   * @brief Append ImplicitSha256Digest
   */
  Name&
  appendImplicitSha256Digest(const ConstBufferPtr& digest);

  /**
   * @brief Append ImplicitSha256Digest
   */
  Name&
  appendImplicitSha256Digest(const uint8_t* digest, size_t digestSize);

  /**
   * @brief Get the successor of a name
   *
   * The successor of a name is defined as follows:
   *
   *     N represents the set of NDN Names, and X,Y ∈ N.
   *     Operator < is defined by canonical order on N.
   *     Y is the successor of X, if (a) X < Y, and (b) ∄ Z ∈ N s.t. X < Z < Y.
   *
   * In plain words, successor of a name is the same name, but with its last component
   * advanced to a next possible value.
   *
   * Examples:
   *
   * - successor for / is /%00
   * - successor for /%00%01/%01%02 is /%00%01/%01%03
   * - successor for /%00%01/%01%FF is /%00%01/%02%00
   * - successor for /%00%01/%FF%FF is /%00%01/%00%00%00
   *
   * @return a new name
   */
  Name
  getSuccessor() const;

  /**
   * Check if this name has the same component count and components as the given name.
   * @param name The Name to check.
   * @return true if the names are equal, otherwise false.
   */
  bool
  equals(const Name& name) const;

  /**
   * @brief Check if the N components of this name are the same as the first N components
   *        of the given name.
   *
   * @param name The Name to check.
   * @return true if this matches the given name, otherwise false.  This always returns
   *              true if this name is empty.
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
      BOOST_THROW_EXCEPTION(Error("Requested component does not exist (out of bounds)"));

    return get(i);
  }

  /**
   * @brief Compare this to the other Name using NDN canonical ordering.
   *
   * If the first components of each name are not equal, this returns a negative value if
   * the first comes before the second using the NDN canonical ordering for name
   * components, or a positive value if it comes after.  If they are equal, this compares
   * the second components of each name, etc. If both names are the same up to the size
   * of the shorter name, this returns a negative value if the first name is shorter than
   * the second or a positive value if it is longer.  For example, if you std::sort gives:
   * /a/b/d /a/b/cc /c /c/a /bb .
   * This is intuitive because all names with the prefix /a are next to each other.
   * But it may be also be counter-intuitive because /c comes before /bb according
   * to NDN canonical ordering since it is shorter.
   *
   * @param other The other Name to compare with.
   *
   * @retval negative this comes before other in canonical ordering
   * @retval zero this equals other
   * @retval positive this comes after other in canonical ordering
   *
   * @see http://named-data.net/doc/ndn-tlv/name.html#canonical-order
   */
  int
  compare(const Name& other) const
  {
    return this->compare(0, npos, other);
  }

  /** \brief compares [pos1, pos1+count1) components in this Name
   *         to [pos2, pos2+count2) components in \p other
   *
   *  This is equivalent to this->getSubName(pos1, count1).compare(other.getSubName(pos2, count2));
   */
  int
  compare(size_t pos1, size_t count1,
          const Name& other, size_t pos2 = 0, size_t count2 = npos) const;

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
  void
  construct(const char* uri);

public:
  /** \brief indicates "until the end" in getSubName and compare
   */
  static const size_t npos;

private:
  mutable Block m_nameBlock;
};

std::ostream&
operator<<(std::ostream& os, const Name& name);

std::istream&
operator>>(std::istream& is, Name& name);

inline bool
Name::hasWire() const
{
  return m_nameBlock.hasWire();
}

} // namespace ndn

namespace std {
template<>
struct hash<ndn::Name>
{
  size_t
  operator()(const ndn::Name& name) const;
};

} // namespace std

#endif

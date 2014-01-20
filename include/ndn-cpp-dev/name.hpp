/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * @author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NAME_HPP
#define NDN_NAME_HPP

#include <vector>
#include <string>
#include <sstream>
#include <string.h>
#include "encoding/block.hpp"

namespace ndn {
    
/**
 * A Name holds an array of Name::Component and represents an NDN name.
 */
class Name {
public:
  /**
   * A Name::Component holds a read-only name component value.
   */
  class Component
  {
  public:
    /**
     * Create a new Name::Component with a null value.
     */
    Component() 
    {    
    }

    // copy constructor OK
  
    /**
     * Create a new Name::Component, taking another pointer to the Blob value.
     * @param value A blob with a pointer to an immutable array.  The pointer is copied.
     */
    Component(const ConstBufferPtr &buffer)
    : value_ (buffer)
    {
    }
  
    /**
     * Create a new Name::Component, copying the given value.
     * @param value The value byte array.
     */
    Component(const Buffer& value) 
      : value_ (new Buffer(value))
    {
    }

    /**
     * Create a new Name::Component, copying the given value.
     * @param value Pointer to the value byte array.
     * @param valueLen Length of value.
     */
    Component(const uint8_t *value, size_t valueLen) 
      : value_ (new Buffer(value, valueLen))
    {
    }

    template<class InputIterator>
    Component(InputIterator begin, InputIterator end)
      : value_ (new Buffer(begin, end))
    {
    }
    
    Component(const char *string)
      : value_ (new Buffer(string, ::strlen(string)))
    {
    }

    const Buffer& 
    getValue() const { return *value_; }

    /**
     * Write this component value to result, escaping characters according to the NDN URI Scheme.
     * This also adds "..." to a value with zero or more ".".
     * @param result the string stream to write to.
     */
    void 
    toEscapedString(std::ostream& result) const
    {
      Name::toEscapedString(*value_, result);
    }

    /**
     * Convert this component value by escaping characters according to the NDN URI Scheme.
     * This also adds "..." to a value with zero or more ".".
     * @return The escaped string.
     */
    std::string
    toEscapedString() const
    {
      return Name::toEscapedString(*value_);
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
      return *value_ == *other.value_;
    }

    bool
    empty() const
    {
      return !value_ || value_->empty();
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
  private:
    ConstBufferPtr value_;
  };

  /**
   * Create a new Name with no components.
   */
  Name() {
  }
  
  /**
   * Create a new Name, copying the name components.
   * @param components A vector of Component
   */
  Name(const std::vector<Component>& components)
  : components_(components)
  {
  }

  Name(const Block &name)
  {
    for (Block::element_const_iterator i = name.getAll().begin();
         i != name.getAll().end();
         ++i)
      {
        append(Component(i->value_begin(), i->value_end()));
      }
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

  const Block &
  wireEncode() const;

  void
  wireDecode(const Block &wire);
  
  /**
   * Parse the uri according to the NDN URI Scheme and set the name with the components.
   * @param uri The null-terminated URI string.
   */
  void 
  set(const char *uri);  

  /**
   * Parse the uri according to the NDN URI Scheme and set the name with the components.
   * @param uri The URI string.
   */
  void 
  set(const std::string& uri) { set(uri.c_str()); }  
  
  /**
   * Append a new component, copying from value of length valueLength.
   * @return This name so that you can chain calls to append.
   */
  Name& 
  append(const uint8_t *value, size_t valueLength) 
  {
    components_.push_back(Component(value, valueLength));
    return *this;
  }

  /**
   * Append a new component, copying from value.
   * @return This name so that you can chain calls to append.
   */
  Name& 
  append(const Buffer& value) 
  {
    components_.push_back(value);
    return *this;
  }
  
  Name& 
  append(const ConstBufferPtr &value)
  {
    components_.push_back(value);
    return *this;
  }
  
  Name& 
  append(const Component &value)
  {
    components_.push_back(value);
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
  append(const char *value)
  {
    components_.push_back(Component(value));
    return *this;
  }
  
  Name&
  append(const Block &value)
  {
    components_.push_back(Component(value.begin(), value.end()));
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
   * @deprecated Use append.
   */
  Name& 
  appendComponent(const uint8_t *value, size_t valueLength) 
  {
    return append(value, valueLength);
  }

  /**
   * @deprecated Use append.
   */
  Name& 
  appendComponent(const Buffer& value) 
  {
    return append(value);
  }
  
  /**
   * @deprecated Use append.
   */
  Name& 
  appendComponent(const ConstBufferPtr &value)
  {
    return append(value);
  }

  /**
   * @deprecated Use append.
   */
  Name& 
  addComponent(const uint8_t *value, size_t valueLength) 
  {
    return append(value, valueLength);
  }

  /**
   * @deprecated Use append.
   */
  Name& 
  addComponent(const Buffer& value) 
  {
    return append(value);
  }
  
  /**
   * @deprecated Use append.
   */
  Name& 
  addComponent(const ConstBufferPtr &value)
  {
    return append(value);
  }
  
  /**
   * Clear all the components.
   */
  void 
  clear() {
    components_.clear();
  }
  
  /**
   * @deprecated use size().
   */
  size_t 
  getComponentCount() const { return size(); }
  
  /**
   * @deprecated Use get(i).
   */
  const Component& 
  getComponent(size_t i) const { return get(i); }
  
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
      return getSubName(0, components_.size() + nComponents);
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
   * Append a component with the encoded segment number.
   * @param segment The segment number.
   * @return This name so that you can chain calls to append.
   */  
  Name& 
  appendSegment(uint64_t segment)
  {
    components_.push_back(Component::fromNumberWithMarker(segment, 0x00));
    return *this;
  }

  /**
   * Append a component with the encoded version number.
   * Note that this encodes the exact value of version without converting from a time representation.
   * @param version The version number.
   * @return This name so that you can chain calls to append.
   */  
  Name& 
  appendVersion(uint64_t version)
  {
    components_.push_back(Component::fromNumberWithMarker(version, 0xFD));
    return *this;
  }

  /**
   * @brief Append a component with the encoded version number.
   * 
   * This version of the method creates version number based on the current timestamp
   * @return This name so that you can chain calls to append.
   */  
  Name& 
  appendVersion();
  
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

  bool
  match(const Name& name) const
  {
    return isPrefixOf(name);
  }
  
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
  fromEscapedString(const char *escapedString);

  /**
   * Make a Blob value by decoding the escapedString according to the NDN URI Scheme.
   * If the escaped string is "", "." or ".." then return a Blob with a null pointer, 
   * which means the component should be skipped in a URI name.
   * @param escapedString The escaped string.
   * @return The Blob value. If the escapedString is not a valid escaped component, then the Blob is a null pointer.
   */
  static Component 
  fromEscapedString(const std::string& escapedString) { return fromEscapedString(escapedString.c_str()); }

  /**
   * Write the value to result, escaping characters according to the NDN URI Scheme.
   * This also adds "..." to a value with zero or more ".".
   * @param value the buffer with the value to escape
   * @param result the string stream to write to.
   */
  static void 
  toEscapedString(const uint8_t *value, size_t valueSize, std::ostream& result);

  inline static void 
  toEscapedString(const std::vector<uint8_t>& value, std::ostream& result)
  {
    toEscapedString(&*value.begin(), value.size(), result);
  }
  
  /**
   * Convert the value by escaping characters according to the NDN URI Scheme.
   * This also adds "..." to a value with zero or more ".".
   * @param value the buffer with the value to escape
   * @return The escaped string.
   */
  inline static std::string
  toEscapedString(const uint8_t *value, size_t valueSize)
  {
    std::ostringstream result;
    toEscapedString(value, valueSize, result);
    return result.str();
  }

  static inline std::string
  toEscapedString(const std::vector<uint8_t>& value)
  {
    return toEscapedString(&*value.begin(), value.size());
  }
  
  //
  // vector equivalent interface.
  //

  /**
   * @brief Check if name is emtpy
   */
  bool
  empty() const { return components_.empty(); }
  
  /**
   * Get the number of components.
   * @return The number of components.
   */
  size_t 
  size() const { return components_.size(); }

  /**
   * Get the component at the given index.
   * @param i The index of the component, starting from 0.
   * @return The name component at the index.
   */
  const Component& 
  get(ssize_t i) const
  {
    if (i >= 0)
      return components_[i];
    else
      return components_[size() + i];
  }
  

  const Component&
  operator [] (int i) const
  {
    return get(i);
  }

  /**
   * Append the component
   * @param component The component of type T.
   */
  template<class T> void
  push_back(const T &component)
  {
    append(component);
  }
  
  /**
   * Check if this name has the same component count and components as the given name.
   * @param name The Name to check.
   * @return true if the names are equal, otherwise false.
   */
  bool
  operator == (const Name &name) const { return equals(name); }

  /**
   * Check if this name has the same component count and components as the given name.
   * @param name The Name to check.
   * @return true if the names are not equal, otherwise false.
   */
  bool
  operator != (const Name &name) const { return !equals(name); }

  /**
   * Compare two names for "less than" using breadth first.  If the first components of each name are not equal, 
   * this returns true if the first comes before the second using the NDN canonical ordering for name components.
   * If they are equal, this compares the second components of each name, etc.  If both names are the same up to
   * the size of the shorter name, this returns true if the first name is shorter than the second.  For example, if you
   * use breadthFirstLess in std::sort, it gives: /a/b/d	/a/b/cc /c /c/a /bb .  This is intuitive because all names
   * with the prefix /a are next to each other.  But it may be also be counter-intuitive because /c comes before /bb 
   * according to NDN canonical ordering since it is shorter.  Note: We don't define this directly as the Name
   * less than operation because there are other valid ways to sort names.
   * @param name1 The first name to compare.
   * @param name2 The second name to compare.
   * @return True if the first name is less than the second using breadth first comparison.
   */
  static bool
  breadthFirstLess(const Name& name1, const Name& name2);
  
  /**
   * Name::BreadthFirstLess is a function object which calls breadthFirstLess, for use as the "less" operator in map, etc.
   * For example, you can use Name as the key type in a map as follows: map<Name, int, Name::BreadthFirstLess>.
   */
  struct BreadthFirstLess {
    bool operator() (const Name& name1, const Name& name2) const { return breadthFirstLess(name1, name2); }
  };
  
  //
  // Iterator interface to name components.
  //
  typedef std::vector<Component>::iterator iterator;
  typedef std::vector<Component>::const_iterator const_iterator;
  typedef std::vector<Component>::reverse_iterator reverse_iterator;
  typedef std::vector<Component>::const_reverse_iterator const_reverse_iterator;
  typedef std::vector<Component>::reference reference;
  typedef std::vector<Component>::const_reference const_reference;

  typedef std::vector<Component>::difference_type difference_type;
  typedef std::vector<Component>::size_type size_type;
  
  typedef std::vector<Component>::value_type value_type;

  /**
   * Begin iterator (const).
   */
  const_iterator
  begin() const { return components_.begin(); }

  /**
   * Begin iterator.
   */
  iterator
  begin() { return components_.begin(); }

  /**
   * End iterator (const).
   */
  const_iterator
  end() const { return components_.end(); }

  /**
   * End iterator.
   */
  iterator
  end() { return components_.end(); }

  /**
   * Reverse begin iterator (const).
   */
  const_reverse_iterator
  rbegin() const { return components_.rbegin(); }

  /**
   * Reverse begin iterator.
   */
  reverse_iterator
  rbegin() { return components_.rbegin(); }

  /**
   * Reverse end iterator (const).
   */
  const_reverse_iterator
  rend() const { return components_.rend(); }

  /**
   * Reverse end iterator.
   */
  reverse_iterator
  rend() { return components_.rend(); }

private:
  std::vector<Component> components_;

  mutable Block wire_;
};

std::ostream &
operator << (std::ostream &os, const Name &name);

inline std::ostream &
operator << (std::ostream &os, const Name::Component &component)
{
  component.toEscapedString(os);
  return os;
}

inline std::string 
Name::toUri() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

}

#endif


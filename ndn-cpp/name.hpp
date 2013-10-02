/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NAME_HPP
#define NDN_NAME_HPP

#include <vector>
#include <string>
#include <sstream>
#include "c/name.h"
#include "encoding/binary-xml-wire-format.hpp"
#include "util/blob.hpp"

namespace ndn {
    
class Name {
public:
  /**
   * A Name::Component is holds a read-only name component value.
   */
  class Component {
  public:
    /**
     * Create a new Name::Component with a null value.
     */
    Component() 
    {    
    }
  
    /**
     * Create a new Name::Component, copying the given value.
     * @param value The value byte array.
     */
    Component(const std::vector<uint8_t>& value) 
    : value_(value)
    {
    }

    /**
     * Create a new Name::Component, copying the given value.
     * @param value Pointer to the value byte array.
     * @param valueLen Length of value.
     */
    Component(const uint8_t *value, size_t valueLen) 
    : value_(value, valueLen)
    {
    }
    
    /**
     * Create a new Name::Component, taking another pointer to the Blob value.
     * @param value A blob with a pointer to an immutable array.  The pointer is copied.
     */
    Component(const Blob &value)
    : value_(value)
    {
    }
  
    /**
     * Set the componentStruct to point to this component, without copying any memory.
     * WARNING: The resulting pointer in componentStruct is invalid after a further use of this object which could reallocate memory.
     * @param componentStruct The C ndn_NameComponent struct to receive the pointer.
     */
    void 
    get(struct ndn_NameComponent& componentStruct) const 
    {
      componentStruct.valueLength = value_.size(); 
      if (value_.size() > 0)
        componentStruct.value = (uint8_t*)value_.buf();
      else
        componentStruct.value = 0;
    }
  
    const Blob& 
    getValue() const { return value_; }

    /**
     * Write this component value to result, escaping characters according to the NDN URI Scheme.
     * This also adds "..." to a value with zero or more ".".
     * @param value the buffer with the value to escape
     * @param result the string stream to write to.
     */
    void 
    toEscapedString(std::ostringstream& result) const
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
     * Make a component value by decoding the escapedString between beginOffset and endOffset according to the NDN URI Scheme.
     * If the escaped string is "", "." or ".." then return a Blob with a null pointer, which means this component value was not changed, and
     * the component should be skipped in a URI name.
     * @param escapedString The escaped string.  It does not need to be null-terminated because we only scan to endOffset.
     * @param beginOffset The offset in escapedString of the beginning of the portion to decode.
     * @param endOffset The offset in escapedString of the end of the portion to decode.
     * @return The component value as a Blob, or a Blob with a null pointer if escapedString is not a valid escaped component.
     */
    static Blob 
    makeFromEscapedString(const char *escapedString, size_t beginOffset, size_t endOffset);
    
    /**
     * Make a component as the encoded segment number.
     * @param segment The segment number.
     * @return The component value as a Blob.
     */
    static Blob 
    makeSegment(unsigned long segment);
  
  private:
    Blob value_;
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
   * Set the nameStruct to point to the components in this name, without copying any memory.
   * WARNING: The resulting pointers in nameStruct are invalid after a further use of this object which could reallocate memory.
   * @param nameStruct A C ndn_Name struct where the components array is already allocated.
   */
  void 
  get(struct ndn_Name& nameStruct) const;
  
  /**
   * Clear this name, and set the components by copying from the name struct.
   * @param nameStruct A C ndn_Name struct
   */
  void 
  set(const struct ndn_Name& nameStruct);
  
  /**
   * Parse the uri according to the NDN URI Scheme and set the name with the components.
   * @param uri The URI string.
   */
  void 
  set(const char *uri);  

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
  append(const std::vector<uint8_t>& value) 
  {
    components_.push_back(value);
    return *this;
  }
  
  Name& 
  append(const Blob &value)
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
  appendComponent(const std::vector<uint8_t>& value) 
  {
    return append(value);
  }
  
  /**
   * @deprecated Use append.
   */
  Name& 
  appendComponent(const Blob &value)
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
  addComponent(const std::vector<uint8_t>& value) 
  {
    return append(value);
  }
  
  /**
   * @deprecated Use append.
   */
  Name& 
  addComponent(const Blob &value)
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
   * Get the number of components.
   * @return The number of components.
   */
  size_t 
  getComponentCount() const {
    return components_.size();
  }
  
  const Component& 
  getComponent(size_t i) const { return components_[i]; }
  
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
   * @param nComponents The number of prefix components.
   * @return A new Name.
   */
  Name
  getPrefix(size_t nComponents) const
  {
    return getSubName(0, nComponents);
  }
  
  /**
   * Encode this name as a URI.
   * @return The encoded URI.
   */
  std::string 
  toUri() const;
  
  /**
   * @deprecated Use toUri().
   */
  std::string 
  to_uri() const 
  {
    return toUri();
  }

  /**
   * Append a component with the encoded segment number.
   * @param segment The segment number.
   * @return This name so that you can chain calls to append.
   */  
  Name& 
  appendSegment(unsigned long segment)
  {
    components_.push_back(Component(Component::makeSegment(segment)));
    return *this;
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
  match(const Name& name) const;
  
  /**
   * Write the value to result, escaping characters according to the NDN URI Scheme.
   * This also adds "..." to a value with zero or more ".".
   * @param value the buffer with the value to escape
   * @param result the string stream to write to.
   */
  static void 
  toEscapedString(const std::vector<uint8_t>& value, std::ostringstream& result);

  /**
   * Convert the value by escaping characters according to the NDN URI Scheme.
   * This also adds "..." to a value with zero or more ".".
   * @param value the buffer with the value to escape
   * @return The escaped string.
   */
  static std::string
  toEscapedString(const std::vector<uint8_t>& value);

private:
  std::vector<Component> components_;
};  

}

#endif


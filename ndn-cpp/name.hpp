/**
 * @author: Jeff Thompson
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
  class Component {
  public:
    /**
     * Create a new Name::Component with an empty value.
     */
    Component() 
    {    
    }
  
    /**
     * Create a new Name::Component, copying the given value.
     * @param value The value byte array.
     */
    Component(const std::vector<unsigned char>& value) 
    : value_(value)
    {
    }

    /**
     * Create a new Name::Component, copying the given value.
     * @param value Pointer to the value byte array.
     * @param valueLen Length of value.
     */
    Component(const unsigned char *value, unsigned int valueLen) 
    : value_(value, valueLen)
    {
    }
  
    /**
     * Set the componentStruct to point to this component, without copying any memory.
     * WARNING: The resulting pointer in componentStruct is invalid after a further use of this object which could reallocate memory.
     * @param componentStruct The C ndn_NameComponent struct to receive the pointer.
     */
    void get(struct ndn_NameComponent& componentStruct) const 
    {
      componentStruct.valueLength = value_.size(); 
      if (value_.size() > 0)
        componentStruct.value = (unsigned char*)value_.buf();
      else
        componentStruct.value = 0;
    }
  
    /**
     * Set this component value by decoding the escapedString between first and last according to the NDN URI Scheme.
     * If the escaped string is "", "." or ".." then return false, which means this component value was not changed, and
     * the component should be skipped in a URI name.
     * @param first Pointer to the beginning of the escaped string
     * @param last Pointer to the first character past the end of the escaped string
     * @return True for success, false if escapedString is not a valid escaped component.
     */
    bool setFromEscapedString(const char *first, const char *last);
  
    const Blob& getValue() const { return value_; }
    
    void setValue(const Blob& value) { value_ = value; }
    
    /**
     * Set this component to the encoded segment number.
     * @param segment The segment number.
     */
    void setSegment(unsigned long segment);
  
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
  Name(const char *uri)
  {
    set(uri);
  }
  
  /**
   * Set the nameStruct to point to the components in this name, without copying any memory.
   * WARNING: The resulting pointers in nameStruct are invalid after a further use of this object which could reallocate memory.
   * @param nameStruct A C ndn_Name struct where the components array is already allocated.
   */
  void get(struct ndn_Name& nameStruct) const;
  
  /**
   * Clear this name, and set the components by copying from the name struct.
   * @param nameStruct A C ndn_Name struct
   */
  void set(const struct ndn_Name& nameStruct);
  
  /**
   * Parse the uri according to the NDN URI Scheme and set the name with the components.
   * @param uri The URI string.
   */
  void set(const char *uri);  

  /**
   * Add a new component, copying from value of length valueLength.
   */
  void addComponent(const unsigned char *value, unsigned int valueLength) {
    components_.push_back(Component(value, valueLength));
  }

  /**
   * Add a new component, copying from value.
   */
  void addComponent(const std::vector<unsigned char>& value) {
    components_.push_back(value);
  }
  
  /**
   * Clear all the components.
   */
  void clear() {
    components_.clear();
  }
  
  /**
   * Get the number of components.
   * @return The number of components.
   */
  unsigned int getComponentCount() const {
    return components_.size();
  }
  
  const Component& getComponent(unsigned int i) const { return components_[i]; }
  
  /**
   * Encode this name as a URI.
   * @return The encoded URI.
   */
  std::string toUri() const;
  
  /**
   * @deprecated Use toUri().
   */
  std::string to_uri() const 
  {
    return toUri();
  }
  
  /**
   * Append a component with the encoded segment number.
   * @param segment The segment number.
   */
  void appendSegment(unsigned long segment)
  {
    components_.push_back(Component());
    components_.back().setSegment(segment);
  }
  
  /**
   * Check if the N components of this name are the same as the first N components of the given name.
   * @param name The Name to check.
   * @return true if this matches the given name, otherwise false.  This always returns true if this name is empty.
   */
  bool match(const Name& name) const;
  
  /**
   * Write the value to result, escaping characters according to the NDN URI Scheme.
   * This also adds "..." to a value with zero or more ".".
   * @param value the buffer with the value to escape
   * @param result the string stream to write to.
   */
  static void toEscapedString(const std::vector<unsigned char>& value, std::ostringstream& result);

private:
  std::vector<Component> components_;
};  

}

#endif


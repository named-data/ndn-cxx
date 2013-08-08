/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NAME_HPP
#define	NDN_NAME_HPP

#include <vector>
#include <string>
#include "c/name.h"
#include "encoding/binary-xml-wire-format.hpp"

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
    Component(const std::vector<unsigned char> &value) 
    : value_(value)
    {
    }

    /**
     * Create a new Name::Component, copying the given value.
     * @param value Pointer to the value byte array.
     * @param valueLen Length of value.
     */
    Component(unsigned char *value, unsigned int valueLen) 
    : value_(value, value + valueLen)
    {
    }
  
    /**
     * Set the componentStruct to point to this component, without copying any memory.
     * WARNING: The resulting pointer in componentStruct is invalid after a further use of this object which could reallocate memory.
     * @param componentStruct The C ndn_NameComponent struct to receive the pointer.
     */
    void get(struct ndn_NameComponent &componentStruct) const 
    {
      componentStruct.value = (unsigned char *)&value_[0];
      componentStruct.valueLength = value_.size(); 
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
  
    const std::vector<unsigned char> &getValue() const { return value_; }
  
  private:
    std::vector<unsigned char> value_;
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
  Name(const std::vector<Component> &components)
  : components_(components)
  {
  }
  
  /**
   * Parse the uri according to the NDN URI Scheme and create the name with the components.
   * @param uri The URI string.
   */
  Name(const char *uri);  
  
  /**
   * Set the nameStruct to point to the components in this name, without copying any memory.
   * WARNING: The resulting pointers in nameStruct are invalid after a further use of this object which could reallocate memory.
   * @param nameStruct A C ndn_Name struct where the components array is already allocated.
   */
  void get(struct ndn_Name &nameStruct) const;
  
  /**
   * Clear this name, and set the components by copying from the name struct.
   * @param nameStruct A C ndn_Name struct
   */
  void set(const struct ndn_Name &nameStruct);
  
  /**
   * Add a new component, copying from value of length valueLength.
   */
  void addComponent(unsigned char *value, unsigned int valueLength) {
    components_.push_back(Component(value, valueLength));
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
  
  const Component &getComponent(unsigned int i) const { return components_[i]; }
  
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

private:
  std::vector<Component> components_;
};  

}

#endif


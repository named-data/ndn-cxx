/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_LOCATOR_HPP
#define NDN_KEY_LOCATOR_HPP

#include "encoding/tlv-element.hpp"
#include "name.hpp"

namespace ndn {

namespace error {
struct KeyLocator : public std::runtime_error { KeyLocator(const std::string &what) : std::runtime_error(what) {} };
} // error

class KeyLocator {
public:
  enum {
    KeyLocator_None = -1,
    KeyLocator_Name = 0
  };
  
  KeyLocator()
    : type_(KeyLocator_None)
  {
  }

  inline bool
  empty() const
  {
    return type_ == KeyLocator_None;
  }
  
  uint32_t 
  getType() const { return type_; }
  
  void 
  setType(uint32_t type) { type_ = type; }
    
  const Block& 
  getValue() const { return value_; }

  void 
  setValue(const Block &value) { value_ = value; }

  ////////////////////////////////////////////////////////
  // Helper methods for different types of key locators
  //
  // For now only Name type is actually supported
  
  Name
  getName() const
  {
    if (type_ != KeyLocator_Name)
      throw error::KeyLocator("Requested Name, but KeyLocator is not of the Name type");

    return Name(getValue());
  }

  void
  setName(const Name &name)
  {
    type_ = KeyLocator_Name;
    value_ = name.wireEncode();
  }
  
private:
  uint32_t type_;
  Block value_;
};

}

#endif

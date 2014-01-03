/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_LOCATOR_HPP
#define NDN_KEY_LOCATOR_HPP

#include "encoding/block.hpp"
#include "name.hpp"

namespace ndn {

class KeyLocator {
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };
  
  enum {
    KeyLocator_None = -1,
    KeyLocator_Name = 0,
    
    KeyLocator_Unknown = 255
  };

  inline
  KeyLocator()
    : type_(KeyLocator_None)
  {
  }

  inline
  KeyLocator(const Name &name);

  inline const Block& 
  wireEncode() const;

  inline void 
  wireDecode(const Block &value);
  
  inline bool
  empty() const
  {
    return type_ == KeyLocator_None;
  }
  
  uint32_t 
  getType() const { return type_; }
      
  ////////////////////////////////////////////////////////
  // Helper methods for different types of key locators
  //
  // For now only Name type is actually supported
  
  inline const Name&
  getName() const;

  inline void
  setName(const Name &name);
  
private:
  uint32_t type_;
  Name name_;
  
  mutable Block wire_;
};

inline
KeyLocator::KeyLocator(const Name &name)
{
  setName(name);
}

inline const Block& 
KeyLocator::wireEncode() const
{
  if (empty())
    throw Error("Wire encoding requested, but KeyLocator is empty");

  if (wire_.hasWire())
    return wire_;
  
  if (type_ != KeyLocator_Name)
    throw Error("Unsupported KeyLocator type");
  
  // KeyLocator
  wire_ = Block(Tlv::KeyLocator);
  wire_.push_back(name_.wireEncode());
  wire_.encode();
  
  return wire_;
}

inline void 
KeyLocator::wireDecode(const Block &value)
{
  wire_ = value;
  wire_.parse();
  
  if (!wire_.getAll().empty() && wire_.getAll().front().type() == Tlv::Name)
    {
      type_ = KeyLocator_Name;
      name_.wireDecode(wire_.getAll().front());
    }
  else
    {
      type_ = KeyLocator_Unknown;
    }
}

inline const Name&
KeyLocator::getName() const
{
  if (type_ != KeyLocator_Name)
    throw Error("Requested Name, but KeyLocator is not of the Name type");

  return name_;
}

inline void
KeyLocator::setName(const Name &name)
{
  type_ = KeyLocator_Name;
  name_ = name;
}


} // namespace ndn

#endif

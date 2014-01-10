/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include <algorithm>
#include <string.h>
#include <ndn-cpp/name.hpp>
#include "c/util/ndn_memory.h"
#include "c/util/time.h"

#include "util/string-helper.hpp"

using namespace std;

namespace ndn {

uint64_t
Name::Component::toNumberWithMarker(uint8_t marker) const
{
  if (empty() || *getValue().begin() != marker)
    throw runtime_error("Name component does not begin with the expected marker");
  
  uint64_t result = 0;
  for (Buffer::const_iterator i = getValue().begin()+1; i != getValue().end(); ++i) {
    result <<= 8;
    result |= *i;
  }
  
  return result;
}

Name::Component 
Name::Component::fromNumber(uint64_t number)
{
  ptr_lib::shared_ptr<Buffer> value(new Buffer);
  
  // First encode in little endian.
  while (number != 0) {
    value->push_back(number & 0xff);
    number >>= 8;
  }
  
  // Make it big endian.
  reverse(value->begin(), value->end());
  return Component(value);
}

Name::Component
Name::Component::fromNumberWithMarker(uint64_t number, uint8_t marker)
{
  ptr_lib::shared_ptr<Buffer> value(new Buffer);
  
  // Add the leading marker.
  value->push_back(marker);
  
  // First encode in little endian.
  while (number != 0) {
    value->push_back(number & 0xff);
    number >>= 8;
  }
  
  // Make it big endian.
  reverse(value->begin() + 1, value->end());
  return Component(value);
}

uint64_t
Name::Component::toNumber() const
{
  uint64_t result = 0;
  for (Buffer::const_iterator i = getValue().begin(); i != getValue().end(); ++i) {
    result <<= 8;
    result |= *i;
  }
  
  return result;
}

int
Name::Component::compare(const Name::Component& other) const
{
  // Imitate ndn_Exclude_compareComponents.
  if (getValue().size() < other.getValue().size())
    return -1;
  if (getValue().size() > other.getValue().size())
    return 1;

  // The components are equal length.  Just do a byte compare.  
  return ndn_memcmp(getValue().buf(), other.getValue().buf(), getValue().size());
}

// const Block &
// Name::wireEncode() const
// {
  
// }

void 
Name::set(const char *uri_cstr) 
{
  components_.clear();
  
  string uri = uri_cstr;
  trim(uri);
  if (uri.size() == 0)
    return;

  size_t iColon = uri.find(':');
  if (iColon != string::npos) {
    // Make sure the colon came before a '/'.
    size_t iFirstSlash = uri.find('/');
    if (iFirstSlash == string::npos || iColon < iFirstSlash) {
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
      if (iAfterAuthority == string::npos)
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
    if (iComponentEnd == string::npos)
      iComponentEnd = uri.size();
    
    Component component(fromEscapedString(&uri[0], iComponentStart, iComponentEnd));
    // Ignore illegal components.  This also gets rid of a trailing '/'.
    if (!component.empty())
      components_.push_back(Component(component));
    
    iComponentStart = iComponentEnd + 1;
  }
}

Name&
Name::append(const Name& name)
{
  if (&name == this)
    // Copying from this name, so need to make a copy first.
    return append(Name(name));

  for (size_t i = 0; i < name.components_.size(); ++i)
    components_.push_back(name.components_[i]);
  
  return *this;
}

Name& 
Name::appendVersion()
{
  appendVersion(ndn_getNowMilliseconds());
  return *this;
}

Name
Name::getSubName(size_t iStartComponent, size_t nComponents) const
{
  Name result;
  
  size_t iEnd = iStartComponent + nComponents;
  for (size_t i = iStartComponent; i < iEnd && i < components_.size(); ++i)
    result.components_.push_back(components_[i]);
  
  return result;
}

Name
Name::getSubName(size_t iStartComponent) const
{
  Name result;
  
  for (size_t i = iStartComponent; i < components_.size(); ++i)
    result.components_.push_back(components_[i]);
  
  return result;
}

bool 
Name::equals(const Name& name) const
{
  if (components_.size() != name.components_.size())
    return false;

  for (size_t i = 0; i < components_.size(); ++i) {
    if (components_[i].getValue() != name.components_[i].getValue())
      return false;
  }

  return true;
}

bool 
Name::isPrefixOf(const Name& name) const
{
  // Imitate ndn_Name_match.
  
  // This name is longer than the name we are checking it against.
  if (components_.size() > name.components_.size())
    return false;

  // Check if at least one of given components doesn't match.
  for (size_t i = 0; i < components_.size(); ++i) {
    if (components_[i].getValue() != name.components_[i].getValue())
      return false;
  }

  return true;
}

Name::Component 
Name::fromEscapedString(const char *escapedString, size_t beginOffset, size_t endOffset)
{
  string trimmedString(escapedString + beginOffset, escapedString + endOffset);
  trim(trimmedString);
  string value = unescape(trimmedString);
        
  if (value.find_first_not_of(".") == string::npos) {
    // Special case for component of only periods.  
    if (value.size() <= 2)
      // Zero, one or two periods is illegal.  Ignore this component.
      return Component();
    else
      // Remove 3 periods.
      return Component((const uint8_t *)&value[3], value.size() - 3); 
  }
  else
    return Component((const uint8_t *)&value[0], value.size()); 
}

Name::Component
Name::fromEscapedString(const char *escapedString)
{
  return fromEscapedString(escapedString, 0, ::strlen(escapedString));
}

void
Name::toEscapedString(const uint8_t *value, size_t valueSize, std::ostream& result)
{
  bool gotNonDot = false;
  for (unsigned i = 0; i < valueSize; ++i) {
    if (value[i] != 0x2e) {
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
    ios::fmtflags saveFlags = result.flags(ios::hex | ios::uppercase);
    
    for (size_t i = 0; i < valueSize; ++i) {
      uint8_t x = value[i];
      // Check for 0-9, A-Z, a-z, (+), (-), (.), (_)
      if ((x >= 0x30 && x <= 0x39) || (x >= 0x41 && x <= 0x5a) ||
          (x >= 0x61 && x <= 0x7a) || x == 0x2b || x == 0x2d || 
          x == 0x2e || x == 0x5f)
        result << x;
      else {
        result << '%';
        if (x < 16)
          result << '0';
        result << (unsigned int)x;
      }
    }
    
    // Restore.
    result.flags(saveFlags);
  }  
}

bool 
Name::breadthFirstLess(const Name& name1, const Name& name2)
{
  for (size_t i = 0; i < name1.size() && i < name2.size(); ++i) {
    if (name1[i] == name2[i])
      // The components at this index are equal, so check the next components.
      continue;
    
    // Otherwise, the result is based on the components at this index.
    return name1[i] < name2[i];
  }
  
  // The components up to min(name1.size(), name2.size()) are equal, so sort on the shorter name.
  return name1.size() < name2.size();
}

std::ostream&
operator << (std::ostream& os, const Name& name)
{
  if (name.empty())
    {
      os << "/";
    }
  else
    {
      for (Name::const_iterator i = name.begin(); i != name.end(); i++) {
        os << "/";
        i->toEscapedString(os);
      }
    }
  
  return os;
}

const Block &
Name::wireEncode() const
{
  if (wire_.hasWire())
    return wire_;

  wire_ = Block(Tlv::Name);
  for (Name::const_iterator i = begin(); i != end(); i++) {
    OBufferStream os;
    Tlv::writeVarNumber(os, Tlv::NameComponent);
    Tlv::writeVarNumber(os, i->getValue().size());
    os.write(reinterpret_cast<const char*>(i->getValue().buf()), i->getValue().size());

    wire_.push_back(Block(os.buf()));
  }
        
  wire_.encode();
  return wire_;
}

void
Name::wireDecode(const Block &wire)
{
  clear();
  
  wire_ = wire;
  wire_.parse();

  components_.clear();
  components_.reserve(wire_.getAll().size());

  for (Block::element_const_iterator i = wire_.getAll().begin();
       i != wire_.getAll().end();
       ++i)
    {
      append(i->value(), i->value_size());
    }
}


}

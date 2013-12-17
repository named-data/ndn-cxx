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
#include "c/name.h"
#include "c/util/ndn_memory.h"

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

static const char *WHITESPACE_CHARS = " \n\r\t";

/**
 * Modify str in place to erase whitespace on the left.
 * @param str
 */
static inline void 
trimLeft(string& str)
{
  size_t found = str.find_first_not_of(WHITESPACE_CHARS);
  if (found != string::npos) {
    if (found > 0)
      str.erase(0, found);
  }
  else
    // All whitespace
    str.clear();    
}

/**
 * Modify str in place to erase whitespace on the right.
 * @param str
 */
static inline void 
trimRight(string& str)
{
  size_t found = str.find_last_not_of(WHITESPACE_CHARS);
  if (found != string::npos) {
    if (found + 1 < str.size())
      str.erase(found + 1);
  }
  else
    // All whitespace
    str.clear();
}

/**
 * Modify str in place to erase whitespace on the left and right.
 * @param str
 */
static void 
trim(string& str)
{
  trimLeft(str);
  trimRight(str);
}

/**
 * Convert the hex character to an integer from 0 to 15, or -1 if not a hex character.
 * @param c
 * @return 
 */
static int 
fromHexChar(uint8_t c)
{
  if (c >= '0' && c <= '9')
    return (int)c - (int)'0';
  else if (c >= 'A' && c <= 'F')
    return (int)c - (int)'A' + 10;
  else if (c >= 'a' && c <= 'f')
    return (int)c - (int)'a' + 10;
  else
    return -1;
}

/**
 * Return a copy of str, converting each escaped "%XX" to the char value.
 * @param str
 */
static string 
unescape(const string& str)
{
  ostringstream result;
  
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] == '%' && i + 2 < str.size()) {
      int hi = fromHexChar(str[i + 1]);
      int lo = fromHexChar(str[i + 2]);
      
      if (hi < 0 || lo < 0)
        // Invalid hex characters, so just keep the escaped string.
        result << str[i] << str[i + 1] << str[i + 2];
      else
        result << (uint8_t)(16 * hi + lo);
      
      // Skip ahead past the escaped value.
      i += 2;
    }
    else
      // Just copy through.
      result << str[i];
  }
  
  return result.str();
}

uint64_t Name::Component::toNumberWithMarker(uint8_t marker) const
{
  struct ndn_NameComponent componentStruct;
  get(componentStruct);
  uint64_t result;
  
  ndn_Error error;
  if ((error = ndn_NameComponent_toNumberWithMarker(&componentStruct, marker, &result)))
    throw runtime_error(ndn_getErrorString(error));
    
  return result;
}

Name::Component 
Name::Component::fromNumber(uint64_t number)
{
  shared_ptr<vector<uint8_t> > value(new vector<uint8_t>());
  
  // First encode in little endian.
  while (number != 0) {
    value->push_back(number & 0xff);
    number >>= 8;
  }
  
  // Make it big endian.
  reverse(value->begin(), value->end());
  return Blob(value);
}

Name::Component 
Name::Component::fromNumberWithMarker(uint64_t number, uint8_t marker)
{
  shared_ptr<vector<uint8_t> > value(new vector<uint8_t>());
  
  // Add the leading marker.
  value->push_back(marker);
  
  // First encode in little endian.
  while (number != 0) {
    value->push_back(number & 0xff);
    number >>= 8;
  }
  
  // Make it big endian.
  reverse(value->begin() + 1, value->end());
  return Blob(value);
}

void 
Name::Component::get(struct ndn_NameComponent& componentStruct) const 
{
  value_.get(componentStruct.value);
}

uint64_t
Name::Component::toNumber() const
{
  struct ndn_NameComponent componentStruct;
  get(componentStruct);
  return ndn_NameComponent_toNumber(&componentStruct);
}

int
Name::Component::compare(const Name::Component& other) const
{
  // Imitate ndn_Exclude_compareComponents.
  if (value_.size() < other.value_.size())
    return -1;
  if (value_.size() > other.value_.size())
    return 1;

  // The components are equal length.  Just do a byte compare.  
  return ndn_memcmp((uint8_t*)value_.buf(), (uint8_t*)other.value_.buf(), value_.size());
}

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
    if (component.getValue())
      components_.push_back(Component(component));
    
    iComponentStart = iComponentEnd + 1;
  }
}

void 
Name::get(struct ndn_Name& nameStruct) const
{
  if (nameStruct.maxComponents < components_.size())
    throw runtime_error("nameStruct.maxComponents must be >= this name getNComponents()");
  
  nameStruct.nComponents = components_.size();
  for (size_t i = 0; i < nameStruct.nComponents; ++i)
    components_[i].get(nameStruct.components[i]);
}
  
void 
Name::set(const struct ndn_Name& nameStruct) 
{
  clear();
  for (size_t i = 0; i < nameStruct.nComponents; ++i)
    append(nameStruct.components[i].value.value, nameStruct.components[i].value.length);  
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

string 
Name::toUri() const
{
  if (components_.size() == 0)
    return "/";
  
  ostringstream result;
  for (size_t i = 0; i < components_.size(); ++i) {
    result << "/";
    toEscapedString(*components_[i].getValue(), result);
  }
  
  return result.str();
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
    if (*components_[i].getValue() != *name.components_[i].getValue())
      return false;
  }

  return true;
}

bool 
Name::match(const Name& name) const
{
  // Imitate ndn_Name_match.
  
  // This name is longer than the name we are checking it against.
  if (components_.size() > name.components_.size())
    return false;

  // Check if at least one of given components doesn't match.
  for (size_t i = 0; i < components_.size(); ++i) {
    if (*components_[i].getValue() != *name.components_[i].getValue())
      return false;
  }

  return true;
}

Blob 
Name::fromEscapedString(const char *escapedString, size_t beginOffset, size_t endOffset)
{
  string trimmedString(escapedString + beginOffset, escapedString + endOffset);
  trim(trimmedString);
  string value = unescape(trimmedString);
        
  if (value.find_first_not_of(".") == string::npos) {
    // Special case for component of only periods.  
    if (value.size() <= 2)
      // Zero, one or two periods is illegal.  Ignore this component.
      return Blob();
    else
      // Remove 3 periods.
      return Blob((const uint8_t *)&value[3], value.size() - 3); 
  }
  else
    return Blob((const uint8_t *)&value[0], value.size()); 
}

Blob 
Name::fromEscapedString(const char *escapedString)
{
  return fromEscapedString(escapedString, 0, ::strlen(escapedString));
}

void 
Name::toEscapedString(const vector<uint8_t>& value, ostringstream& result)
{
  bool gotNonDot = false;
  for (unsigned i = 0; i < value.size(); ++i) {
    if (value[i] != 0x2e) {
      gotNonDot = true;
      break;
    }
  }
  if (!gotNonDot) {
    // Special case for component of zero or more periods.  Add 3 periods.
    result << "...";
    for (size_t i = 0; i < value.size(); ++i)
      result << '.';
  }
  else {
    // In case we need to escape, set to upper case hex and save the previous flags.
    ios::fmtflags saveFlags = result.flags(ios::hex | ios::uppercase);
    
    for (size_t i = 0; i < value.size(); ++i) {
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

string
Name::toEscapedString(const vector<uint8_t>& value)
{
  ostringstream result;
  toEscapedString(value, result);
  return result.str();
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


}

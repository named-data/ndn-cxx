/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "name.hpp"

#include <algorithm>
#include <cstring>

#include "util/time.hpp"
#include "util/string-helper.hpp"

namespace ndn {

void 
Name::set(const char *uri_cstr) 
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

Name&
Name::append(const Name& name)
{
  if (&name == this)
    // Copying from this name, so need to make a copy first.
    return append(Name(name));

  for (size_t i = 0; i < name.size(); ++i)
    append(name.at(i));
  
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
  for (size_t i = iStartComponent; i < iEnd && i < size(); ++i)
    result.append(at(i));
  
  return result;
}

Name
Name::getSubName(size_t iStartComponent) const
{
  Name result;
  
  for (size_t i = iStartComponent; i < size(); ++i)
    result.append(at(i));
  
  return result;
}

bool 
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

bool 
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


int
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

} // namespace ndn

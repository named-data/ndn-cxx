/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * @author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"

#include "name-component.hpp"

#include "util/time.hpp"
#include "util/string-helper.hpp"

namespace ndn {
namespace name {

Component 
Component::fromEscapedString(const char *escapedString, size_t beginOffset, size_t endOffset)
{
  std::string trimmedString(escapedString + beginOffset, escapedString + endOffset);
  trim(trimmedString);
  std::string value = unescape(trimmedString);
        
  if (value.find_first_not_of(".") == std::string::npos) {
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

void
Component::toEscapedString(std::ostream& result) const
{
  const uint8_t *valuePtr = value();
  size_t valueSize = value_size();
  
  bool gotNonDot = false;
  for (unsigned i = 0; i < valueSize; ++i) {
    if (valuePtr[i] != 0x2e) {
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
    std::ios::fmtflags saveFlags = result.flags(std::ios::hex | std::ios::uppercase);
    
    for (size_t i = 0; i < valueSize; ++i) {
      uint8_t x = valuePtr[i];
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

int
Component::compare(const Component& other) const
{
  // Imitate ndn_Exclude_compareComponents.
  if (value_size() < other.value_size())
    return -1;
  if (value_size() > other.value_size())
    return 1;

  if (value_size() == 0)
    return 0;
  
  // The components are equal length.  Just do a byte compare.  
  return std::memcmp(value(), other.value(), value_size());
}

// const Block &
// wireEncode() const
// {
  
// }

} // namespace name
} // namespace ndn

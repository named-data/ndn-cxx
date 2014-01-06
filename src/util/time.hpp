/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TIME_HPP
#define NDN_TIME_HPP

#include <stdexcept>
#include "../c/util/time.h"

namespace ndn {

/**
 * Convert to the ISO string representation of the time.
 * @param time Milliseconds since 1/1/1970.
 * @return The ISO string.
 */
inline std::string
toIsoString(const MillisecondsSince1970& time)
{
  char isoString[25];
  ndn_Error error;
  if ((error = ndn_toIsoString(time, isoString)))
    throw std::runtime_error(ndn_getErrorString(error));
  
  return isoString;
}
  
/**
 * Convert from the ISO string representation to the internal time format.
 * @param isoString The ISO time formatted string. 
 * @return The time in milliseconds since 1/1/1970.
 */
inline MillisecondsSince1970
fromIsoString(const std::string& isoString)
{
  MillisecondsSince1970 milliseconds;
  ndn_Error error;
  if ((error = ndn_fromIsoString(isoString.c_str(), &milliseconds)))
    throw std::runtime_error(ndn_getErrorString(error));
  
  return milliseconds;
}

} // namespace ndn

#endif // NDN_TIME_HPP

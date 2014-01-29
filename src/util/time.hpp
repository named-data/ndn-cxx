/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TIME_HPP
#define NDN_TIME_HPP

#include "common.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace ndn {

const boost::posix_time::ptime UNIX_EPOCH_TIME =
  boost::posix_time::ptime (boost::gregorian::date (1970, boost::gregorian::Jan, 1));

/**
 * @brief Get the current time in milliseconds since 1/1/1970, including fractions of a millisecond
 */
inline MillisecondsSince1970 
getNowMilliseconds()
{
  return (boost::posix_time::microsec_clock::universal_time() - UNIX_EPOCH_TIME).total_milliseconds();
}

inline MillisecondsSince1970 
ndn_getNowMilliseconds()
{
  return getNowMilliseconds();
}


/**
 * Convert to the ISO string representation of the time.
 * @param time Milliseconds since 1/1/1970.
 * @return The ISO string.
 */
inline std::string
toIsoString(const MillisecondsSince1970& time)
{
  boost::posix_time::ptime boostTime = UNIX_EPOCH_TIME + boost::posix_time::milliseconds(time);

  /// @todo Determine whether this is necessary at all
  if ((time % 1000) == 0)
    return boost::posix_time::to_iso_string(boostTime) + ".000000"; 
  else
    return boost::posix_time::to_iso_string(boostTime);
}
  
/**
 * Convert from the ISO string representation to the internal time format.
 * @param isoString The ISO time formatted string. 
 * @return The time in milliseconds since 1/1/1970.
 */
inline MillisecondsSince1970
fromIsoString(const std::string& isoString)
{
  boost::posix_time::ptime boostTime = boost::posix_time::from_iso_string(isoString);
  
  return (boostTime-UNIX_EPOCH_TIME).total_milliseconds();
}

} // namespace ndn

#endif // NDN_TIME_HPP

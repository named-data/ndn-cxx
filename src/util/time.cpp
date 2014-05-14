/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#include "time.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace ndn {
namespace time {

std::string
toIsoString(const system_clock::TimePoint& timePoint)
{
  namespace bpt = boost::posix_time;
  bpt::ptime ptime = bpt::from_time_t(system_clock::to_time_t(timePoint));

  uint64_t micro = duration_cast<microseconds>(timePoint - getUnixEpoch()).count() % 1000000;
  if (micro > 0)
    {
      ptime += bpt::microseconds(micro);
      return bpt::to_iso_string(ptime);
    }
  else
    return bpt::to_iso_string(ptime);
}


system_clock::TimePoint
fromIsoString(const std::string& isoString)
{
  namespace bpt = boost::posix_time;
  static bpt::ptime posixTimeEpoch = bpt::from_time_t(0);

  bpt::ptime ptime = bpt::from_iso_string(isoString);

  system_clock::TimePoint point =
    system_clock::from_time_t((ptime - posixTimeEpoch).total_seconds());
  point += microseconds((ptime - posixTimeEpoch).total_microseconds() % 1000000);
  return point;
}


std::string
toString(const system_clock::TimePoint& timePoint,
         const std::string& format/* = "%Y-%m-%d %H:%M:%S"*/,
         const std::locale& locale/* = std::locale("C")*/)
{
  namespace bpt = boost::posix_time;
  bpt::ptime ptime = bpt::from_time_t(system_clock::to_time_t(timePoint));

  uint64_t micro = duration_cast<microseconds>(timePoint - getUnixEpoch()).count() % 1000000;
  ptime += bpt::microseconds(micro);

  bpt::time_facet* facet = new bpt::time_facet(format.c_str());
  std::ostringstream formattedTimePoint;
  formattedTimePoint.imbue(std::locale(locale, facet));
  formattedTimePoint << ptime;

  return formattedTimePoint.str();
}


system_clock::TimePoint
fromString(const std::string& formattedTimePoint,
           const std::string& format/* = "%Y-%m-%d %H:%M:%S"*/,
           const std::locale& locale/* = std::locale("C")*/)
{
  namespace bpt = boost::posix_time;
  static bpt::ptime posixTimeEpoch = bpt::from_time_t(0);

  bpt::time_input_facet* facet = new bpt::time_input_facet(format);
  std::istringstream is(formattedTimePoint);

  is.imbue(std::locale(locale, facet));
  bpt::ptime ptime;
  is >> ptime;

  system_clock::TimePoint point =
    system_clock::from_time_t((ptime - posixTimeEpoch).total_seconds());
  point += microseconds((ptime - posixTimeEpoch).total_microseconds() % 1000000);
  return point;
}

} // namespace time
} // namespace ndn

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_COMMON_H
#define NDN_COMMON_H

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace ndn
{
typedef boost::posix_time::ptime Time;
typedef boost::posix_time::time_duration TimeInterval;

namespace time
{
inline TimeInterval Seconds (int secs) { return boost::posix_time::seconds (secs); }
inline TimeInterval Milliseconds (int msecs) { return boost::posix_time::milliseconds (msecs); }
inline TimeInterval Microseconds (int musecs) { return boost::posix_time::microseconds (musecs); }

inline TimeInterval Seconds (double fractionalSeconds)
{
  double seconds, microseconds;
  seconds = std::modf (fractionalSeconds, &microseconds);
  microseconds *= 1000000;

  return time::Seconds((int)seconds) + time::Microseconds((int)microseconds);
}

inline Time Now () { return boost::posix_time::microsec_clock::universal_time (); }

const Time UNIX_EPOCH_TIME = Time (boost::gregorian::date (1970, boost::gregorian::Jan, 1));
inline TimeInterval NowUnixTimestamp ()
{
  return TimeInterval (time::Now () - UNIX_EPOCH_TIME);
}
} // time
} // ndn


extern "C" {
#include <ccn/ccn.h>
#include <ccn/charbuf.h>
#include <ccn/keystore.h>
#include <ccn/uri.h>
#include <ccn/bloom.h>
#include <ccn/signing.h>
}
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/exception/all.hpp>
#include <boost/function.hpp>
#include <string>
#include <sstream>
#include <map>
#include <utility>
#include <string.h>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/make_shared.hpp>

namespace ndn {
typedef std::vector<unsigned char> Bytes;
typedef std::vector<std::string>Comps;

typedef boost::shared_ptr<Bytes> BytesPtr;
typedef boost::shared_ptr<const Bytes> ConstBytesPtr;

inline
const unsigned char *
head(const Bytes &bytes)
{
  return &bytes[0];
}

inline
unsigned char *
head (Bytes &bytes)
{
  return &bytes[0];
}

// --- Bytes operations start ---
inline void
readRaw(Bytes &bytes, const unsigned char *src, size_t len)
{
  if (len > 0)
  {
    bytes.resize(len);
    memcpy (head (bytes), src, len);
  }
}

inline BytesPtr
readRawPtr (const unsigned char *src, size_t len)
{
  if (len > 0)
    {
      BytesPtr ret (new Bytes (len));
      memcpy (head (*ret), src, len);

      return ret;
    }
  else
    return BytesPtr ();
}

// Exceptions
typedef boost::error_info<struct tag_errmsg, std::string> error_info_str;

} // ndn
#endif // NDN_COMMON_H

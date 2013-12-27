/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/ndn-cpp-config.h>
#include "asn_ext.hpp"

#if NDN_CPP_HAVE_TIME_H
#include <time.h>
#endif
#if NDN_CPP_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "../../util/time.hpp"

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace CryptoPP;

namespace ndn {

size_t
DEREncodeGeneralTime(CryptoPP::BufferedTransformation &bt, MillisecondsSince1970 time)
{
  if (time < 0)
    throw Asn::Error("Calendar time value out of range");
  else if (time > 2e14)
    // 2e14 is about the year 8300.  We don't want to go over a 4-digit year.
    throw Asn::Error("Calendar time value out of range");
  
  time_t secondsSince1970 = time / 1000;
  struct tm* gmt = gmtime(&secondsSince1970);

  std::string asn1time ((boost::format("%04d%02d%02d%02d%02d%02dZ")
                         % (1900 + gmt->tm_year)
                         % (gmt->tm_mon + 1)
                         % gmt->tm_mday
                         % gmt->tm_hour
                         % gmt->tm_min
                         % gmt->tm_sec).str());
  
  bt.Put(GENERALIZED_TIME);
  size_t lengthBytes = DERLengthEncode(bt, asn1time.size());
  bt.Put(reinterpret_cast<const uint8_t*>(asn1time.c_str()), asn1time.size());
  return 1+lengthBytes+asn1time.size();
}

void
BERDecodeTime(CryptoPP::BufferedTransformation &bt, MillisecondsSince1970 &time)
{
  byte b;
  if (!bt.Get(b) || (b != GENERALIZED_TIME && b != UTC_TIME))
    BERDecodeError();

  size_t bc;
  if (!BERLengthDecode(bt, bc))
    BERDecodeError();

  SecByteBlock time_str(bc);
  if (bc != bt.Get(time_str, bc))
    BERDecodeError();

  std::string str;
  str.assign (time_str.begin(), time_str.end());
  
  if (b == UTC_TIME) {
    if (boost::lexical_cast<int>(str.substr(0,2)) < 50)
      str = "20" + str;
    else
      str = "19" + str;
  }
 
  time = fromIsoString(str.substr(0, 8) + "T" + str.substr(8, 6));
}

} // namespace ndn

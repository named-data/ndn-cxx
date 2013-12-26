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

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace CryptoPP;

namespace ndn {

size_t
DEREncodeGeneralTime(CryptoPP::BufferedTransformation &bt, MillisecondsSince1970 time)
{
#ifndef NDN_CPP_HAVE_GMTIME_SUPPORT
  throw Asn::Error("Time functions are not supported by the standard library");
#endif

  if (time < 0)
    throw Asn::Error("Calendar time value out of range");
  else if (time > 2e14)
    // 2e14 is about the year 8300.  We don't want to go over a 4-digit year.
    throw Asn::Error("Calendar time value out of range");
  
  time_t secondsSince1970 = time / 1000;
  struct tm* gmt = gmtime(&secondsSince1970);

  std::string asn1time ((boost::format("%04d%02d%02d%02d%02d%02d%sZ")
                         % (1900 + gmt->tm_year)
                         % (gmt->tm_mon + 1)
                         % gmt->tm_mday
                         % gmt->tm_hour
                         % gmt->tm_min
                         % gmt->tm_sec).str());
   // = os.str();
  
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

  std::vector<std::string> params;
  std::string current;
  std::locale cLocale("C");
  
  for(uint32_t j = 0; j != time_str.size(); ++j)
    {
      if(std::isdigit(reinterpret_cast<char&>(time_str[j]), cLocale))
        current += time_str[j];
      else
        {
          if(current != "")
            params.push_back(current);
          current.clear();
        }
    }
  if(current != "")
    params.push_back(current);
  
  if(params.size() < 3 || params.size() > 6)
    throw Asn::Error("Invalid time specification " + std::string(time_str.begin(), time_str.end()));

  struct tm gmt;
  gmt.tm_year = boost::lexical_cast<int>(params[0]);
  gmt.tm_mon  = boost::lexical_cast<int>(params[1]) - 1;
  gmt.tm_mday = boost::lexical_cast<int>(params[2]);
  gmt.tm_hour = (params.size() >= 4) ? boost::lexical_cast<int>(params[3]) : 0;
  gmt.tm_min  = (params.size() >= 5) ? boost::lexical_cast<int>(params[4]) : 0;
  gmt.tm_sec  = (params.size() == 6) ? boost::lexical_cast<int>(params[5]) : 0;

  if (b == GENERALIZED_TIME)
    gmt.tm_year -= 1900;

  time = timegm(&gmt) * 1000;
}

} // namespace ndn

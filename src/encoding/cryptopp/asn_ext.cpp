/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "ndn-cpp-config.h"
#include "asn_ext.hpp"
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


  boost::posix_time::ptime boostTime =
    UNIX_EPOCH_TIME + boost::posix_time::milliseconds(time);
  
  const boost::format f = boost::format("%04d%02d%02d%02d%02d%02dZ")
                % boostTime.date().year_month_day().year
                % boostTime.date().year_month_day().month.as_number()
                % boostTime.date().year_month_day().day.as_number()
                % boostTime.time_of_day().hours()
                % boostTime.time_of_day().minutes()
                % boostTime.time_of_day().seconds()
    ;

  std::string asn1time = f.str();
  
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

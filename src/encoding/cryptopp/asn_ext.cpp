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
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#include "asn_ext.hpp"
#include "../../util/time.hpp"

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace CryptoPP;

namespace ndn {

size_t
DEREncodeGeneralTime(CryptoPP::BufferedTransformation& bt,
                     const time::system_clock::TimePoint& time)
{
  std::string str = time::toIsoString(time);
  // For example, 20131226T232254
  // 20131226T232254.100000
  BOOST_ASSERT(str.size() >= 15);
  std::string asn1time = str.substr(0, 8) + str.substr(9,6) + "Z";

  bt.Put(GENERALIZED_TIME);
  size_t lengthBytes = DERLengthEncode(bt, asn1time.size());
  bt.Put(reinterpret_cast<const uint8_t*>(asn1time.c_str()), asn1time.size());
  return 1+lengthBytes+asn1time.size();
}

void
BERDecodeTime(CryptoPP::BufferedTransformation& bt,
              time::system_clock::TimePoint& time)
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

  time = time::fromIsoString(str.substr(0, 8) + "T" + str.substr(8, 6));
}

} // namespace ndn

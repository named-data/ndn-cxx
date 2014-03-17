/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_ASN_EXT_HPP
#define NDN_ASN_EXT_HPP

#include <stdexcept>
#include "../../common.hpp"
#include <cryptopp/asn.h>

namespace ndn {

namespace Asn {
struct Error : public std::runtime_error { Error(const std::string& what) : std::runtime_error(what) {} };
}

size_t
DEREncodeGeneralTime(CryptoPP::BufferedTransformation& bt, const time::system_clock::TimePoint& time);

void
BERDecodeTime(CryptoPP::BufferedTransformation& bt, time::system_clock::TimePoint& time);

} // namespace ndn

#endif // NDN_ASN_EXT_HPP


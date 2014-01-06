/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_ASN_EXT_HPP
#define NDN_ASN_EXT_HPP

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
#pragma clang diagnostic ignored "-Wtautological-compare"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder" 
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <stdexcept>
#include <ndn-cpp/common.hpp>
#include <cryptopp/asn.h>

namespace ndn {

namespace Asn {
struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };
}

size_t
DEREncodeGeneralTime(CryptoPP::BufferedTransformation &bt, MillisecondsSince1970 time);

void
BERDecodeTime(CryptoPP::BufferedTransformation &bt, MillisecondsSince1970 &time);

} // namespace ndn

#endif // NDN_ASN_EXT_HPP


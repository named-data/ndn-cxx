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
 * @author: Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 * @author: Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#ifndef NDN_ASN_EXT_HPP
#define NDN_ASN_EXT_HPP

#include "../../common.hpp"
#include "../../security/cryptopp.hpp"

#include "../../util/time.hpp"

namespace ndn {

size_t
DEREncodeGeneralTime(CryptoPP::BufferedTransformation& bt,
                     const time::system_clock::TimePoint& time);

void
BERDecodeTime(CryptoPP::BufferedTransformation& bt,
              time::system_clock::TimePoint& time);

} // namespace ndn

#endif // NDN_ASN_EXT_HPP

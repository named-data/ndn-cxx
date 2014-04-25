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

#ifndef NDN_SECURITY_OPENSSL_HPP
#define NDN_SECURITY_OPENSSL_HPP

// suppress deprecation warnings in OSX >= 10.7

#if defined(__APPLE__)

#ifdef __clang__
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif // __clang__

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif // __GNUC__

#endif


#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>


#endif // NDN_SECURITY_OPENSSL_HPP

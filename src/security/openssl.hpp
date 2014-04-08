/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2014 Regents of the University of California.
 * See COPYING for copyright and distribution information.
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

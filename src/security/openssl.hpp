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
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif // __clang__

#ifdef __GNUC__
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif // __GNUC__

#endif


#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>


#if defined(__APPLE__)

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif // __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)

#endif


#endif // NDN_SECURITY_OPENSSL_HPP

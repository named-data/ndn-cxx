/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_COMMON_HPP
#define NDN_COMMON_HPP

#include "ndn-cpp-config.h"
#include <stdint.h>
// TODO: Is stddef.h portable?
#include <stddef.h>

#include <vector>

#if NDN_CPP_HAVE_CXX11

#if (__cplusplus < 201103L)
#error "NDN-CPP-DEV library is configured and compiled in C++11 mode, but the current compiler is not C++11 enabled"
#endif

#include <memory>
namespace ndn { namespace ptr_lib = std; }

// #if NDN_CPP_HAVE_STD_FUNCTION
#include <functional>
namespace ndn { namespace func_lib = std; }

#else

// #if NDN_CPP_HAVE_BOOST_SHARED_PTR
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
namespace ndn { namespace ptr_lib = boost; }

// #if NDN_CPP_HAVE_BOOST_FUNCTION
#include <boost/function.hpp>
#include <boost/bind.hpp>
namespace ndn { namespace func_lib = boost; }

#endif

namespace ndn {

/**
 * A time interval represented as the number of milliseconds.
 */
typedef int64_t Milliseconds;
   
/**
 * The calendar time represented as the number of milliseconds since 1/1/1970.
 */
typedef int64_t MillisecondsSince1970;

/**
 * Return the hex representation of the bytes in array.
 * @param array The array of bytes.
 * @return Hex string.
 */
std::string 
toHex(const std::vector<uint8_t>& array);

MillisecondsSince1970
getNow();

}

#endif

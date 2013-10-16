/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_COMMON_HPP
#define NDN_COMMON_HPP

#include <vector>
// common.h include ndn-cpp-config.h.
#include "c/common.h"

// Depending on where ./configure found shared_ptr, define the ptr_lib namespace.
// We always use ndn::ptr_lib.
#if NDN_CPP_HAVE_STD_SHARED_PTR
#include <memory>
namespace ndn { namespace ptr_lib = std; }
#elif NDN_CPP_HAVE_BOOST_SHARED_PTR
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
namespace ndn { namespace ptr_lib = boost; }
#else
// Use the boost header files in this distribution that were extracted with:
// cd <BOOST DEVELOPMENT DIRECTORY WITH boost SUBDIRECTORY>
// dist/bin/bcp --namespace=ndnboost shared_ptr make_shared weak_ptr function bind any <NDN-CPP ROOT>/include
// cd <NDN-CPP ROOT>/include
// mv boost ndnboost
// cd ndnboost
// # Replace when including files.
// (unset LANG; find . -type f -exec sed -i '' 's/\<boost\//\<ndnboost\//g' {} +)
// (unset LANG; find . -type f -exec sed -i '' 's/\"boost\//\"ndnboost\//g' {} +)
// (unset LANG; find . -type f -exec sed -i '' 's/ boost\// ndnboost\//g' {} +)
// (unset LANG; find . -type f -exec sed -i '' 's/(boost\//(ndnboost\//g' {} +)
// # Replace macro definitions.
// (unset LANG; find . -type f -exec sed -i '' 's/BOOST_/NDNBOOST_/g' {} +)
// # Replace header include guards which don't start with BOOST_ .  This may result in some with NDNBOOST twice, but that is OK.
// (unset LANG; find . -type f -exec sed -i '' 's/_DWA/_NDNBOOST_DWA/g' {} +)
// (unset LANG; find . -type f -exec sed -i '' 's/ UUID_/ NDNBOOST_UUID_/g' {} +)
// (unset LANG; find . -type f -exec sed -i '' 's/ FILE_boost/ FILE_ndnboost/g' {} +)
#include <ndnboost/shared_ptr.hpp>
#include <ndnboost/make_shared.hpp>
namespace ndn { namespace ptr_lib = ndnboost; }
#endif

// Depending on where ./configure found function, define the func_lib namespace.
// We always use ndn::func_lib.
#if NDN_CPP_HAVE_STD_FUNCTION
#include <functional>
namespace ndn { namespace func_lib = std; }
#elif NDN_CPP_HAVE_BOOST_FUNCTION
#include <boost/function.hpp>
#include <boost/bind.hpp>
namespace ndn { namespace func_lib = boost; }
#else
// Use the boost header files in this distribution that were extracted as above:
#include <ndnboost/function.hpp>
#include <ndnboost/bind.hpp>
namespace ndn { namespace func_lib = ndnboost; }
#endif

namespace ndn {
   
// TODO: Implement Time, used in certificates and DER encoding.
typedef double Time;  
  
/**
 * Return the hex representation of the bytes in array.
 * @param array The array of bytes.
 * @return Hex string.
 */
std::string 
toHex(const std::vector<uint8_t>& array);

}

#endif

//  (C) Copyright John Maddock 2001 - 2002. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  SGI C++ compiler setup:

#define NDNBOOST_COMPILER "SGI Irix compiler version " NDNBOOST_STRINGIZE(_COMPILER_VERSION)

#include "ndnboost/config/compiler/common_edg.hpp"

//
// Threading support:
// Turn this on unconditionally here, it will get turned off again later
// if no threading API is detected.
//
#define NDNBOOST_HAS_THREADS
#define NDNBOOST_NO_TWO_PHASE_NAME_LOOKUP

#undef NDNBOOST_NO_SWPRINTF
#undef NDNBOOST_DEDUCED_TYPENAME

//
// version check:
// probably nothing to do here?



//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2012-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#include <ndnboost/config.hpp>

#ifdef NDNBOOST_MSVC
   #ifndef _CRT_SECURE_NO_DEPRECATE
      #define  NDNBOOST_MOVE_CRT_SECURE_NO_DEPRECATE
      #define _CRT_SECURE_NO_DEPRECATE
   #endif
   #ifndef _SCL_SECURE_NO_WARNINGS
      #define  NDNBOOST_MOVE_SCL_SECURE_NO_WARNINGS
      #define _SCL_SECURE_NO_WARNINGS
   #endif
   #pragma warning (push)
   #pragma warning (disable : 4996) // "function": was declared deprecated
#endif

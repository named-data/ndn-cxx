// Copyright John Maddock 2008.

// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_MATH_SPECIAL_ROUND_FWD_HPP
#define NDNBOOST_MATH_SPECIAL_ROUND_FWD_HPP

#include <ndnboost/config.hpp>
#include <ndnboost/math/tools/promotion.hpp>

#ifdef _MSC_VER
#pragma once
#endif

namespace ndnboost
{
   namespace math
   { 

   template <class T, class Policy>
   typename tools::promote_args<T>::type trunc(const T& v, const Policy& pol);
   template <class T>
   typename tools::promote_args<T>::type trunc(const T& v);
   template <class T, class Policy>
   int itrunc(const T& v, const Policy& pol);
   template <class T>
   int itrunc(const T& v);
   template <class T, class Policy>
   long ltrunc(const T& v, const Policy& pol);
   template <class T>
   long ltrunc(const T& v);
#ifdef NDNBOOST_HAS_LONG_LONG
   template <class T, class Policy>
   ndnboost::long_long_type lltrunc(const T& v, const Policy& pol);
   template <class T>
   ndnboost::long_long_type lltrunc(const T& v);
#endif
   template <class T, class Policy>
   typename tools::promote_args<T>::type round(const T& v, const Policy& pol);
   template <class T>
   typename tools::promote_args<T>::type round(const T& v);
   template <class T, class Policy>
   int iround(const T& v, const Policy& pol);
   template <class T>
   int iround(const T& v);
   template <class T, class Policy>
   long lround(const T& v, const Policy& pol);
   template <class T>
   long lround(const T& v);
#ifdef NDNBOOST_HAS_LONG_LONG
   template <class T, class Policy>
   ndnboost::long_long_type llround(const T& v, const Policy& pol);
   template <class T>
   ndnboost::long_long_type llround(const T& v);
#endif
   template <class T, class Policy>
   T modf(const T& v, T* ipart, const Policy& pol);
   template <class T>
   T modf(const T& v, T* ipart);
   template <class T, class Policy>
   T modf(const T& v, int* ipart, const Policy& pol);
   template <class T>
   T modf(const T& v, int* ipart);
   template <class T, class Policy>
   T modf(const T& v, long* ipart, const Policy& pol);
   template <class T>
   T modf(const T& v, long* ipart);
#ifdef NDNBOOST_HAS_LONG_LONG
   template <class T, class Policy>
   T modf(const T& v, ndnboost::long_long_type* ipart, const Policy& pol);
   template <class T>
   T modf(const T& v, ndnboost::long_long_type* ipart);
#endif

   }
}

#undef NDNBOOST_MATH_STD_USING
#define NDNBOOST_MATH_STD_USING NDNBOOST_MATH_STD_USING_CORE\
   using ndnboost::math::round;\
   using ndnboost::math::iround;\
   using ndnboost::math::lround;\
   using ndnboost::math::trunc;\
   using ndnboost::math::itrunc;\
   using ndnboost::math::ltrunc;\
   using ndnboost::math::modf;


#endif // NDNBOOST_MATH_SPECIAL_ROUND_FWD_HPP


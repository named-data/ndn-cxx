
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_IS_INTEGRAL_HPP_INCLUDED
#define NDNBOOST_TT_IS_INTEGRAL_HPP_INCLUDED

#include <ndnboost/config.hpp>

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

//* is a type T an [cv-qualified-] integral type described in the standard (3.9.1p3)
// as an extension we include long long, as this is likely to be added to the
// standard at a later date
#if defined( __CODEGEARC__ )
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_integral,T,__is_integral(T))
#else
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_integral,T,false)

NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned char,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned short,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned int,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned long,true)

NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,signed char,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,signed short,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,signed int,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,signed long,true)

NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,bool,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,char,true)

#ifndef NDNBOOST_NO_INTRINSIC_WCHAR_T
// If the following line fails to compile and you're using the Intel
// compiler, see http://lists.boost.org/MailArchives/boost-users/msg06567.php,
// and define NDNBOOST_NO_INTRINSIC_WCHAR_T on the command line.
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,wchar_t,true)
#endif

// Same set of integral types as in ndnboost/type_traits/integral_promotion.hpp.
// Please, keep in sync. -- Alexander Nasonov
#if (defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC < 1300)) \
    || (defined(NDNBOOST_INTEL_CXX_VERSION) && defined(_MSC_VER) && (NDNBOOST_INTEL_CXX_VERSION <= 600)) \
    || (defined(__BORLANDC__) && (__BORLANDC__ == 0x600) && (_MSC_VER < 1300))
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned __int8,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,__int8,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned __int16,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,__int16,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned __int32,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,__int32,true)
#ifdef __BORLANDC__
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned __int64,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,__int64,true)
#endif
#endif

# if defined(NDNBOOST_HAS_LONG_LONG)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral, ::ndnboost::ulong_long_type,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral, ::ndnboost::long_long_type,true)
#elif defined(NDNBOOST_HAS_MS_INT64)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned __int64,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,__int64,true)
#endif
        
#ifdef NDNBOOST_HAS_INT128
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,ndnboost::int128_type,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,ndnboost::uint128_type,true)
#endif

#endif  // non-CodeGear implementation

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_INTEGRAL_HPP_INCLUDED

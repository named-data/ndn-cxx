
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifdef __WAVE__
// this file has been generated from the master.hpp file in the same directory
#   pragma wave option(preserve: 0)
#endif

#if !defined(NDNBOOST_FT_PREPROCESSING_MODE)
#   error "this file is only for two-pass preprocessing"
#endif

#if !defined(NDNBOOST_PP_VALUE)
#   include <ndnboost/preprocessor/slot/slot.hpp>
#   include <ndnboost/preprocessor/facilities/empty.hpp>
#   include <ndnboost/preprocessor/facilities/expand.hpp>
#   include <ndnboost/function_types/detail/encoding/def.hpp>

NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_mfp 0
NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_syntax NDNBOOST_FT_type_function

#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_function|NDNBOOST_FT_non_variadic
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_function|NDNBOOST_FT_variadic
#   include __FILE__

NDNBOOST_PP_EXPAND(#) if !NDNBOOST_FT_NO_CV_FUNC_SUPPORT
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_function|NDNBOOST_FT_non_variadic|NDNBOOST_FT_const
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_function|NDNBOOST_FT_variadic|NDNBOOST_FT_const
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_function|NDNBOOST_FT_non_variadic|NDNBOOST_FT_volatile
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_function|NDNBOOST_FT_variadic|NDNBOOST_FT_volatile
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_function|NDNBOOST_FT_non_variadic|NDNBOOST_FT_const|NDNBOOST_FT_volatile
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_function|NDNBOOST_FT_variadic|NDNBOOST_FT_const|NDNBOOST_FT_volatile
#   include __FILE__
NDNBOOST_PP_EXPAND(#) endif


NDNBOOST_PP_EXPAND(#) undef  NDNBOOST_FT_syntax
NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_syntax NDNBOOST_FT_type_function_pointer

#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_pointer|NDNBOOST_FT_non_variadic
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_pointer|NDNBOOST_FT_variadic
#   include __FILE__

NDNBOOST_PP_EXPAND(#) undef  NDNBOOST_FT_syntax
NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_syntax NDNBOOST_FT_type_function_reference

#   define NDNBOOST_PP_VALUE \
      NDNBOOST_FT_reference|NDNBOOST_FT_non_variadic
#   include __FILE__
#   define NDNBOOST_PP_VALUE \
      NDNBOOST_FT_reference|NDNBOOST_FT_variadic
#   include __FILE__

NDNBOOST_PP_EXPAND(#) undef  NDNBOOST_FT_syntax
NDNBOOST_PP_EXPAND(#) undef  NDNBOOST_FT_mfp

NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_mfp 1
NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_syntax NDNBOOST_FT_type_member_function_pointer

#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_member_function_pointer|NDNBOOST_FT_non_variadic
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_member_function_pointer|NDNBOOST_FT_variadic
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_member_function_pointer|NDNBOOST_FT_non_variadic|NDNBOOST_FT_const
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_member_function_pointer|NDNBOOST_FT_variadic|NDNBOOST_FT_const
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_member_function_pointer|NDNBOOST_FT_non_variadic|NDNBOOST_FT_volatile
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_member_function_pointer|NDNBOOST_FT_variadic|NDNBOOST_FT_volatile
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_member_function_pointer|NDNBOOST_FT_non_variadic|NDNBOOST_FT_const|NDNBOOST_FT_volatile
#   include __FILE__
#   define  NDNBOOST_PP_VALUE \
      NDNBOOST_FT_member_function_pointer|NDNBOOST_FT_variadic|NDNBOOST_FT_const|NDNBOOST_FT_volatile
#   include __FILE__

NDNBOOST_PP_EXPAND(#) undef  NDNBOOST_FT_syntax
NDNBOOST_PP_EXPAND(#) undef  NDNBOOST_FT_mfp

#   include <ndnboost/function_types/detail/encoding/undef.hpp>
#else 

#   include NDNBOOST_PP_ASSIGN_SLOT(1)

#   define  NDNBOOST_PP_VALUE NDNBOOST_PP_SLOT(1) & NDNBOOST_FT_kind_mask
#   include NDNBOOST_PP_ASSIGN_SLOT(2)

NDNBOOST_PP_EXPAND(#) if !!(NDNBOOST_PP_SLOT(2) & (NDNBOOST_FT_variations))
NDNBOOST_PP_EXPAND(#) if (NDNBOOST_PP_SLOT(1) & (NDNBOOST_FT_cond)) == (NDNBOOST_FT_cond)

#   if ( NDNBOOST_PP_SLOT(1) & (NDNBOOST_FT_variadic) )
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_ell ...
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_nullary_param
#   else
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_ell
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_nullary_param NDNBOOST_FT_NULLARY_PARAM
#   endif

#   if !( NDNBOOST_PP_SLOT(1) & (NDNBOOST_FT_volatile) )
#     if !( NDNBOOST_PP_SLOT(1) & (NDNBOOST_FT_const) )
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_cv 
#     else
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_cv const
#     endif
#   else
#     if !( NDNBOOST_PP_SLOT(1) & (NDNBOOST_FT_const) )
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_cv volatile
#     else
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_cv const volatile
#     endif
#   endif
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_flags NDNBOOST_PP_SLOT(1)
NDNBOOST_PP_EXPAND(#)   include NDNBOOST_FT_variate_file

NDNBOOST_PP_EXPAND(#)   undef NDNBOOST_FT_cv
NDNBOOST_PP_EXPAND(#)   undef NDNBOOST_FT_ell
NDNBOOST_PP_EXPAND(#)   undef NDNBOOST_FT_nullary_param
NDNBOOST_PP_EXPAND(#)   undef NDNBOOST_FT_flags
NDNBOOST_PP_EXPAND(#) endif
NDNBOOST_PP_EXPAND(#) endif
#endif



// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

// no include guards, this file is intended for multiple inclusions

#ifndef NDNBOOST_FT_DETAIL_PP_LOOP_HPP_INCLUDED
#define NDNBOOST_FT_DETAIL_PP_LOOP_HPP_INCLUDED
#   include <ndnboost/preprocessor/facilities/expand.hpp>
#   include <ndnboost/preprocessor/facilities/empty.hpp>
#   include <ndnboost/preprocessor/punctuation/paren.hpp>
#endif

#include <ndnboost/function_types/detail/encoding/def.hpp>
#include <ndnboost/function_types/detail/encoding/aliases_def.hpp>

#if defined(NDNBOOST_FT_PREPROCESSING_MODE)
#   define NDNBOOST_FT_loop <ndnboost/function_types/detail/pp_cc_loop/master.hpp>
#else
#   define NDNBOOST_FT_loop \
        <ndnboost/function_types/detail/pp_cc_loop/preprocessed.hpp>
#endif

#if defined(NDNBOOST_FT_al_path)

#   define NDNBOOST_FT_cc_file \
        <ndnboost/function_types/detail/pp_variate_loop/preprocessed.hpp>
#   define NDNBOOST_FT_variate_file \
        <ndnboost/function_types/detail/pp_arity_loop.hpp>

#   ifndef NDNBOOST_FT_type_function
#   define NDNBOOST_FT_type_function(cc,name) NDNBOOST_FT_SYNTAX( \
      R NDNBOOST_PP_EMPTY,NDNBOOST_PP_EMPTY,cc,NDNBOOST_PP_EMPTY,name,NDNBOOST_PP_EMPTY)
#   endif
#   ifndef NDNBOOST_FT_type_function_pointer
#   define NDNBOOST_FT_type_function_pointer(cc,name) NDNBOOST_FT_SYNTAX( \
      R NDNBOOST_PP_EMPTY,NDNBOOST_PP_LPAREN,cc,* NDNBOOST_PP_EMPTY,name,NDNBOOST_PP_RPAREN)
#   endif
#   ifndef NDNBOOST_FT_type_function_reference
#   define NDNBOOST_FT_type_function_reference(cc,name) NDNBOOST_FT_SYNTAX( \
      R NDNBOOST_PP_EMPTY,NDNBOOST_PP_LPAREN,cc,& NDNBOOST_PP_EMPTY,name,NDNBOOST_PP_RPAREN)
#   endif
#   ifndef NDNBOOST_FT_type_member_function_pointer
#   define NDNBOOST_FT_type_member_function_pointer(cc,name) NDNBOOST_FT_SYNTAX( \
      R NDNBOOST_PP_EMPTY,NDNBOOST_PP_LPAREN,cc,T0::* NDNBOOST_PP_EMPTY,name,NDNBOOST_PP_RPAREN)
#   endif

#   include NDNBOOST_FT_loop

#   undef NDNBOOST_FT_type_function
#   undef NDNBOOST_FT_type_function_pointer
#   undef NDNBOOST_FT_type_function_reference
#   undef NDNBOOST_FT_type_member_function_pointer

#   undef NDNBOOST_FT_variations
#   undef NDNBOOST_FT_variate_file
#   undef NDNBOOST_FT_cc_file
#   undef NDNBOOST_FT_al_path

#elif defined(NDNBOOST_FT_cc_file)

#   include NDNBOOST_FT_loop
#   undef NDNBOOST_FT_cc_file

#else

#   error "argument missing"

#endif

#undef NDNBOOST_FT_loop

#include <ndnboost/function_types/detail/encoding/aliases_undef.hpp>
#include <ndnboost/function_types/detail/encoding/undef.hpp>



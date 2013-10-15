
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

// no include guards, this file is intended for multiple inclusion

#ifndef NDNBOOST_FT_PREPROCESSING_MODE
// input:  NDNBOOST_FT_mfp        0 or 1 <=> member function pointer?
// input:  NDNBOOST_FT_type_name  NDNBOOST_FT_type --> "R (* ..._type_name)()" (pass2)
#endif
// input:  NDNBOOST_FT_syntax     type macro to use
// input:  NDNBOOST_FT_cc         empty or cc specifier 
// input:  NDNBOOST_FT_ell        empty or "..."
// input:  NDNBOOST_FT_cv         empty or cv qualifiers
// input:  NDNBOOST_FT_flags      single decimal integer encoding the flags
// output: NDNBOOST_FT_n          number of component types (arity+1)
// output: NDNBOOST_FT_arity      current arity
// output: NDNBOOST_FT_type       macro that expands to the type
// output: NDNBOOST_FT_tplargs(p) template arguments with given prefix
// output: NDNBOOST_FT_params(p)  parameters with given prefix

#ifdef __WAVE__
#   pragma wave option(preserve: 0)
#endif

#ifndef NDNBOOST_FT_ARITY_LOOP_IS_ITERATING

#   define NDNBOOST_FT_AL_PREPROCESSED \
        NDNBOOST_FT_AL_FILE(NDNBOOST_FT_al_path,NDNBOOST_FT_FROM_ARITY,NDNBOOST_FT_mfp)

#   define NDNBOOST_FT_AL_FILE(base_path,max_arity,mfp) \
        NDNBOOST_FT_AL_FILE_I(base_path,max_arity,mfp)
#   define NDNBOOST_FT_AL_FILE_I(base_path,max_arity,mfp) \
        <base_path/arity ## max_arity ## _ ## mfp.hpp>

#   if !defined(NDNBOOST_FT_PREPROCESSING_MODE)

#     if NDNBOOST_FT_MAX_ARITY < 10
#       define NDNBOOST_FT_FROM_ARITY 0 
#     elif NDNBOOST_FT_MAX_ARITY < 20
#       define NDNBOOST_FT_FROM_ARITY 10
#     elif NDNBOOST_FT_MAX_ARITY < 30
#       define NDNBOOST_FT_FROM_ARITY 20
#     elif NDNBOOST_FT_MAX_ARITY < 40
#       define NDNBOOST_FT_FROM_ARITY 30
#     endif

#     if NDNBOOST_FT_FROM_ARITY
#       include NDNBOOST_FT_AL_PREPROCESSED
#     endif

#   elif !defined(NDNBOOST_FT_FROM_ARITY) // single pass preprocessing
#     define NDNBOOST_FT_FROM_ARITY 0

#   elif NDNBOOST_FT_FROM_ARITY > 0       // arity20 includes arity10
NDNBOOST_PP_EXPAND(#) include NDNBOOST_FT_AL_PREPROCESSED
#   endif

#   undef NDNBOOST_FT_AL_PREPROCESSED

#   undef NDNBOOST_FT_AL_FILE
#   undef NDNBOOST_FT_AL_FILE_I

#   if NDNBOOST_FT_MAX_ARITY > NDNBOOST_FT_FROM_ARITY

#     ifndef NDNBOOST_FT_DETAIL_ARITY_LOOP_HPP_INCLUDED
#     define NDNBOOST_FT_DETAIL_ARITY_LOOP_HPP_INCLUDED
#         include <ndnboost/preprocessor/cat.hpp>
#         include <ndnboost/preprocessor/tuple/eat.hpp>
#         include <ndnboost/preprocessor/control/if.hpp>
#         include <ndnboost/preprocessor/arithmetic/inc.hpp>
#         include <ndnboost/preprocessor/facilities/empty.hpp>
#         include <ndnboost/preprocessor/facilities/expand.hpp>
#         include <ndnboost/preprocessor/iteration/iterate.hpp>
#         include <ndnboost/preprocessor/repetition/enum_params.hpp>
#         include <ndnboost/preprocessor/repetition/enum_shifted_params.hpp>
#         include <ndnboost/preprocessor/repetition/enum_trailing_params.hpp>
#     endif

#     define NDNBOOST_FT_AL_INCLUDE_FILE <NDNBOOST_FT_al_path/master.hpp>

#     define NDNBOOST_FT_ARITY_LOOP_PREFIX 1
#     include NDNBOOST_FT_AL_INCLUDE_FILE
#     undef  NDNBOOST_FT_ARITY_LOOP_PREFIX

#     if !NDNBOOST_PP_IS_ITERATING
#       define NDNBOOST_PP_FILENAME_1 NDNBOOST_FT_AL_INCLUDE_FILE
#     elif NDNBOOST_PP_ITERATION_DEPTH() == 1
#       define NDNBOOST_PP_FILENAME_2 NDNBOOST_FT_AL_INCLUDE_FILE
#     else
#       error "loops nested too deeply"
#     endif

#     define NDNBOOST_FT_arity NDNBOOST_PP_ITERATION()
#     define NDNBOOST_FT_n     NDNBOOST_PP_INC(NDNBOOST_FT_arity)

#     define NDNBOOST_FT_type \
          NDNBOOST_FT_syntax(NDNBOOST_FT_cc,NDNBOOST_FT_type_name NDNBOOST_PP_EMPTY)\
               (NDNBOOST_FT_params(NDNBOOST_PP_EMPTY) NDNBOOST_FT_ell) NDNBOOST_FT_cv

#     define NDNBOOST_FT_tplargs(prefx) \
          prefx() R NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_FT_arity,prefx() T)

#     if !NDNBOOST_FT_mfp

#       define NDNBOOST_FT_params(prefx) \
            NDNBOOST_PP_IF(NDNBOOST_FT_arity,NDNBOOST_PP_ENUM_PARAMS, \
                NDNBOOST_FT_nullary_param NDNBOOST_PP_TUPLE_EAT(2))( \
                    NDNBOOST_FT_arity,prefx() T) 
#     else

#       define NDNBOOST_FT_params(prefx) \
            NDNBOOST_PP_ENUM_SHIFTED_PARAMS(NDNBOOST_FT_arity,prefx() T)

#     endif

#     if !NDNBOOST_FT_FROM_ARITY 
#       define NDNBOOST_PP_ITERATION_LIMITS (NDNBOOST_FT_mfp, NDNBOOST_FT_MAX_ARITY)
#     else
#       define NDNBOOST_PP_ITERATION_LIMITS \
            (NDNBOOST_FT_FROM_ARITY+1, NDNBOOST_FT_MAX_ARITY)
#     endif

#     define NDNBOOST_FT_ARITY_LOOP_IS_ITERATING 1
#     include NDNBOOST_PP_ITERATE()
#     undef  NDNBOOST_FT_ARITY_LOOP_IS_ITERATING

#     undef NDNBOOST_FT_arity
#     undef NDNBOOST_FT_params
#     undef NDNBOOST_FT_tplargs
#     undef NDNBOOST_FT_type

#     define NDNBOOST_FT_ARITY_LOOP_SUFFIX 1
#     include NDNBOOST_FT_AL_INCLUDE_FILE
#     undef  NDNBOOST_FT_ARITY_LOOP_SUFFIX

#     undef NDNBOOST_FT_AL_INCLUDE_FILE
#   endif

#   undef NDNBOOST_FT_FROM_ARITY

#else
#   error "attempt to nest arity loops"
#endif


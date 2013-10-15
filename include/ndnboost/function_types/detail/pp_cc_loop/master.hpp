
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

// no include guards, this file is intended for multiple inclusions

#ifdef __WAVE__
// this file has been generated from the master.hpp file in the same directory
#   pragma wave option(preserve: 0)
#endif


#if !NDNBOOST_PP_IS_ITERATING

#   ifndef NDNBOOST_FT_DETAIL_CC_LOOP_MASTER_HPP_INCLUDED
#   define NDNBOOST_FT_DETAIL_CC_LOOP_MASTER_HPP_INCLUDED
#     include <ndnboost/function_types/config/cc_names.hpp>

#     include <ndnboost/preprocessor/cat.hpp>
#     include <ndnboost/preprocessor/seq/size.hpp>
#     include <ndnboost/preprocessor/seq/elem.hpp>
#     include <ndnboost/preprocessor/tuple/elem.hpp>
#     include <ndnboost/preprocessor/iteration/iterate.hpp>
#     include <ndnboost/preprocessor/facilities/expand.hpp>
#     include <ndnboost/preprocessor/arithmetic/inc.hpp>
#   endif

#   include <ndnboost/function_types/detail/encoding/def.hpp>
#   include <ndnboost/function_types/detail/encoding/aliases_def.hpp>

#   define  NDNBOOST_PP_FILENAME_1 \
        <ndnboost/function_types/detail/pp_cc_loop/master.hpp>
#   define  NDNBOOST_PP_ITERATION_LIMITS \
        (0,NDNBOOST_PP_SEQ_SIZE(NDNBOOST_FT_CC_NAMES_SEQ)-1)
#   include NDNBOOST_PP_ITERATE()
#   if !defined(NDNBOOST_FT_config_valid) && NDNBOOST_FT_CC_PREPROCESSING
#     define NDNBOOST_FT_cc_id 1
#     define NDNBOOST_FT_cc_name implicit_cc
#     define NDNBOOST_FT_cc NDNBOOST_PP_EMPTY
#     define NDNBOOST_FT_cond callable_builtin
#     include NDNBOOST_FT_cc_file
#     undef NDNBOOST_FT_cond
#     undef NDNBOOST_FT_cc_name
#     undef NDNBOOST_FT_cc
#     undef NDNBOOST_FT_cc_id
#   elif !defined(NDNBOOST_FT_config_valid) // and generating preprocessed file
NDNBOOST_PP_EXPAND(#) ifndef NDNBOOST_FT_config_valid
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_cc_id 1
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_cc_name implicit_cc
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_cc NDNBOOST_PP_EMPTY
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_cond callable_builtin
#define _()
NDNBOOST_PP_EXPAND(#)   include NDNBOOST_FT_cc_file
#undef _
NDNBOOST_PP_EXPAND(#)   undef NDNBOOST_FT_cond
NDNBOOST_PP_EXPAND(#)   undef NDNBOOST_FT_cc_name
NDNBOOST_PP_EXPAND(#)   undef NDNBOOST_FT_cc
NDNBOOST_PP_EXPAND(#)   undef NDNBOOST_FT_cc_id
NDNBOOST_PP_EXPAND(#) else
NDNBOOST_PP_EXPAND(#)   undef NDNBOOST_FT_config_valid
NDNBOOST_PP_EXPAND(#) endif

#   else
#     undef NDNBOOST_FT_config_valid
#   endif

#   include <ndnboost/function_types/detail/encoding/aliases_undef.hpp>
#   include <ndnboost/function_types/detail/encoding/undef.hpp>

#elif NDNBOOST_FT_CC_PREPROCESSING

#   define NDNBOOST_FT_cc_id  NDNBOOST_PP_INC(NDNBOOST_PP_FRAME_ITERATION(1))
#   define NDNBOOST_FT_cc_inf \
        NDNBOOST_PP_SEQ_ELEM(NDNBOOST_PP_FRAME_ITERATION(1),NDNBOOST_FT_CC_NAMES_SEQ)

#   define NDNBOOST_FT_cc_pp_name NDNBOOST_PP_TUPLE_ELEM(3,0,NDNBOOST_FT_cc_inf)
#   define NDNBOOST_FT_cc_name    NDNBOOST_PP_TUPLE_ELEM(3,1,NDNBOOST_FT_cc_inf)
#   define NDNBOOST_FT_cc         NDNBOOST_PP_TUPLE_ELEM(3,2,NDNBOOST_FT_cc_inf)

#   define NDNBOOST_FT_cond NDNBOOST_PP_CAT(NDNBOOST_FT_CC_,NDNBOOST_FT_cc_pp_name)

#   if NDNBOOST_FT_cond
#     define NDNBOOST_FT_config_valid 1
#     include NDNBOOST_FT_cc_file
#   endif

#   undef NDNBOOST_FT_cond

#   undef NDNBOOST_FT_cc_pp_name
#   undef NDNBOOST_FT_cc_name
#   undef NDNBOOST_FT_cc

#   undef NDNBOOST_FT_cc_id
#   undef NDNBOOST_FT_cc_inf

#else // if generating preprocessed file
NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_cc_id NDNBOOST_PP_INC(NDNBOOST_PP_ITERATION())

#   define NDNBOOST_FT_cc_inf \
        NDNBOOST_PP_SEQ_ELEM(NDNBOOST_PP_ITERATION(),NDNBOOST_FT_CC_NAMES_SEQ)

#   define NDNBOOST_FT_cc_pp_name NDNBOOST_PP_TUPLE_ELEM(3,0,NDNBOOST_FT_cc_inf)

#   define NDNBOOST_FT_CC_DEF(name,index) \
        name NDNBOOST_PP_TUPLE_ELEM(3,index,NDNBOOST_FT_cc_inf)
NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_CC_DEF(NDNBOOST_FT_cc_name,1)
NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_CC_DEF(NDNBOOST_FT_cc,2)
#   undef  NDNBOOST_FT_CC_DEF

#   define NDNBOOST_FT_cc_cond_v NDNBOOST_PP_CAT(NDNBOOST_FT_CC_,NDNBOOST_FT_cc_pp_name)
NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_cond NDNBOOST_FT_cc_cond_v
#   undef  NDNBOOST_FT_cc_cond_v

#   undef NDNBOOST_FT_cc_pp_name
#   undef NDNBOOST_FT_cc_inf

NDNBOOST_PP_EXPAND(#) if NDNBOOST_FT_cond
NDNBOOST_PP_EXPAND(#)   define NDNBOOST_FT_config_valid 1
#define _()
NDNBOOST_PP_EXPAND(#)   include NDNBOOST_FT_cc_file
#undef _
NDNBOOST_PP_EXPAND(#) endif

NDNBOOST_PP_EXPAND(#) undef NDNBOOST_FT_cond

NDNBOOST_PP_EXPAND(#) undef NDNBOOST_FT_cc_name
NDNBOOST_PP_EXPAND(#) undef NDNBOOST_FT_cc

NDNBOOST_PP_EXPAND(#) undef NDNBOOST_FT_cc_id

#endif


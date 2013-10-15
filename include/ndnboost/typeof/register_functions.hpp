// Copyright (C) 2004 Arkadiy Vertleyb
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_TYPEOF_REGISTER_FUNCTIONS_HPP_INCLUDED
#define NDNBOOST_TYPEOF_REGISTER_FUNCTIONS_HPP_INCLUDED

#include <ndnboost/preprocessor/repetition/enum.hpp>
#include <ndnboost/preprocessor/repetition/enum_params.hpp>
#include <ndnboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <ndnboost/preprocessor/cat.hpp>
#include <ndnboost/preprocessor/inc.hpp>
#include <ndnboost/preprocessor/dec.hpp>
#include <ndnboost/preprocessor/if.hpp>
#include <ndnboost/preprocessor/arithmetic/add.hpp>
#include <ndnboost/preprocessor/iteration/iterate.hpp>

#include NDNBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

#ifndef NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY
#define NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY 10
#endif

enum 
{
    FUN_ID                          = NDNBOOST_TYPEOF_UNIQUE_ID(),
    FUN_PTR_ID                      = FUN_ID +  1 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_REF_ID                      = FUN_ID +  2 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    MEM_FUN_ID                      = FUN_ID +  3 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    CONST_MEM_FUN_ID                = FUN_ID +  4 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_MEM_FUN_ID             = FUN_ID +  5 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_CONST_MEM_FUN_ID       = FUN_ID +  6 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_VAR_ID                      = FUN_ID +  7 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_VAR_PTR_ID                  = FUN_ID +  8 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_VAR_REF_ID                  = FUN_ID +  9 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    MEM_FUN_VAR_ID                  = FUN_ID + 10 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    CONST_MEM_FUN_VAR_ID            = FUN_ID + 11 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_MEM_FUN_VAR_ID         = FUN_ID + 12 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_CONST_MEM_FUN_VAR_ID   = FUN_ID + 13 * NDNBOOST_PP_INC(NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY)
};

NDNBOOST_TYPEOF_BEGIN_ENCODE_NS

# define NDNBOOST_PP_ITERATION_LIMITS (0, NDNBOOST_TYPEOF_LIMIT_FUNCTION_ARITY)
# define NDNBOOST_PP_FILENAME_1 <ndnboost/typeof/register_functions_iterate.hpp>
# include NDNBOOST_PP_ITERATE()

NDNBOOST_TYPEOF_END_ENCODE_NS

#endif//NDNBOOST_TYPEOF_REGISTER_FUNCTIONS_HPP_INCLUDED

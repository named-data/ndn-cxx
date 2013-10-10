// Copyright (C) 2004 Arkadiy Vertleyb
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_TYPEOF_REGISTER_FUNCTIONS_HPP_INCLUDED
#define BOOST_TYPEOF_REGISTER_FUNCTIONS_HPP_INCLUDED

#include <ndnboost/preprocessor/repetition/enum.hpp>
#include <ndnboost/preprocessor/repetition/enum_params.hpp>
#include <ndnboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <ndnboost/preprocessor/cat.hpp>
#include <ndnboost/preprocessor/inc.hpp>
#include <ndnboost/preprocessor/dec.hpp>
#include <ndnboost/preprocessor/if.hpp>
#include <ndnboost/preprocessor/arithmetic/add.hpp>
#include <ndnboost/preprocessor/iteration/iterate.hpp>

#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

#ifndef BOOST_TYPEOF_LIMIT_FUNCTION_ARITY
#define BOOST_TYPEOF_LIMIT_FUNCTION_ARITY 10
#endif

enum 
{
    FUN_ID                          = BOOST_TYPEOF_UNIQUE_ID(),
    FUN_PTR_ID                      = FUN_ID +  1 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_REF_ID                      = FUN_ID +  2 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    MEM_FUN_ID                      = FUN_ID +  3 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    CONST_MEM_FUN_ID                = FUN_ID +  4 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_MEM_FUN_ID             = FUN_ID +  5 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_CONST_MEM_FUN_ID       = FUN_ID +  6 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_VAR_ID                      = FUN_ID +  7 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_VAR_PTR_ID                  = FUN_ID +  8 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_VAR_REF_ID                  = FUN_ID +  9 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    MEM_FUN_VAR_ID                  = FUN_ID + 10 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    CONST_MEM_FUN_VAR_ID            = FUN_ID + 11 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_MEM_FUN_VAR_ID         = FUN_ID + 12 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_CONST_MEM_FUN_VAR_ID   = FUN_ID + 13 * BOOST_PP_INC(BOOST_TYPEOF_LIMIT_FUNCTION_ARITY)
};

BOOST_TYPEOF_BEGIN_ENCODE_NS

# define BOOST_PP_ITERATION_LIMITS (0, BOOST_TYPEOF_LIMIT_FUNCTION_ARITY)
# define BOOST_PP_FILENAME_1 <ndnboost/typeof/register_functions_iterate.hpp>
# include BOOST_PP_ITERATE()

BOOST_TYPEOF_END_ENCODE_NS

#endif//BOOST_TYPEOF_REGISTER_FUNCTIONS_HPP_INCLUDED

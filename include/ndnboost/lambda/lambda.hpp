// -- lambda.hpp -- Boost Lambda Library -----------------------------------
// Copyright (C) 1999, 2000 Jaakko Jarvi (jaakko.jarvi@cs.utu.fi)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://lambda.cs.utu.fi 

#ifndef NDNBOOST_LAMBDA_LAMBDA_HPP
#define NDNBOOST_LAMBDA_LAMBDA_HPP


#include "ndnboost/lambda/core.hpp"

#ifdef NDNBOOST_NO_FDECL_TEMPLATES_AS_TEMPLATE_TEMPLATE_PARAMS
#include <istream>
#include <ostream>
#endif

#include "ndnboost/lambda/detail/operator_actions.hpp"
#include "ndnboost/lambda/detail/operator_lambda_func_base.hpp"
#include "ndnboost/lambda/detail/operator_return_type_traits.hpp"


#include "ndnboost/lambda/detail/operators.hpp"

#ifndef NDNBOOST_LAMBDA_FAILS_IN_TEMPLATE_KEYWORD_AFTER_SCOPE_OPER
// sorry, member ptr does not work with gcc2.95
#include "ndnboost/lambda/detail/member_ptr.hpp"
#endif


#endif

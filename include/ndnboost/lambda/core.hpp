// -- core.hpp -- Boost Lambda Library -------------------------------------
//
// Copyright (C) 2000 Gary Powell (powellg@amazon.com)
// Copyright (C) 1999, 2000 Jaakko Jarvi (jaakko.jarvi@cs.utu.fi)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see www.boost.org
// 
// Includes the core of LL, without any real features for client:
// 
// tuples, lambda functors, return type deduction templates,
// argument substitution mechanism (select functions)
// 
// Some functionality comes as well:
// Assignment and subscript operators, as well as function
// call operator for placeholder variables.
// -------------------------------------------------------------------------

#ifndef NDNBOOST_LAMBDA_CORE_HPP
#define NDNBOOST_LAMBDA_CORE_HPP

#include "ndnboost/type_traits/transform_traits.hpp"
#include "ndnboost/type_traits/cv_traits.hpp"

#include "ndnboost/tuple/tuple.hpp"

// inject some of the tuple names into lambda 
namespace ndnboost {
namespace lambda {

using ::ndnboost::tuples::tuple;
using ::ndnboost::tuples::null_type;

} // lambda
} // boost

#include "ndnboost/lambda/detail/lambda_config.hpp"
#include "ndnboost/lambda/detail/lambda_fwd.hpp"

#include "ndnboost/lambda/detail/arity_code.hpp"
#include "ndnboost/lambda/detail/actions.hpp"

#include "ndnboost/lambda/detail/lambda_traits.hpp"

#include "ndnboost/lambda/detail/function_adaptors.hpp"
#include "ndnboost/lambda/detail/return_type_traits.hpp"

#include "ndnboost/lambda/detail/select_functions.hpp"

#include "ndnboost/lambda/detail/lambda_functor_base.hpp"

#include "ndnboost/lambda/detail/lambda_functors.hpp"

#include "ndnboost/lambda/detail/ret.hpp"

namespace ndnboost {
namespace lambda {

namespace {

  // These are constants types and need to be initialised
  ndnboost::lambda::placeholder1_type free1 = ndnboost::lambda::placeholder1_type();
  ndnboost::lambda::placeholder2_type free2 = ndnboost::lambda::placeholder2_type();
  ndnboost::lambda::placeholder3_type free3 = ndnboost::lambda::placeholder3_type();

  ndnboost::lambda::placeholder1_type& _1 = free1;
  ndnboost::lambda::placeholder2_type& _2 = free2;
  ndnboost::lambda::placeholder3_type& _3 = free3;
  // _1, _2, ... naming scheme by Peter Dimov
} // unnamed
   
} // lambda
} // boost
   
   
#endif //NDNBOOST_LAMBDA_CORE_HPP

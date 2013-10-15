// Boost.Function library - Typeof support
//  Copyright (C) Douglas Gregor 2008
//
//  Use, modification and distribution is subject to the Boost
//  Software License, Version 1.0.  (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org
#ifndef NDNBOOST_FUNCTION_TYPEOF_HPP
#define NDNBOOST_FUNCTION_TYPEOF_HPP
#include <ndnboost/function/function_fwd.hpp>
#include <ndnboost/typeof/typeof.hpp>

#include NDNBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

NDNBOOST_TYPEOF_REGISTER_TYPE(ndnboost::bad_function_call)

#if !defined(NDNBOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX)
NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function, (typename))
#endif

NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function0, (typename))
NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function1, (typename)(typename))
NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function2, (typename)(typename)(typename))
NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function3, 
  (typename)(typename)(typename)(typename))
NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function4, 
  (typename)(typename)(typename)(typename)(typename))
NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function5, 
  (typename)(typename)(typename)(typename)(typename)(typename))
NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function6, 
  (typename)(typename)(typename)(typename)(typename)(typename)(typename))
NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function7, 
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename))
NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function8, 
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename)(typename))
NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function9, 
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename)(typename)(typename))
NDNBOOST_TYPEOF_REGISTER_TEMPLATE(ndnboost::function10, 
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename)(typename)(typename)(typename))
#endif

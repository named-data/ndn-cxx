
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).
//------------------------------------------------------------------------------

#ifndef NDNBOOST_FT_IS_FUNCTION_HPP_INCLUDED
#define NDNBOOST_FT_IS_FUNCTION_HPP_INCLUDED

#include <ndnboost/mpl/aux_/lambda_support.hpp>
#include <ndnboost/type_traits/detail/template_arity_spec.hpp>

#include <ndnboost/function_types/components.hpp>

namespace ndnboost 
{ 
  namespace function_types 
  {
    template< typename T, typename Tag = null_tag > 
    struct is_function
      : function_types::represents
        < function_types::components<T>
        , function_types::tag<Tag ,detail::function_tag> 
        >
    { 
      NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(2,is_function,(T,Tag))
    };
  }
  NDNBOOST_TT_AUX_TEMPLATE_ARITY_SPEC(2,function_types::is_function)
} 

#endif



// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef NDNBOOST_FT_IS_CALLABLE_BUILTIN_HPP_INCLUDED
#define NDNBOOST_FT_IS_CALLABLE_BUILTIN_HPP_INCLUDED

#include <ndnboost/mpl/aux_/lambda_support.hpp>
#include <ndnboost/type_traits/detail/template_arity_spec.hpp>

#include <ndnboost/function_types/components.hpp>

namespace ndnboost 
{ 
  namespace function_types 
  {
    template< typename T, typename Tag = null_tag > 
    struct is_callable_builtin
      : function_types::represents
        < function_types::components<T>
        , function_types::tag<Tag, detail::callable_builtin_tag> 
        >
    { 
      NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(2,is_callable_builtin,(T,Tag))
    };
  }
  NDNBOOST_TT_AUX_TEMPLATE_ARITY_SPEC(2,function_types::is_callable_builtin)
}

#endif


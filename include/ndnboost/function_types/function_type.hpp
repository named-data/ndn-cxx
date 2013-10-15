
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef NDNBOOST_FT_FUNCTION_TYPE_HPP_INCLUDED
#define NDNBOOST_FT_FUNCTION_TYPE_HPP_INCLUDED

#include <ndnboost/function_types/detail/synthesize.hpp>
#include <ndnboost/function_types/detail/to_sequence.hpp>

namespace ndnboost 
{ 
  namespace function_types 
  {
    template<typename Types, typename Tag = null_tag> struct function_type
      : detail::synthesize_func<typename detail::to_sequence<Types>::type, Tag>
    {
      NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(2,function_type,(Types,Tag))
    };
  }
  NDNBOOST_TT_AUX_TEMPLATE_ARITY_SPEC(2,function_types::function_type)
}

#endif
 

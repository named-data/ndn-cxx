
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef BOOST_FT_RESULT_TYPE_HPP_INCLUDED
#define BOOST_FT_RESULT_TYPE_HPP_INCLUDED

#include <ndnboost/blank.hpp>
#include <ndnboost/mpl/if.hpp>

#include <ndnboost/mpl/aux_/lambda_support.hpp>
#include <ndnboost/type_traits/detail/template_arity_spec.hpp>

#include <ndnboost/mpl/at.hpp>

#include <ndnboost/function_types/is_callable_builtin.hpp>
#include <ndnboost/function_types/components.hpp>

namespace ndnboost 
{ 
  namespace function_types 
  {
    template< typename T > struct result_type;

    namespace detail
    {
      template<typename T> struct result_type_impl
        : mpl::at_c
          < typename function_types::components<T>::types, 0 >
      { };
    }

    template<typename T> struct result_type
      : mpl::if_
        < function_types::is_callable_builtin<T>
        , detail::result_type_impl<T>, ndnboost::blank
        >::type
    { 
      BOOST_MPL_AUX_LAMBDA_SUPPORT(1,result_type,(T)) 
    };
  }
  BOOST_TT_AUX_TEMPLATE_ARITY_SPEC(1,function_types::result_type)
}

#endif


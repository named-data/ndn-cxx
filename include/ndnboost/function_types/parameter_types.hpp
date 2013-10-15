
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef NDNBOOST_FT_PARAMETER_TYPES_HPP_INCLUDED
#define NDNBOOST_FT_PARAMETER_TYPES_HPP_INCLUDED

#include <ndnboost/blank.hpp>
#include <ndnboost/mpl/if.hpp>

#include <ndnboost/mpl/aux_/lambda_support.hpp>
#include <ndnboost/type_traits/detail/template_arity_spec.hpp>

#include <ndnboost/mpl/pop_front.hpp>

#include <ndnboost/function_types/is_callable_builtin.hpp>
#include <ndnboost/function_types/components.hpp>

namespace ndnboost 
{ 
  namespace function_types 
  {
    using mpl::placeholders::_;
 
    template< typename T, typename ClassTypeTransform = add_reference<_> >
    struct parameter_types;

    namespace detail
    {
      template<typename T, typename ClassTypeTransform> 
      struct parameter_types_impl
        : mpl::pop_front
          < typename function_types::components<T,ClassTypeTransform>::types 
          >::type
      { };
    }

    template<typename T, typename ClassTypeTransform> struct parameter_types
      : mpl::if_
        < function_types::is_callable_builtin<T>
        , detail::parameter_types_impl<T,ClassTypeTransform>, ndnboost::blank
        >::type
    {
      NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(2,parameter_types,(T,ClassTypeTransform)) 
    };
  }
  NDNBOOST_TT_AUX_TEMPLATE_ARITY_SPEC(2,function_types::parameter_types)
}

#endif


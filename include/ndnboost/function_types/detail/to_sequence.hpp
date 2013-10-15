
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef NDNBOOST_FT_DETAIL_TO_SEQUENCE_HPP_INCLUDED
#define NDNBOOST_FT_DETAIL_TO_SEQUENCE_HPP_INCLUDED

#include <ndnboost/mpl/eval_if.hpp>
#include <ndnboost/mpl/identity.hpp>
#include <ndnboost/mpl/is_sequence.hpp>
#include <ndnboost/mpl/placeholders.hpp>
#include <ndnboost/type_traits/add_reference.hpp>

#include <ndnboost/function_types/is_callable_builtin.hpp>

namespace ndnboost { namespace function_types { namespace detail {

// wrap first arguments in components, if callable builtin type
template<typename T>
struct to_sequence
{
  typedef typename
   mpl::eval_if
   < is_callable_builtin<T>
   , to_sequence< components<T> >
   , mpl::identity< T >
   >::type
  type;
};

#ifndef NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
// reduce template instantiations, if possible
template<typename T, typename U>
struct to_sequence< components<T,U> > 
{
  typedef typename components<T,U>::types type;
};
#endif

} } } // namespace ::ndnboost::function_types::detail

#endif


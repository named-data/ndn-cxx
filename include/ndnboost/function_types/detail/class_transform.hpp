
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef NDNBOOST_FT_DETAIL_CLASS_TRANSFORM_HPP_INCLUDED
#define NDNBOOST_FT_DETAIL_CLASS_TRANSFORM_HPP_INCLUDED

#include <ndnboost/mpl/apply.hpp>
#include <ndnboost/mpl/always.hpp>
#include <ndnboost/mpl/identity.hpp>
#include <ndnboost/mpl/placeholders.hpp>

#include <ndnboost/type_traits/remove_cv.hpp>
#include <ndnboost/type_traits/add_pointer.hpp>
#include <ndnboost/type_traits/add_reference.hpp>

namespace ndnboost { namespace function_types { namespace detail {

using mpl::placeholders::_;

// Transformation metafunction for the class type of member function pointers.
template<typename T, typename L>
struct class_transform
{ typedef typename mpl::apply1<L,T>::type type; };


#ifndef NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
// We can short-circuit the mechanism implemented in the primary template for 
// the most common lambda expression and save both the "un-lambdaing" and the
// type traits invocation (we know that T can only be a class type).

template<typename T> struct class_transform< T, mpl::identity<_> >
{ typedef T type; };

template<typename T> struct class_transform< T, add_reference<_> > 
{ typedef T & type; };

template<typename T> struct class_transform< T, add_pointer<_> >
{ typedef T * type; };

template<typename T> struct class_transform< T, remove_cv<_> >
{ typedef typename ndnboost::remove_cv<T>::type type; };

template<typename T> struct class_transform< T, add_reference< remove_cv<_> > >
{ typedef typename ndnboost::remove_cv<T>::type & type; };

template<typename T> struct class_transform< T, add_pointer< remove_cv<_> > >
{ typedef typename ndnboost::remove_cv<T>::type * type; };

template<typename T, typename U> struct class_transform< T, mpl::always<U> >
{ typedef U type; };
#endif


} } } // namespace ::ndnboost::function_types::detail

#endif


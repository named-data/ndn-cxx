// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_WRAP_UNWRAP_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_WRAP_UNWRAP_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

#include <ndnboost/config.hpp>                             // SFINAE, MSVC.
#include <ndnboost/detail/workaround.hpp>
#include <ndnboost/iostreams/detail/enable_if_stream.hpp>
#include <ndnboost/iostreams/traits_fwd.hpp>               // is_std_io.
#include <ndnboost/mpl/bool.hpp>
#include <ndnboost/mpl/identity.hpp>
#include <ndnboost/mpl/eval_if.hpp>
#include <ndnboost/mpl/if.hpp>
#include <ndnboost/ref.hpp>

namespace ndnboost { namespace iostreams { namespace detail {
                    
//------------------Definition of wrap/unwrap traits--------------------------//

template<typename T>
struct wrapped_type 
    : mpl::if_<is_std_io<T>, reference_wrapper<T>, T>
    { };

template<typename T>
struct unwrapped_type 
    : unwrap_reference<T>
    { };

template<typename T>
struct unwrap_ios 
    : mpl::eval_if<
          is_std_io<T>, 
          unwrap_reference<T>, 
          mpl::identity<T>
      >
    { };

//------------------Definition of wrap----------------------------------------//

#ifndef NDNBOOST_NO_SFINAE //----------------------------------------------------//
    template<typename T>
    inline T wrap(const T& t NDNBOOST_IOSTREAMS_DISABLE_IF_STREAM(T)) 
    { return t; }

    template<typename T>
    inline typename wrapped_type<T>::type
    wrap(T& t NDNBOOST_IOSTREAMS_ENABLE_IF_STREAM(T)) { return ndnboost::ref(t); }
#else // #ifndef NDNBOOST_NO_SFINAE //-------------------------------------------//
    template<typename T>
    inline typename wrapped_type<T>::type // BCC 5.x needs namespace qualification.
    wrap_impl(const T& t, mpl::true_) { return ndnboost::ref(const_cast<T&>(t)); }

    template<typename T>
    inline typename wrapped_type<T>::type // BCC 5.x needs namespace qualification.
    wrap_impl(T& t, mpl::true_) { return ndnboost::ref(t); }

    template<typename T>
    inline typename wrapped_type<T>::type 
    wrap_impl(const T& t, mpl::false_) { return t; }

    template<typename T>
    inline typename wrapped_type<T>::type 
    wrap_impl(T& t, mpl::false_) { return t; }

    template<typename T>
    inline typename wrapped_type<T>::type 
    wrap(const T& t) { return wrap_impl(t, is_std_io<T>()); }

    template<typename T>
    inline typename wrapped_type<T>::type 
    wrap(T& t) { return wrap_impl(t, is_std_io<T>()); }
#endif // #ifndef NDNBOOST_NO_SFINAE //------------------------------------------//

//------------------Definition of unwrap--------------------------------------//

#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1310) //----------------------------------//

template<typename T>
typename unwrapped_type<T>::type& 
unwrap(const reference_wrapper<T>& ref) { return ref.get(); }

template<typename T>
typename unwrapped_type<T>::type& unwrap(T& t) { return t; }

template<typename T>
const typename unwrapped_type<T>::type& unwrap(const T& t) { return t; }

#else // #if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1310) //-------------------------//

// Since unwrap is a potential bottleneck, we avoid runtime tag dispatch.
template<bool IsRefWrap>
struct unwrap_impl;

template<>
struct unwrap_impl<true> {
    template<typename T>
    static typename unwrapped_type<T>::type& unwrap(const T& t) 
    { return t.get(); }
};

template<>
struct unwrap_impl<false> {
    template<typename T>
    static typename unwrapped_type<T>::type& unwrap(const T& t) 
    { return const_cast<T&>(t); }
};

template<typename T>
typename unwrapped_type<T>::type& 
unwrap(const T& t) 
{ return unwrap_impl<is_reference_wrapper<T>::value>::unwrap(t); }

#endif // #if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1310) //------------------------//

} } } // End namespaces detail, iostreams, boost.

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_WRAP_UNWRAP_HPP_INCLUDED

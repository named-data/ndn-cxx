// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_FORWARD_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_FORWARD_HPP_INCLUDED   

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif                  
 
#include <ndnboost/config.hpp> // NDNBOOST_MSVC, NDNBOOST_NO_SFINAE
#include <ndnboost/detail/workaround.hpp>
#include <ndnboost/iostreams/detail/config/limits.hpp>
#include <ndnboost/iostreams/detail/push_params.hpp>
#include <ndnboost/preprocessor/arithmetic/dec.hpp>
#include <ndnboost/preprocessor/arithmetic/inc.hpp>
#include <ndnboost/preprocessor/punctuation/comma_if.hpp>
#include <ndnboost/preprocessor/repetition/enum_binary_params.hpp>
#include <ndnboost/preprocessor/repetition/enum_params.hpp>
#include <ndnboost/preprocessor/repetition/repeat_from_to.hpp>
#include <ndnboost/preprocessor/tuple/elem.hpp>
#include <ndnboost/type_traits/is_same.hpp>

//------Macros for defining forwarding constructors and open overloads--------//
    
//
// Macro: NDNBOOST_IOSTREAMS_FORWARD(class, impl, device, params, args)
// Description: Defines constructors and overloads of 'open' which construct
//      a device using the specified argument list and pass it to the specified
//      helper function
//   class - The class name
//   impl - The helper function
//   device - The device type
//   params - The list of formal parameters trailing the device parameter in
//     the helper function's signature
//   params - The list of arguments passed to the helper function, following the
//     device argument
//
#define NDNBOOST_IOSTREAMS_FORWARD(class, impl, device, params, args) \
    class(const device& t params()) \
    { this->impl(::ndnboost::iostreams::detail::wrap(t) args()); } \
    class(device& t params()) \
    { this->impl(::ndnboost::iostreams::detail::wrap(t) args()); } \
    class(const ::ndnboost::reference_wrapper<device>& ref params()) \
    { this->impl(ref args()); } \
    void open(const device& t params()) \
    { this->impl(::ndnboost::iostreams::detail::wrap(t) args()); } \
    void open(device& t params()) \
    { this->impl(::ndnboost::iostreams::detail::wrap(t) args()); } \
    void open(const ::ndnboost::reference_wrapper<device>& ref params()) \
    { this->impl(ref args()); } \
    NDNBOOST_PP_REPEAT_FROM_TO( \
        1, NDNBOOST_PP_INC(NDNBOOST_IOSTREAMS_MAX_FORWARDING_ARITY), \
        NDNBOOST_IOSTREAMS_FORWARDING_CTOR, (class, impl, device) \
    ) \
    NDNBOOST_PP_REPEAT_FROM_TO( \
        1, NDNBOOST_PP_INC(NDNBOOST_IOSTREAMS_MAX_FORWARDING_ARITY), \
        NDNBOOST_IOSTREAMS_FORWARDING_FN, (class, impl, device) \
    ) \
    /**/
#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
# define NDNBOOST_IOSTREAMS_FORWARDING_CTOR_I(z, n, tuple) \
    template< typename U100 NDNBOOST_PP_COMMA_IF(NDNBOOST_PP_DEC(n)) \
              NDNBOOST_PP_ENUM_PARAMS_Z(z, NDNBOOST_PP_DEC(n), typename U) > \
    NDNBOOST_PP_TUPLE_ELEM(3, 0, tuple) \
    ( U100& u100 NDNBOOST_PP_COMMA_IF(NDNBOOST_PP_DEC(n)) \
      NDNBOOST_PP_ENUM_BINARY_PARAMS_Z(z, NDNBOOST_PP_DEC(n), const U, &u) \
      NDNBOOST_IOSTREAMS_DISABLE_IF_SAME(U100, NDNBOOST_PP_TUPLE_ELEM(3, 2, tuple))) \
    { this->NDNBOOST_PP_TUPLE_ELEM(3, 1, tuple) \
      ( NDNBOOST_PP_TUPLE_ELEM(3, 2, tuple) \
        ( u100 NDNBOOST_PP_COMMA_IF(NDNBOOST_PP_DEC(n)) \
          NDNBOOST_PP_ENUM_PARAMS_Z(z, NDNBOOST_PP_DEC(n), u)) ); } \
    /**/
# define NDNBOOST_IOSTREAMS_FORWARDING_FN_I(z, n, tuple) \
    template< typename U100 NDNBOOST_PP_COMMA_IF(NDNBOOST_PP_DEC(n)) \
              NDNBOOST_PP_ENUM_PARAMS_Z(z, NDNBOOST_PP_DEC(n), typename U) > \
    void open \
    ( U100& u100 NDNBOOST_PP_COMMA_IF(NDNBOOST_PP_DEC(n)) \
      NDNBOOST_PP_ENUM_BINARY_PARAMS_Z(z, NDNBOOST_PP_DEC(n), const U, &u) \
      NDNBOOST_IOSTREAMS_DISABLE_IF_SAME(U100, NDNBOOST_PP_TUPLE_ELEM(3, 2, tuple))) \
    { this->NDNBOOST_PP_TUPLE_ELEM(3, 1, tuple) \
      ( u100 NDNBOOST_PP_COMMA_IF(NDNBOOST_PP_DEC(n)) \
        NDNBOOST_PP_ENUM_PARAMS_Z(z, NDNBOOST_PP_DEC(n), u) ); } \
    /**/
#else
# define NDNBOOST_IOSTREAMS_FORWARDING_CTOR_I(z, n, tuple)
# define NDNBOOST_IOSTREAMS_FORWARDING_FN_I(z, n, tuple)
#endif
#define NDNBOOST_IOSTREAMS_FORWARDING_CTOR(z, n, tuple) \
    template<NDNBOOST_PP_ENUM_PARAMS_Z(z, n, typename U)> \
    NDNBOOST_PP_TUPLE_ELEM(3, 0, tuple) \
    (NDNBOOST_PP_ENUM_BINARY_PARAMS_Z(z, n, const U, &u) \
      NDNBOOST_IOSTREAMS_DISABLE_IF_SAME(U0, NDNBOOST_PP_TUPLE_ELEM(3, 2, tuple))) \
    { this->NDNBOOST_PP_TUPLE_ELEM(3, 1, tuple) \
      ( NDNBOOST_PP_TUPLE_ELEM(3, 2, tuple) \
        (NDNBOOST_PP_ENUM_PARAMS_Z(z, n, u)) ); } \
    NDNBOOST_IOSTREAMS_FORWARDING_CTOR_I(z, n, tuple) \
    /**/
#define NDNBOOST_IOSTREAMS_FORWARDING_FN(z, n, tuple) \
    template<NDNBOOST_PP_ENUM_PARAMS_Z(z, n, typename U)> \
    void open(NDNBOOST_PP_ENUM_BINARY_PARAMS_Z(z, n, const U, &u) \
      NDNBOOST_IOSTREAMS_DISABLE_IF_SAME(U0, NDNBOOST_PP_TUPLE_ELEM(3, 2, tuple))) \
    { this->NDNBOOST_PP_TUPLE_ELEM(3, 1, tuple) \
      ( NDNBOOST_PP_TUPLE_ELEM(3, 2, tuple) \
        (NDNBOOST_PP_ENUM_PARAMS_Z(z, n, u)) ); } \
    NDNBOOST_IOSTREAMS_FORWARDING_FN_I(z, n, tuple) \
    /**/

// Disable forwarding constructors if first parameter type is the same
// as the device type
#if !defined(NDNBOOST_NO_SFINAE) && \
    !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x592))
# define NDNBOOST_IOSTREAMS_DISABLE_IF_SAME(device, param) \
    , typename ndnboost::disable_if< ndnboost::is_same<device, param> >::type* = 0 \
    /**/
#else 
# define NDNBOOST_IOSTREAMS_DISABLE_IF_SAME(device, param)
#endif

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_FORWARD_HPP_INCLUDED

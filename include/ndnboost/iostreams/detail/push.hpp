// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_PUSH_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_PUSH_HPP_INCLUDED 

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif                    
 
#include <ndnboost/config.hpp> // NDNBOOST_MSVC.
#include <ndnboost/detail/workaround.hpp>
#include <ndnboost/iostreams/categories.hpp>
#include <ndnboost/iostreams/categories.hpp>
#include <ndnboost/iostreams/detail/adapter/range_adapter.hpp>
#include <ndnboost/iostreams/detail/config/wide_streams.hpp>
#include <ndnboost/iostreams/detail/enable_if_stream.hpp>   
#include <ndnboost/iostreams/pipeline.hpp>   
#include <ndnboost/iostreams/detail/push_params.hpp>   
#include <ndnboost/iostreams/detail/resolve.hpp>
#include <ndnboost/mpl/bool.hpp>   
#include <ndnboost/preprocessor/cat.hpp> 
#include <ndnboost/preprocessor/control/iif.hpp>
#include <ndnboost/static_assert.hpp>
#include <ndnboost/type_traits/is_convertible.hpp>

//
// Macro: NDNBOOST_IOSTREAMS_DEFINE_PUSH_CONSTRUCTOR(name, mode, ch, helper).
// Description: Defines overloads with name 'name' which forward to a function
//      'helper' which takes a filter or devide by const reference.
//
#define NDNBOOST_IOSTREAMS_DEFINE_PUSH_CONSTRUCTOR(name, mode, ch, helper) \
    NDNBOOST_IOSTREAMS_DEFINE_PUSH_IMPL(name, mode, ch, helper, 0, ?) \
    /**/

//
// Macro: NDNBOOST_IOSTREAMS_DEFINE_PUSH(name, mode, ch, helper).
// Description: Defines constructors which forward to a function
//      'helper' which takes a filter or device by const reference.
//
#define NDNBOOST_IOSTREAMS_DEFINE_PUSH(name, mode, ch, helper) \
    NDNBOOST_IOSTREAMS_DEFINE_PUSH_IMPL(name, mode, ch, helper, 1, void) \
    /**/

//--------------------Definition of NDNBOOST_IOSTREAMS_DEFINE_PUSH_IMPL----------//
          
#define NDNBOOST_IOSTREAMS_ADAPT_STREAM(mode, ch, arg, helper, has_return) \
    this->helper( ::ndnboost::iostreams::detail::resolve<mode, ch>(arg) \
                  NDNBOOST_IOSTREAMS_PUSH_ARGS() ); \
    /**/

#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300) && \
    !NDNBOOST_WORKAROUND(__BORLANDC__, < 0x600) \
    /**/
# ifndef NDNBOOST_IOSTREAMS_NO_STREAM_TEMPLATES
#  define NDNBOOST_IOSTREAMS_DEFINE_PUSH_IMPL(name, mode, ch, helper, has_return, result) \
    template<typename CharType, typename TraitsType> \
    NDNBOOST_PP_IIF(has_return, result, explicit) \
    name(::std::basic_streambuf<CharType, TraitsType>& sb NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
    { NDNBOOST_IOSTREAMS_ADAPT_STREAM(mode, ch, sb, helper, has_return); } \
    template<typename CharType, typename TraitsType> \
    NDNBOOST_PP_IIF(has_return, result, explicit) \
    name(::std::basic_istream<CharType, TraitsType>& is NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
    { NDNBOOST_STATIC_ASSERT((!is_convertible<mode, output>::value)); \
      NDNBOOST_IOSTREAMS_ADAPT_STREAM(mode, ch, is, helper, has_return); } \
    template<typename CharType, typename TraitsType> \
    NDNBOOST_PP_IIF(has_return, result, explicit) \
    name(::std::basic_ostream<CharType, TraitsType>& os NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
    { NDNBOOST_STATIC_ASSERT((!is_convertible<mode, input>::value)); \
      NDNBOOST_IOSTREAMS_ADAPT_STREAM(mode, ch, os, helper, has_return); } \
    template<typename CharType, typename TraitsType> \
    NDNBOOST_PP_IIF(has_return, result, explicit) \
    name(::std::basic_iostream<CharType, TraitsType>& io NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
    { NDNBOOST_IOSTREAMS_ADAPT_STREAM(mode, ch, io, helper, has_return); } \
    template<typename Iter> \
    NDNBOOST_PP_IIF(has_return, result, explicit) \
    name(const iterator_range<Iter>& rng NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
    { NDNBOOST_PP_EXPR_IF(has_return, return) \
    this->helper( ::ndnboost::iostreams::detail::range_adapter< \
                      mode, iterator_range<Iter> \
                  >(rng) \
                  NDNBOOST_IOSTREAMS_PUSH_ARGS() ); } \
    template<typename Pipeline, typename Concept> \
    NDNBOOST_PP_IIF(has_return, result, explicit) \
    name(const ::ndnboost::iostreams::pipeline<Pipeline, Concept>& p) \
    { p.push(*this); } \
    template<typename T> \
    NDNBOOST_PP_IIF(has_return, result, explicit) \
    name(const T& t NDNBOOST_IOSTREAMS_PUSH_PARAMS() NDNBOOST_IOSTREAMS_DISABLE_IF_STREAM(T)) \
    { this->helper( ::ndnboost::iostreams::detail::resolve<mode, ch>(t) \
                    NDNBOOST_IOSTREAMS_PUSH_ARGS() ); } \
    /**/
# else // # ifndef NDNBOOST_IOSTREAMS_NO_STREAM_TEMPLATES
#  define NDNBOOST_IOSTREAMS_DEFINE_PUSH_IMPL(name, mode, ch, helper, has_return, result) \
    NDNBOOST_PP_IF(has_return, result, explicit) \
    name(::std::streambuf& sb NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
    { NDNBOOST_IOSTREAMS_ADAPT_STREAM(mode, ch, sb, helper, has_return); } \
    NDNBOOST_PP_IF(has_return, result, explicit) \
    name(::std::istream& is NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
    { NDNBOOST_STATIC_ASSERT((!is_convertible<mode, output>::value)); \
      NDNBOOST_IOSTREAMS_ADAPT_STREAM(mode, ch, is, helper, has_return); } \
    NDNBOOST_PP_IF(has_return, result, explicit) \
    name(::std::ostream& os NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
    { NDNBOOST_STATIC_ASSERT((!is_convertible<mode, input>::value)); \
      NDNBOOST_IOSTREAMS_ADAPT_STREAM(mode, ch, os, helper, has_return); } \
    NDNBOOST_PP_IF(has_return, result, explicit) \
    name(::std::iostream& io NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
    { NDNBOOST_IOSTREAMS_ADAPT_STREAM(mode, ch, io, helper, has_return); } \
    template<typename Iter> \
    NDNBOOST_PP_IF(has_return, result, explicit) \
    name(const iterator_range<Iter>& rng NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
    { NDNBOOST_PP_EXPR_IF(has_return, return) \
    this->helper( ::ndnboost::iostreams::detail::range_adapter< \
                      mode, iterator_range<Iter> \
                  >(rng) \
                  NDNBOOST_IOSTREAMS_PUSH_ARGS() ); } \
    template<typename Pipeline, typename Concept> \
    NDNBOOST_PP_IF(has_return, result, explicit) \
    name(const ::ndnboost::iostreams::pipeline<Pipeline, Concept>& p) \
    { p.push(*this); } \
    template<typename T> \
    NDNBOOST_PP_EXPR_IF(has_return, result) \
    name(const T& t NDNBOOST_IOSTREAMS_PUSH_PARAMS() NDNBOOST_IOSTREAMS_DISABLE_IF_STREAM(T)) \
    { this->helper( ::ndnboost::iostreams::detail::resolve<mode, ch>(t) \
                    NDNBOOST_IOSTREAMS_PUSH_ARGS() ); } \
    /**/
# endif // # ifndef NDNBOOST_IOSTREAMS_NO_STREAM_TEMPLATES
#else // #if VC6, VC7.0, Borland 5.x
# define NDNBOOST_IOSTREAMS_DEFINE_PUSH_IMPL(name, mode, ch, helper, has_return, result) \
    template<typename T> \
    void NDNBOOST_PP_CAT(name, _msvc_impl) \
    ( ::ndnboost::mpl::true_, const T& t NDNBOOST_IOSTREAMS_PUSH_PARAMS() ) \
    { t.push(*this); } \
    template<typename T> \
    void NDNBOOST_PP_CAT(name, _msvc_impl) \
    ( ::ndnboost::mpl::false_, const T& t NDNBOOST_IOSTREAMS_PUSH_PARAMS() ) \
    { this->helper( ::ndnboost::iostreams::detail::resolve<mode, ch>(t) \
                    NDNBOOST_IOSTREAMS_PUSH_ARGS() ); } \
    template<typename T> \
    NDNBOOST_PP_IF(has_return, result, explicit) \
    name(const T& t NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
    { \
        this->NDNBOOST_PP_CAT(name, _msvc_impl) \
              ( ::ndnboost::iostreams::detail::is_pipeline<T>(), \
                t NDNBOOST_IOSTREAMS_PUSH_ARGS() ); \
    } \
    /**/
#endif // #if VC6, VC7.0, Borland 5.x

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_PUSH_HPP_INCLUDED

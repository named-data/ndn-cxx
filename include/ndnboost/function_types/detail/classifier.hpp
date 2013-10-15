
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef NDNBOOST_FT_DETAIL_CLASSIFIER_HPP_INCLUDED
#define NDNBOOST_FT_DETAIL_CLASSIFIER_HPP_INCLUDED

#include <ndnboost/type.hpp>
#include <ndnboost/config.hpp>
#include <ndnboost/type_traits/config.hpp>
#include <ndnboost/type_traits/is_reference.hpp>
#include <ndnboost/type_traits/add_reference.hpp>

#include <ndnboost/function_types/config/config.hpp>
#include <ndnboost/function_types/property_tags.hpp>

namespace ndnboost { namespace function_types { namespace detail {

template<typename T> struct classifier;

template<std::size_t S> struct char_array { typedef char (&type)[S]; };

template<bits_t Flags, bits_t CCID, std::size_t Arity> struct encode_charr
{
  typedef typename char_array<
    ::ndnboost::function_types::detail::encode_charr_impl<Flags,CCID,Arity>::value 
  >::type type;
};

char NDNBOOST_TT_DECL classifier_impl(...);

#define NDNBOOST_FT_variations NDNBOOST_FT_function|NDNBOOST_FT_pointer|\
                            NDNBOOST_FT_member_pointer

#define NDNBOOST_FT_type_function(cc,name) NDNBOOST_FT_SYNTAX( \
    R NDNBOOST_PP_EMPTY,NDNBOOST_PP_LPAREN,cc,* NDNBOOST_PP_EMPTY,name,NDNBOOST_PP_RPAREN)

#define NDNBOOST_FT_type_function_pointer(cc,name) NDNBOOST_FT_SYNTAX( \
    R NDNBOOST_PP_EMPTY,NDNBOOST_PP_LPAREN,cc,** NDNBOOST_PP_EMPTY,name,NDNBOOST_PP_RPAREN)

#define NDNBOOST_FT_type_member_function_pointer(cc,name) NDNBOOST_FT_SYNTAX( \
    R NDNBOOST_PP_EMPTY,NDNBOOST_PP_LPAREN,cc,T0::** NDNBOOST_PP_EMPTY,name,NDNBOOST_PP_RPAREN)

#define NDNBOOST_FT_al_path ndnboost/function_types/detail/classifier_impl
#include <ndnboost/function_types/detail/pp_loop.hpp>

template<typename T> struct classifier_bits
{
  static typename ndnboost::add_reference<T>::type tester;

  NDNBOOST_STATIC_CONSTANT(bits_t,value = (bits_t)sizeof(
    ndnboost::function_types::detail::classifier_impl(& tester) 
  )-1);
};

template<typename T> struct classifier
{
  typedef detail::constant<
    ::ndnboost::function_types::detail::decode_bits<
      ::ndnboost::function_types::detail::classifier_bits<T>::value
    >::tag_bits > 
  bits;

  typedef detail::full_mask mask;
 
  typedef detail::constant<
    ::ndnboost::function_types::detail::decode_bits<
      ::ndnboost::function_types::detail::classifier_bits<T>::value
    >::arity > 
  function_arity;
};



} } } // namespace ::ndnboost::function_types::detail

#endif


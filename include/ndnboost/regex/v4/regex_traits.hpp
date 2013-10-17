/*
 *
 * Copyright (c) 2003
 * John Maddock
 *
 * Use, modification and distribution are subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
 
 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         regex_traits.hpp
  *   VERSION      see <ndnboost/version.hpp>
  *   DESCRIPTION: Declares regular expression traits classes.
  */

#ifndef NDNBOOST_REGEX_TRAITS_HPP_INCLUDED
#define NDNBOOST_REGEX_TRAITS_HPP_INCLUDED

#ifndef NDNBOOST_REGEX_CONFIG_HPP
#include <ndnboost/regex/config.hpp>
#endif
#ifndef NDNBOOST_REGEX_WORKAROUND_HPP
#include <ndnboost/regex/v4/regex_workaround.hpp>
#endif
#ifndef NDNBOOST_REGEX_SYNTAX_TYPE_HPP
#include <ndnboost/regex/v4/syntax_type.hpp>
#endif
#ifndef NDNBOOST_REGEX_ERROR_TYPE_HPP
#include <ndnboost/regex/v4/error_type.hpp>
#endif
#ifndef NDNBOOST_REGEX_TRAITS_DEFAULTS_HPP_INCLUDED
#include <ndnboost/regex/v4/regex_traits_defaults.hpp>
#endif
#ifndef NDNBOOST_NO_STD_LOCALE
#  ifndef NDNBOOST_CPP_REGEX_TRAITS_HPP_INCLUDED
#     include <ndnboost/regex/v4/cpp_regex_traits.hpp>
#  endif
#endif
#if !NDNBOOST_WORKAROUND(__BORLANDC__, < 0x560)
#  ifndef NDNBOOST_C_REGEX_TRAITS_HPP_INCLUDED
#     include <ndnboost/regex/v4/c_regex_traits.hpp>
#  endif
#endif
#if defined(_WIN32) && !defined(NDNBOOST_REGEX_NO_W32)
#  ifndef NDNBOOST_W32_REGEX_TRAITS_HPP_INCLUDED
#     include <ndnboost/regex/v4/w32_regex_traits.hpp>
#  endif
#endif
#ifndef NDNBOOST_REGEX_FWD_HPP_INCLUDED
#include <ndnboost/regex_fwd.hpp>
#endif

#include "ndnboost/mpl/has_xxx.hpp"
#include <ndnboost/static_assert.hpp>

#ifdef NDNBOOST_MSVC
#pragma warning(push)
#pragma warning(disable: 4103)
#endif
#ifdef NDNBOOST_HAS_ABI_HEADERS
#  include NDNBOOST_ABI_PREFIX
#endif
#ifdef NDNBOOST_MSVC
#pragma warning(pop)
#endif

namespace ndnboost{

template <class charT, class implementationT >
struct regex_traits : public implementationT
{
   regex_traits() : implementationT() {}
};

//
// class regex_traits_wrapper.
// this is what our implementation will actually store;
// it provides default implementations of the "optional"
// interfaces that we support, in addition to the
// required "standard" ones:
//
namespace re_detail{
#if !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !NDNBOOST_WORKAROUND(__HP_aCC, < 60000)
NDNBOOST_MPL_HAS_XXX_TRAIT_DEF(boost_extensions_tag)
#else
template<class T>
struct has_boost_extensions_tag
{
   NDNBOOST_STATIC_CONSTANT(bool, value = false);
};
#endif

template <class BaseT>
struct default_wrapper : public BaseT
{
   typedef typename BaseT::char_type char_type;
   std::string error_string(::ndnboost::regex_constants::error_type e)const
   {
      return ::ndnboost::re_detail::get_default_error_string(e);
   }
   ::ndnboost::regex_constants::syntax_type syntax_type(char_type c)const
   {
      return ((c & 0x7f) == c) ? get_default_syntax_type(static_cast<char>(c)) : ::ndnboost::regex_constants::syntax_char;
   }
   ::ndnboost::regex_constants::escape_syntax_type escape_syntax_type(char_type c)const
   {
      return ((c & 0x7f) == c) ? get_default_escape_syntax_type(static_cast<char>(c)) : ::ndnboost::regex_constants::escape_type_identity;
   }
   int toi(const char_type*& p1, const char_type* p2, int radix)const
   {
      return ::ndnboost::re_detail::global_toi(p1, p2, radix, *this);
   }
   char_type translate(char_type c, bool icase)const
   {
      return (icase ? this->translate_nocase(c) : this->translate(c));
   }
   char_type translate(char_type c)const
   {
      return BaseT::translate(c);
   }
   char_type tolower(char_type c)const
   {
      return ::ndnboost::re_detail::global_lower(c);
   }
   char_type toupper(char_type c)const
   {
      return ::ndnboost::re_detail::global_upper(c);
   }
};

template <class BaseT, bool has_extensions>
struct compute_wrapper_base
{
   typedef BaseT type;
};
#if !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !NDNBOOST_WORKAROUND(__HP_aCC, < 60000)
template <class BaseT>
struct compute_wrapper_base<BaseT, false>
{
   typedef default_wrapper<BaseT> type;
};
#else
template <>
struct compute_wrapper_base<c_regex_traits<char>, false>
{
   typedef default_wrapper<c_regex_traits<char> > type;
};
#ifndef NDNBOOST_NO_WREGEX
template <>
struct compute_wrapper_base<c_regex_traits<wchar_t>, false>
{
   typedef default_wrapper<c_regex_traits<wchar_t> > type;
};
#endif
#endif

} // namespace re_detail

template <class BaseT>
struct regex_traits_wrapper 
   : public ::ndnboost::re_detail::compute_wrapper_base<
               BaseT, 
               ::ndnboost::re_detail::has_boost_extensions_tag<BaseT>::value
            >::type
{
   regex_traits_wrapper(){}
private:
   regex_traits_wrapper(const regex_traits_wrapper&);
   regex_traits_wrapper& operator=(const regex_traits_wrapper&);
};

} // namespace ndnboost

#ifdef NDNBOOST_MSVC
#pragma warning(push)
#pragma warning(disable: 4103)
#endif
#ifdef NDNBOOST_HAS_ABI_HEADERS
#  include NDNBOOST_ABI_SUFFIX
#endif
#ifdef NDNBOOST_MSVC
#pragma warning(pop)
#endif

#endif // include


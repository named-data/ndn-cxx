
#ifndef NDNBOOST_MPL_HAS_XXX_HPP_INCLUDED
#define NDNBOOST_MPL_HAS_XXX_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2006
// Copyright David Abrahams 2002-2003
// Copyright Daniel Walker 2007
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: has_xxx.hpp 64146 2010-07-19 00:46:31Z djwalker $
// $Date: 2010-07-18 17:46:31 -0700 (Sun, 18 Jul 2010) $
// $Revision: 64146 $

#include <ndnboost/mpl/bool.hpp>
#include <ndnboost/mpl/aux_/na_spec.hpp>
#include <ndnboost/mpl/aux_/type_wrapper.hpp>
#include <ndnboost/mpl/aux_/yes_no.hpp>
#include <ndnboost/mpl/aux_/config/gcc.hpp>
#include <ndnboost/mpl/aux_/config/has_xxx.hpp>
#include <ndnboost/mpl/aux_/config/msvc_typename.hpp>
#include <ndnboost/mpl/aux_/config/msvc.hpp>
#include <ndnboost/mpl/aux_/config/static_constant.hpp>
#include <ndnboost/mpl/aux_/config/workaround.hpp>

#include <ndnboost/preprocessor/array/elem.hpp>
#include <ndnboost/preprocessor/cat.hpp>
#include <ndnboost/preprocessor/control/if.hpp>
#include <ndnboost/preprocessor/repetition/enum_params.hpp>
#include <ndnboost/preprocessor/repetition/enum_trailing_params.hpp>

#if NDNBOOST_WORKAROUND( __BORLANDC__, NDNBOOST_TESTED_AT(0x590) )
# include <ndnboost/type_traits/is_class.hpp>
#endif

#if !defined(NDNBOOST_MPL_CFG_NO_HAS_XXX)

#   if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300)

// agurt, 11/sep/02: MSVC-specific version (< 7.1), based on a USENET 
// newsgroup's posting by John Madsen (comp.lang.c++.moderated, 
// 1999-11-12 19:17:06 GMT); the code is _not_ standard-conforming, but 
// it works way more reliably than the SFINAE-based implementation

// Modified dwa 8/Oct/02 to handle reference types.

#   include <ndnboost/mpl/if.hpp>
#   include <ndnboost/mpl/bool.hpp>

namespace ndnboost { namespace mpl { namespace aux {

struct has_xxx_tag;

#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, == 1300)
template< typename U > struct msvc_incomplete_array
{
    typedef char (&type)[sizeof(U) + 1];
};
#endif

template< typename T >
struct msvc_is_incomplete
{
    // MSVC is capable of some kinds of SFINAE.  If U is an incomplete
    // type, it won't pick the second overload
    static char tester(...);

#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, == 1300)
    template< typename U >
    static typename msvc_incomplete_array<U>::type tester(type_wrapper<U>);
#else
    template< typename U >
    static char (& tester(type_wrapper<U>) )[sizeof(U)+1];
#endif 
    
    NDNBOOST_STATIC_CONSTANT(bool, value = 
          sizeof(tester(type_wrapper<T>())) == 1
        );
};

template<>
struct msvc_is_incomplete<int>
{
    NDNBOOST_STATIC_CONSTANT(bool, value = false);
};

}}}

#   define NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF_(trait, name, default_) \
template< typename T, typename name = ::ndnboost::mpl::aux::has_xxx_tag > \
struct NDNBOOST_PP_CAT(trait,_impl) : T \
{ \
    static ndnboost::mpl::aux::no_tag \
    test(void(*)(::ndnboost::mpl::aux::has_xxx_tag)); \
    \
    static ndnboost::mpl::aux::yes_tag test(...); \
    \
    NDNBOOST_STATIC_CONSTANT(bool, value = \
          sizeof(test(static_cast<void(*)(name)>(0))) \
            != sizeof(ndnboost::mpl::aux::no_tag) \
        ); \
    typedef ndnboost::mpl::bool_<value> type; \
}; \
\
template< typename T, typename fallback_ = ndnboost::mpl::bool_<default_> > \
struct trait \
    : ndnboost::mpl::if_c< \
          ndnboost::mpl::aux::msvc_is_incomplete<T>::value \
        , ndnboost::mpl::bool_<false> \
        , NDNBOOST_PP_CAT(trait,_impl)<T> \
        >::type \
{ \
}; \
\
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, void) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, bool) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, char) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, signed char) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, unsigned char) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, signed short) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, unsigned short) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, signed int) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, unsigned int) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, signed long) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, unsigned long) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, float) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, double) \
NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, long double) \
/**/

#   define NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, T) \
template<> struct trait<T> \
{ \
    NDNBOOST_STATIC_CONSTANT(bool, value = false); \
    typedef ndnboost::mpl::bool_<false> type; \
}; \
/**/

#if !defined(NDNBOOST_NO_INTRINSIC_WCHAR_T)
#   define NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, unused) \
    NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF_(trait, name, unused) \
    NDNBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, wchar_t) \
/**/
#else
#   define NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, unused) \
    NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF_(trait, name, unused) \
/**/
#endif


// SFINAE-based implementations below are derived from a USENET newsgroup's 
// posting by Rani Sharoni (comp.lang.c++.moderated, 2002-03-17 07:45:09 PST)

#   elif NDNBOOST_WORKAROUND(NDNBOOST_MSVC, NDNBOOST_TESTED_AT(1400)) \
      || NDNBOOST_WORKAROUND(__IBMCPP__, <= 700)

// MSVC 7.1+ & VACPP

// agurt, 15/jun/05: replace overload-based SFINAE implementation with SFINAE
// applied to partial specialization to fix some apparently random failures 
// (thanks to Daniel Wallin for researching this!)

#   define NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, default_) \
template< typename T > \
struct NDNBOOST_PP_CAT(trait, _msvc_sfinae_helper) \
{ \
    typedef void type; \
};\
\
template< typename T, typename U = void > \
struct NDNBOOST_PP_CAT(trait,_impl_) \
{ \
    NDNBOOST_STATIC_CONSTANT(bool, value = false); \
    typedef ndnboost::mpl::bool_<value> type; \
}; \
\
template< typename T > \
struct NDNBOOST_PP_CAT(trait,_impl_)< \
      T \
    , typename NDNBOOST_PP_CAT(trait, _msvc_sfinae_helper)< typename T::name >::type \
    > \
{ \
    NDNBOOST_STATIC_CONSTANT(bool, value = true); \
    typedef ndnboost::mpl::bool_<value> type; \
}; \
\
template< typename T, typename fallback_ = ndnboost::mpl::bool_<default_> > \
struct trait \
    : NDNBOOST_PP_CAT(trait,_impl_)<T> \
{ \
}; \
/**/

#   elif NDNBOOST_WORKAROUND( __BORLANDC__, NDNBOOST_TESTED_AT(0x590) )

#   define NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_BCB_DEF(trait, trait_tester, name, default_) \
template< typename T, bool IS_CLASS > \
struct trait_tester \
{ \
    NDNBOOST_STATIC_CONSTANT( bool,  value = false ); \
}; \
template< typename T > \
struct trait_tester< T, true > \
{ \
    struct trait_tester_impl \
    { \
        template < class U > \
        static int  resolve( ndnboost::mpl::aux::type_wrapper<U> const volatile * \
                           , ndnboost::mpl::aux::type_wrapper<typename U::name >* = 0 ); \
        static char resolve( ... ); \
    }; \
    typedef ndnboost::mpl::aux::type_wrapper<T> t_; \
    NDNBOOST_STATIC_CONSTANT( bool, value = ( sizeof( trait_tester_impl::resolve( static_cast< t_ * >(0) ) ) == sizeof(int) ) ); \
}; \
template< typename T, typename fallback_ = ndnboost::mpl::bool_<default_> > \
struct trait           \
{                      \
    NDNBOOST_STATIC_CONSTANT( bool, value = (trait_tester< T, ndnboost::is_class< T >::value >::value) );     \
    typedef ndnboost::mpl::bool_< trait< T, fallback_ >::value > type; \
};

#   define NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, default_) \
    NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_BCB_DEF( trait \
                                         , NDNBOOST_PP_CAT(trait,_tester)      \
                                         , name       \
                                         , default_ ) \
/**/

#   else // other SFINAE-capable compilers

#   define NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, default_) \
template< typename T, typename fallback_ = ndnboost::mpl::bool_<default_> > \
struct trait \
{ \
    struct gcc_3_2_wknd \
    { \
        template< typename U > \
        static ndnboost::mpl::aux::yes_tag test( \
              ndnboost::mpl::aux::type_wrapper<U> const volatile* \
            , ndnboost::mpl::aux::type_wrapper<NDNBOOST_MSVC_TYPENAME U::name>* = 0 \
            ); \
    \
        static ndnboost::mpl::aux::no_tag test(...); \
    }; \
    \
    typedef ndnboost::mpl::aux::type_wrapper<T> t_; \
    NDNBOOST_STATIC_CONSTANT(bool, value = \
          sizeof(gcc_3_2_wknd::test(static_cast<t_*>(0))) \
            == sizeof(ndnboost::mpl::aux::yes_tag) \
        ); \
    typedef ndnboost::mpl::bool_<value> type; \
}; \
/**/

#   endif // NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300)


#else // NDNBOOST_MPL_CFG_NO_HAS_XXX

// placeholder implementation

#   define NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, default_) \
template< typename T, typename fallback_ = ndnboost::mpl::bool_<default_> > \
struct trait \
{ \
    NDNBOOST_STATIC_CONSTANT(bool, value = fallback_::value); \
    typedef fallback_ type; \
}; \
/**/

#endif

#define NDNBOOST_MPL_HAS_XXX_TRAIT_DEF(name) \
    NDNBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(NDNBOOST_PP_CAT(has_,name), name, false) \
/**/


#if !defined(NDNBOOST_MPL_CFG_NO_HAS_XXX_TEMPLATE)

// Create a boolean Metafunction to detect a nested template
// member. This implementation is based on a USENET newsgroup's
// posting by Aleksey Gurtovoy (comp.lang.c++.moderated, 2002-03-19),
// Rani Sharoni's USENET posting cited above, the non-template has_xxx
// implementations above, and discussion on the Boost mailing list.

#   if !defined(NDNBOOST_MPL_HAS_XXX_NO_WRAPPED_TYPES)
#     if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1400)
#       define NDNBOOST_MPL_HAS_XXX_NO_WRAPPED_TYPES 1
#     endif
#   endif

#   if !defined(NDNBOOST_MPL_HAS_XXX_NO_EXPLICIT_TEST_FUNCTION)
#     if (defined(NDNBOOST_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS))
#       define NDNBOOST_MPL_HAS_XXX_NO_EXPLICIT_TEST_FUNCTION 1
#     endif
#   endif

#   if !defined(NDNBOOST_MPL_HAS_XXX_NEEDS_TEMPLATE_SFINAE)
#     if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1400)
#       define NDNBOOST_MPL_HAS_XXX_NEEDS_TEMPLATE_SFINAE 1
#     endif
#   endif

// NOTE: Many internal implementation macros take a Boost.Preprocessor
// array argument called args which is of the following form.
//           ( 4, ( trait, name, max_arity, default_ ) )

#   define NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args) \
      NDNBOOST_PP_CAT(NDNBOOST_PP_ARRAY_ELEM(0, args) , _introspect) \
    /**/

#   define NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) \
      NDNBOOST_PP_CAT(NDNBOOST_PP_CAT(NDNBOOST_PP_ARRAY_ELEM(0, args) , _substitute), n) \
    /**/

#   define NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args) \
      NDNBOOST_PP_CAT(NDNBOOST_PP_ARRAY_ELEM(0, args) , _test) \
    /**/

// Thanks to Guillaume Melquiond for pointing out the need for the
// "substitute" template as an argument to the overloaded test
// functions to get SFINAE to work for member templates with the
// correct name but different number of arguments.
#   define NDNBOOST_MPL_HAS_MEMBER_MULTI_SUBSTITUTE(z, n, args) \
      template< \
          template< NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_INC(n), typename V) > class V \
       > \
      struct NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) { \
      }; \
    /**/

#   define NDNBOOST_MPL_HAS_MEMBER_SUBSTITUTE(args, substitute_macro) \
      NDNBOOST_PP_REPEAT( \
          NDNBOOST_PP_ARRAY_ELEM(2, args) \
        , NDNBOOST_MPL_HAS_MEMBER_MULTI_SUBSTITUTE \
        , args \
      ) \
    /**/

#   if !NDNBOOST_MPL_HAS_XXX_NO_EXPLICIT_TEST_FUNCTION
#     define NDNBOOST_MPL_HAS_MEMBER_REJECT(args, member_macro) \
        template< typename V > \
        static ndnboost::mpl::aux::no_tag \
        NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)(...); \
      /**/
#   else
#     define NDNBOOST_MPL_HAS_MEMBER_REJECT(args, member_macro) \
        static ndnboost::mpl::aux::no_tag \
        NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)(...); \
      /**/
#   endif

#   if !NDNBOOST_MPL_HAS_XXX_NO_WRAPPED_TYPES
#     define NDNBOOST_MPL_HAS_MEMBER_MULTI_ACCEPT(z, n, args) \
        template< typename V > \
        static ndnboost::mpl::aux::yes_tag \
        NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)( \
            ndnboost::mpl::aux::type_wrapper< V > const volatile* \
          , NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) < \
                V::template NDNBOOST_PP_ARRAY_ELEM(1, args) \
            >* = 0 \
        ); \
      /**/
#     define NDNBOOST_MPL_HAS_MEMBER_ACCEPT(args, member_macro) \
        NDNBOOST_PP_REPEAT( \
            NDNBOOST_PP_ARRAY_ELEM(2, args) \
          , NDNBOOST_MPL_HAS_MEMBER_MULTI_ACCEPT \
          , args \
        ) \
      /**/
#   else
#     define NDNBOOST_MPL_HAS_MEMBER_ACCEPT(args, member_macro) \
        template< typename V > \
        static ndnboost::mpl::aux::yes_tag \
        NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)( \
            V const volatile* \
          , member_macro(args, V, T)* = 0 \
        ); \
      /**/
#   endif

#   if !NDNBOOST_MPL_HAS_XXX_NO_EXPLICIT_TEST_FUNCTION
#     define NDNBOOST_MPL_HAS_MEMBER_TEST(args) \
          sizeof(NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)< U >(0)) \
              == sizeof(ndnboost::mpl::aux::yes_tag) \
      /**/
#   else
#     if !NDNBOOST_MPL_HAS_XXX_NO_WRAPPED_TYPES
#       define NDNBOOST_MPL_HAS_MEMBER_TEST(args) \
          sizeof( \
              NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)( \
                  static_cast< ndnboost::mpl::aux::type_wrapper< U >* >(0) \
              ) \
          ) == sizeof(ndnboost::mpl::aux::yes_tag) \
        /**/
#     else
#       define NDNBOOST_MPL_HAS_MEMBER_TEST(args) \
          sizeof( \
              NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)( \
                  static_cast< U* >(0) \
              ) \
          ) == sizeof(ndnboost::mpl::aux::yes_tag) \
        /**/
#     endif
#   endif

#   define NDNBOOST_MPL_HAS_MEMBER_INTROSPECT( \
               args, substitute_macro, member_macro \
           ) \
      template< typename U > \
      struct NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args) { \
          NDNBOOST_MPL_HAS_MEMBER_SUBSTITUTE(args, substitute_macro) \
          NDNBOOST_MPL_HAS_MEMBER_REJECT(args, member_macro) \
          NDNBOOST_MPL_HAS_MEMBER_ACCEPT(args, member_macro) \
          NDNBOOST_STATIC_CONSTANT( \
              bool, value = NDNBOOST_MPL_HAS_MEMBER_TEST(args) \
          ); \
          typedef ndnboost::mpl::bool_< value > type; \
      }; \
    /**/

#   define NDNBOOST_MPL_HAS_MEMBER_IMPLEMENTATION( \
               args, introspect_macro, substitute_macro, member_macro \
           ) \
      template< \
          typename T \
        , typename fallback_ \
              = ndnboost::mpl::bool_< NDNBOOST_PP_ARRAY_ELEM(3, args) > \
      > \
      class NDNBOOST_PP_ARRAY_ELEM(0, args) { \
          introspect_macro(args, substitute_macro, member_macro) \
      public: \
          static const bool value \
              = NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args)< T >::value; \
          typedef typename NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args)< \
              T \
          >::type type; \
      }; \
    /**/

// NDNBOOST_MPL_HAS_MEMBER_WITH_FUNCTION_SFINAE expands to the full
// implementation of the function-based metafunction. Compile with -E
// to see the preprocessor output for this macro.
#   define NDNBOOST_MPL_HAS_MEMBER_WITH_FUNCTION_SFINAE( \
               args, substitute_macro, member_macro \
           ) \
      NDNBOOST_MPL_HAS_MEMBER_IMPLEMENTATION( \
          args \
        , NDNBOOST_MPL_HAS_MEMBER_INTROSPECT \
        , substitute_macro \
        , member_macro \
      ) \
    /**/

#   if NDNBOOST_MPL_HAS_XXX_NEEDS_TEMPLATE_SFINAE

#     if !defined(NDNBOOST_MPL_HAS_XXX_NEEDS_NAMESPACE_LEVEL_SUBSTITUTE)
#       if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1400)
#         define NDNBOOST_MPL_HAS_XXX_NEEDS_NAMESPACE_LEVEL_SUBSTITUTE 1
#       endif
#     endif

#     if !NDNBOOST_MPL_HAS_XXX_NEEDS_NAMESPACE_LEVEL_SUBSTITUTE
#       define NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME_WITH_TEMPLATE_SFINAE( \
                   args, n \
               ) \
          NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) \
        /**/
#     else
#       define NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME_WITH_TEMPLATE_SFINAE( \
                   args, n \
               ) \
          NDNBOOST_PP_CAT( \
              boost_mpl_has_xxx_ \
            , NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) \
          ) \
        /**/
#     endif

#     define NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_TAG_NAME( \
                 args \
             ) \
        NDNBOOST_PP_CAT( \
            NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME_WITH_TEMPLATE_SFINAE( \
                args, 0 \
            ) \
          , _tag \
        ) \
      /**/

#     define NDNBOOST_MPL_HAS_MEMBER_MULTI_SUBSTITUTE_WITH_TEMPLATE_SFINAE( \
                 z, n, args \
             ) \
        template< \
             template< NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_INC(n), typename U) > class U \
        > \
        struct NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME_WITH_TEMPLATE_SFINAE( \
                args, n \
               ) { \
            typedef \
                NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_TAG_NAME(args) \
                type; \
        }; \
      /**/

#     define NDNBOOST_MPL_HAS_MEMBER_SUBSTITUTE_WITH_TEMPLATE_SFINAE( \
                 args, substitute_macro \
             ) \
        typedef void \
            NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_TAG_NAME(args); \
        NDNBOOST_PP_REPEAT( \
            NDNBOOST_PP_ARRAY_ELEM(2, args) \
          , NDNBOOST_MPL_HAS_MEMBER_MULTI_SUBSTITUTE_WITH_TEMPLATE_SFINAE \
          , args \
        ) \
      /**/

#     define NDNBOOST_MPL_HAS_MEMBER_REJECT_WITH_TEMPLATE_SFINAE( \
                 args, member_macro \
             ) \
        template< \
            typename U \
          , typename V \
                = NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_TAG_NAME(args) \
        > \
        struct NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args) { \
            NDNBOOST_STATIC_CONSTANT(bool, value = false); \
            typedef ndnboost::mpl::bool_< value > type; \
        }; \
      /**/

#     define NDNBOOST_MPL_HAS_MEMBER_MULTI_ACCEPT_WITH_TEMPLATE_SFINAE( \
                 z, n, args \
             ) \
        template< typename U > \
        struct NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)< \
            U \
          , typename \
                NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME_WITH_TEMPLATE_SFINAE( \
                    args, n \
                )< \
                    NDNBOOST_MSVC_TYPENAME U::NDNBOOST_PP_ARRAY_ELEM(1, args)< > \
                >::type \
        > { \
            NDNBOOST_STATIC_CONSTANT(bool, value = true); \
            typedef ndnboost::mpl::bool_< value > type; \
        }; \
      /**/

#     define NDNBOOST_MPL_HAS_MEMBER_ACCEPT_WITH_TEMPLATE_SFINAE( \
                 args, member_macro \
             ) \
        NDNBOOST_PP_REPEAT( \
            NDNBOOST_PP_ARRAY_ELEM(2, args) \
          , NDNBOOST_MPL_HAS_MEMBER_MULTI_ACCEPT_WITH_TEMPLATE_SFINAE \
          , args \
        ) \
      /**/

#     define NDNBOOST_MPL_HAS_MEMBER_INTROSPECT_WITH_TEMPLATE_SFINAE( \
                 args, substitute_macro, member_macro \
             ) \
        NDNBOOST_MPL_HAS_MEMBER_REJECT_WITH_TEMPLATE_SFINAE(args, member_macro) \
        NDNBOOST_MPL_HAS_MEMBER_ACCEPT_WITH_TEMPLATE_SFINAE(args, member_macro) \
        template< typename U > \
        struct NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args) \
            : NDNBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)< U > { \
        }; \
      /**/
 
// NDNBOOST_MPL_HAS_MEMBER_WITH_TEMPLATE_SFINAE expands to the full
// implementation of the template-based metafunction. Compile with -E
// to see the preprocessor output for this macro.
//
// Note that if NDNBOOST_MPL_HAS_XXX_NEEDS_NAMESPACE_LEVEL_SUBSTITUTE is
// defined NDNBOOST_MPL_HAS_MEMBER_SUBSTITUTE_WITH_TEMPLATE_SFINAE needs
// to be expanded at namespace level before
// NDNBOOST_MPL_HAS_MEMBER_WITH_TEMPLATE_SFINAE can be used.
#     define NDNBOOST_MPL_HAS_MEMBER_WITH_TEMPLATE_SFINAE( \
                 args, substitute_macro, member_macro \
             ) \
        NDNBOOST_MPL_HAS_MEMBER_SUBSTITUTE_WITH_TEMPLATE_SFINAE( \
            args, substitute_macro \
        ) \
        NDNBOOST_MPL_HAS_MEMBER_IMPLEMENTATION( \
            args \
          , NDNBOOST_MPL_HAS_MEMBER_INTROSPECT_WITH_TEMPLATE_SFINAE \
          , substitute_macro \
          , member_macro \
        ) \
      /**/

#   endif // NDNBOOST_MPL_HAS_XXX_NEEDS_TEMPLATE_SFINAE

// Note: In the current implementation the parameter and access macros
// are no longer expanded.
#   if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1400)
#     define NDNBOOST_MPL_HAS_XXX_TEMPLATE_NAMED_DEF(trait, name, default_) \
        NDNBOOST_MPL_HAS_MEMBER_WITH_FUNCTION_SFINAE( \
            ( 4, ( trait, name, NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY, default_ ) ) \
          , NDNBOOST_MPL_HAS_MEMBER_TEMPLATE_SUBSTITUTE_PARAMETER \
          , NDNBOOST_MPL_HAS_MEMBER_TEMPLATE_ACCESS \
        ) \
      /**/
#   else
#     define NDNBOOST_MPL_HAS_XXX_TEMPLATE_NAMED_DEF(trait, name, default_) \
        NDNBOOST_MPL_HAS_MEMBER_WITH_TEMPLATE_SFINAE( \
            ( 4, ( trait, name, NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY, default_ ) ) \
          , NDNBOOST_MPL_HAS_MEMBER_TEMPLATE_SUBSTITUTE_PARAMETER \
          , NDNBOOST_MPL_HAS_MEMBER_TEMPLATE_ACCESS \
        ) \
      /**/
#   endif

#else // NDNBOOST_MPL_CFG_NO_HAS_XXX_TEMPLATE

// placeholder implementation

#   define NDNBOOST_MPL_HAS_XXX_TEMPLATE_NAMED_DEF(trait, name, default_) \
      template< typename T \
              , typename fallback_ = ndnboost::mpl::bool_< default_ > > \
      struct trait { \
          NDNBOOST_STATIC_CONSTANT(bool, value = fallback_::value); \
          typedef fallback_ type; \
      }; \
    /**/

#endif // NDNBOOST_MPL_CFG_NO_HAS_XXX_TEMPLATE

#   define NDNBOOST_MPL_HAS_XXX_TEMPLATE_DEF(name) \
      NDNBOOST_MPL_HAS_XXX_TEMPLATE_NAMED_DEF( \
          NDNBOOST_PP_CAT(has_, name), name, false \
      ) \
    /**/

#endif // NDNBOOST_MPL_HAS_XXX_HPP_INCLUDED

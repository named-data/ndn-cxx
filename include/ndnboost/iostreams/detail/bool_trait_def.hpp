// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_BOOL_TRAIT_DEF_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_BOOL_TRAIT_DEF_HPP_INCLUDED     

#include <ndnboost/config.hpp> // NDNBOOST_STATIC_CONSTANT.
#include <ndnboost/iostreams/detail/template_params.hpp>
#include <ndnboost/mpl/aux_/lambda_support.hpp>
#include <ndnboost/mpl/bool.hpp>
#include <ndnboost/preprocessor/cat.hpp>
#include <ndnboost/preprocessor/repetition/enum_params.hpp>
#include <ndnboost/type_traits/detail/yes_no_type.hpp>
 
// 
// Macro name: NDNBOOST_IOSTREAMS_BOOL_TRAIT_DEF
// Description: Used to generate the traits classes is_istream, is_ostream,
//      etc.
//
#if NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x582))
# define NDNBOOST_IOSTREAMS_TRAIT_NAMESPACE(trait)
#else
# define NDNBOOST_IOSTREAMS_TRAIT_NAMESPACE(trait) NDNBOOST_PP_CAT(trait, _impl_):: 
#endif
#define NDNBOOST_IOSTREAMS_BOOL_TRAIT_DEF(trait, type, arity) \
    namespace NDNBOOST_PP_CAT(trait, _impl_) { \
      NDNBOOST_IOSTREAMS_TEMPLATE_PARAMS(arity, T) \
      type_traits::yes_type helper \
          (const volatile type NDNBOOST_IOSTREAMS_TEMPLATE_ARGS(arity, T)*); \
      type_traits::no_type helper(...); \
      template<typename T> \
      struct impl { \
           NDNBOOST_STATIC_CONSTANT(bool, value = \
           (sizeof(NDNBOOST_IOSTREAMS_TRAIT_NAMESPACE(trait) \
              helper(static_cast<T*>(0))) == \
                sizeof(type_traits::yes_type))); \
      }; \
    } \
    template<typename T> \
    struct trait \
        : mpl::bool_<NDNBOOST_PP_CAT(trait, _impl_)::impl<T>::value> \
    { NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(1, trait, (T)) }; \
    /**/

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_BOOL_TRAIT_DEF_HPP_INCLUDED

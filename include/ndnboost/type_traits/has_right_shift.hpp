//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_HAS_RIGHT_SHIFT_HPP_INCLUDED
#define NDNBOOST_TT_HAS_RIGHT_SHIFT_HPP_INCLUDED

#define NDNBOOST_TT_TRAIT_NAME has_right_shift
#define NDNBOOST_TT_TRAIT_OP >>
#define NDNBOOST_TT_FORBIDDEN_IF\
   ::ndnboost::type_traits::ice_or<\
      /* Lhs==fundamental and Rhs==fundamental and (Lhs!=integral or Rhs!=integral) */\
      ::ndnboost::type_traits::ice_and<\
         ::ndnboost::is_fundamental< Lhs_nocv >::value,\
         ::ndnboost::is_fundamental< Rhs_nocv >::value,\
         ::ndnboost::type_traits::ice_or<\
            ::ndnboost::type_traits::ice_not< ::ndnboost::is_integral< Lhs_noref >::value >::value,\
            ::ndnboost::type_traits::ice_not< ::ndnboost::is_integral< Rhs_noref >::value >::value\
         >::value\
      >::value,\
      /* Lhs==fundamental and Rhs==pointer */\
      ::ndnboost::type_traits::ice_and<\
         ::ndnboost::is_fundamental< Lhs_nocv >::value,\
         ::ndnboost::is_pointer< Rhs_noref >::value\
      >::value,\
      /* Rhs==fundamental and Lhs==pointer */\
      ::ndnboost::type_traits::ice_and<\
         ::ndnboost::is_fundamental< Rhs_nocv >::value,\
         ::ndnboost::is_pointer< Lhs_noref >::value\
      >::value,\
      /* Lhs==pointer and Rhs==pointer */\
      ::ndnboost::type_traits::ice_and<\
         ::ndnboost::is_pointer< Lhs_noref >::value,\
         ::ndnboost::is_pointer< Rhs_noref >::value\
      >::value\
   >::value


#include <ndnboost/type_traits/detail/has_binary_operator.hpp>

#undef NDNBOOST_TT_TRAIT_NAME
#undef NDNBOOST_TT_TRAIT_OP
#undef NDNBOOST_TT_FORBIDDEN_IF

#endif

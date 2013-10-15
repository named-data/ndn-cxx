//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_HAS_PRE_INCREMENT_HPP_INCLUDED
#define NDNBOOST_TT_HAS_PRE_INCREMENT_HPP_INCLUDED

#define NDNBOOST_TT_TRAIT_NAME has_pre_increment
#define NDNBOOST_TT_TRAIT_OP ++
#define NDNBOOST_TT_FORBIDDEN_IF\
   ::ndnboost::type_traits::ice_or<\
      /* bool */\
      ::ndnboost::is_same< bool, Rhs_nocv >::value,\
      /* void* */\
      ::ndnboost::type_traits::ice_and<\
         ::ndnboost::is_pointer< Rhs_noref >::value,\
         ::ndnboost::is_void< Rhs_noptr >::value\
      >::value,\
      /* (fundamental or pointer) and const */\
      ::ndnboost::type_traits::ice_and<\
         ::ndnboost::type_traits::ice_or<\
            ::ndnboost::is_fundamental< Rhs_nocv >::value,\
            ::ndnboost::is_pointer< Rhs_noref >::value\
         >::value,\
         ::ndnboost::is_const< Rhs_noref >::value\
      >::value\
   >::value


#include <ndnboost/type_traits/detail/has_prefix_operator.hpp>

#undef NDNBOOST_TT_TRAIT_NAME
#undef NDNBOOST_TT_TRAIT_OP
#undef NDNBOOST_TT_FORBIDDEN_IF

#endif

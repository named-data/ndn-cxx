//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_HAS_POST_INCREMENT_HPP_INCLUDED
#define BOOST_TT_HAS_POST_INCREMENT_HPP_INCLUDED

#define BOOST_TT_TRAIT_NAME has_post_increment
#define BOOST_TT_TRAIT_OP ++
#define BOOST_TT_FORBIDDEN_IF\
   ::ndnboost::type_traits::ice_or<\
      /* bool */\
      ::ndnboost::is_same< bool, Lhs_nocv >::value,\
      /* void* */\
      ::ndnboost::type_traits::ice_and<\
         ::ndnboost::is_pointer< Lhs_noref >::value,\
         ::ndnboost::is_void< Lhs_noptr >::value\
      >::value,\
      /* (fundamental or pointer) and const */\
      ::ndnboost::type_traits::ice_and<\
         ::ndnboost::type_traits::ice_or<\
            ::ndnboost::is_fundamental< Lhs_nocv >::value,\
            ::ndnboost::is_pointer< Lhs_noref >::value\
         >::value,\
         ::ndnboost::is_const< Lhs_noref >::value\
      >::value\
   >::value


#include <ndnboost/type_traits/detail/has_postfix_operator.hpp>

#undef BOOST_TT_TRAIT_NAME
#undef BOOST_TT_TRAIT_OP
#undef BOOST_TT_FORBIDDEN_IF

#endif

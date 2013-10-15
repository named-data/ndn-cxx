//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_HAS_UNARY_MINUS_HPP_INCLUDED
#define NDNBOOST_TT_HAS_UNARY_MINUS_HPP_INCLUDED

#define NDNBOOST_TT_TRAIT_NAME has_unary_minus
#define NDNBOOST_TT_TRAIT_OP -
#define NDNBOOST_TT_FORBIDDEN_IF\
   /* pointer */\
   ::ndnboost::is_pointer< Rhs_noref >::value


#include <ndnboost/type_traits/detail/has_prefix_operator.hpp>

#undef NDNBOOST_TT_TRAIT_NAME
#undef NDNBOOST_TT_TRAIT_OP
#undef NDNBOOST_TT_FORBIDDEN_IF

#endif

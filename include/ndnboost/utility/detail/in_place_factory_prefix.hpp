// Copyright (C) 2003, Fernando Luis Cacciola Carballal.
// Copyright (C) 2007, Tobias Schwinger.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/optional for documentation.
//
// You are welcome to contact the author at:
//  fernando_cacciola@hotmail.com
//
#ifndef BOOST_UTILITY_DETAIL_INPLACE_FACTORY_PREFIX_04APR2007_HPP
#define BOOST_UTILITY_DETAIL_INPLACE_FACTORY_PREFIX_04APR2007_HPP

#include <new>
#include <cstddef>
#include <ndnboost/config.hpp>
#include <ndnboost/preprocessor/cat.hpp>
#include <ndnboost/preprocessor/punctuation/paren.hpp>
#include <ndnboost/preprocessor/iteration/iterate.hpp>
#include <ndnboost/preprocessor/repetition/repeat.hpp>
#include <ndnboost/preprocessor/repetition/enum.hpp>
#include <ndnboost/preprocessor/repetition/enum_params.hpp>
#include <ndnboost/preprocessor/repetition/enum_binary_params.hpp>
#include <ndnboost/preprocessor/repetition/enum_trailing_params.hpp>

#define BOOST_DEFINE_INPLACE_FACTORY_CLASS_MEMBER_INIT(z,n,_) BOOST_PP_CAT(m_a,n) BOOST_PP_LPAREN() BOOST_PP_CAT(a,n) BOOST_PP_RPAREN()
#define BOOST_DEFINE_INPLACE_FACTORY_CLASS_MEMBER_DECL(z,n,_) BOOST_PP_CAT(A,n) const& BOOST_PP_CAT(m_a,n);

#define BOOST_MAX_INPLACE_FACTORY_ARITY 10

#undef BOOST_UTILITY_DETAIL_INPLACE_FACTORY_SUFFIX_04APR2007_HPP

#endif


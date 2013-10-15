# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef NDNBOOST_PREPROCESSOR_DETAIL_IS_UNARY_HPP
# define NDNBOOST_PREPROCESSOR_DETAIL_IS_UNARY_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/detail/check.hpp>
#
# /* NDNBOOST_PP_IS_UNARY */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_IS_UNARY(x) NDNBOOST_PP_CHECK(x, NDNBOOST_PP_IS_UNARY_CHECK)
# else
#    define NDNBOOST_PP_IS_UNARY(x) NDNBOOST_PP_IS_UNARY_I(x)
#    define NDNBOOST_PP_IS_UNARY_I(x) NDNBOOST_PP_CHECK(x, NDNBOOST_PP_IS_UNARY_CHECK)
# endif
#
# define NDNBOOST_PP_IS_UNARY_CHECK(a) 1
# define NDNBOOST_PP_CHECK_RESULT_NDNBOOST_PP_IS_UNARY_CHECK 0, NDNBOOST_PP_NIL
#
# endif

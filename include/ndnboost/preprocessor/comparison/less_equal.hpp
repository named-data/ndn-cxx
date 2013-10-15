# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef NDNBOOST_PREPROCESSOR_COMPARISON_LESS_EQUAL_HPP
# define NDNBOOST_PREPROCESSOR_COMPARISON_LESS_EQUAL_HPP
#
# include <ndnboost/preprocessor/arithmetic/sub.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/logical/not.hpp>
#
# /* NDNBOOST_PP_LESS_EQUAL */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_LESS_EQUAL(x, y) NDNBOOST_PP_NOT(NDNBOOST_PP_SUB(x, y))
# else
#    define NDNBOOST_PP_LESS_EQUAL(x, y) NDNBOOST_PP_LESS_EQUAL_I(x, y)
#    define NDNBOOST_PP_LESS_EQUAL_I(x, y) NDNBOOST_PP_NOT(NDNBOOST_PP_SUB(x, y))
# endif
#
# /* NDNBOOST_PP_LESS_EQUAL_D */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_LESS_EQUAL_D(d, x, y) NDNBOOST_PP_NOT(NDNBOOST_PP_SUB_D(d, x, y))
# else
#    define NDNBOOST_PP_LESS_EQUAL_D(d, x, y) NDNBOOST_PP_LESS_EQUAL_D_I(d, x, y)
#    define NDNBOOST_PP_LESS_EQUAL_D_I(d, x, y) NDNBOOST_PP_NOT(NDNBOOST_PP_SUB_D(d, x, y))
# endif
#
# endif

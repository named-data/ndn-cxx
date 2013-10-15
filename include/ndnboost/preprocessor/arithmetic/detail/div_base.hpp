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
# ifndef NDNBOOST_PREPROCESSOR_ARITHMETIC_DETAIL_DIV_BASE_HPP
# define NDNBOOST_PREPROCESSOR_ARITHMETIC_DETAIL_DIV_BASE_HPP
#
# include <ndnboost/preprocessor/arithmetic/inc.hpp>
# include <ndnboost/preprocessor/arithmetic/sub.hpp>
# include <ndnboost/preprocessor/comparison/less_equal.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/control/while.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
# include <ndnboost/preprocessor/tuple/rem.hpp>
#
# /* NDNBOOST_PP_DIV_BASE */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_DIV_BASE(x, y) NDNBOOST_PP_WHILE(NDNBOOST_PP_DIV_BASE_P, NDNBOOST_PP_DIV_BASE_O, (0, x, y))
# else
#    define NDNBOOST_PP_DIV_BASE(x, y) NDNBOOST_PP_DIV_BASE_I(x, y)
#    define NDNBOOST_PP_DIV_BASE_I(x, y) NDNBOOST_PP_WHILE(NDNBOOST_PP_DIV_BASE_P, NDNBOOST_PP_DIV_BASE_O, (0, x, y))
# endif
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_STRICT()
#    define NDNBOOST_PP_DIV_BASE_P(d, rxy) NDNBOOST_PP_DIV_BASE_P_IM(d, NDNBOOST_PP_TUPLE_REM_3 rxy)
#    define NDNBOOST_PP_DIV_BASE_P_IM(d, im) NDNBOOST_PP_DIV_BASE_P_I(d, im)
# else
#    define NDNBOOST_PP_DIV_BASE_P(d, rxy) NDNBOOST_PP_DIV_BASE_P_I(d, NDNBOOST_PP_TUPLE_ELEM(3, 0, rxy), NDNBOOST_PP_TUPLE_ELEM(3, 1, rxy), NDNBOOST_PP_TUPLE_ELEM(3, 2, rxy))
# endif
#
# define NDNBOOST_PP_DIV_BASE_P_I(d, r, x, y) NDNBOOST_PP_LESS_EQUAL_D(d, y, x)
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_STRICT()
#    define NDNBOOST_PP_DIV_BASE_O(d, rxy) NDNBOOST_PP_DIV_BASE_O_IM(d, NDNBOOST_PP_TUPLE_REM_3 rxy)
#    define NDNBOOST_PP_DIV_BASE_O_IM(d, im) NDNBOOST_PP_DIV_BASE_O_I(d, im)
# else
#    define NDNBOOST_PP_DIV_BASE_O(d, rxy) NDNBOOST_PP_DIV_BASE_O_I(d, NDNBOOST_PP_TUPLE_ELEM(3, 0, rxy), NDNBOOST_PP_TUPLE_ELEM(3, 1, rxy), NDNBOOST_PP_TUPLE_ELEM(3, 2, rxy))
# endif
#
# define NDNBOOST_PP_DIV_BASE_O_I(d, r, x, y) (NDNBOOST_PP_INC(r), NDNBOOST_PP_SUB_D(d, x, y), y)
#
# /* NDNBOOST_PP_DIV_BASE_D */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_DIV_BASE_D(d, x, y) NDNBOOST_PP_WHILE_ ## d(NDNBOOST_PP_DIV_BASE_P, NDNBOOST_PP_DIV_BASE_O, (0, x, y))
# else
#    define NDNBOOST_PP_DIV_BASE_D(d, x, y) NDNBOOST_PP_DIV_BASE_D_I(d, x, y)
#    define NDNBOOST_PP_DIV_BASE_D_I(d, x, y) NDNBOOST_PP_WHILE_ ## d(NDNBOOST_PP_DIV_BASE_P, NDNBOOST_PP_DIV_BASE_O, (0, x, y))
# endif
#
# endif

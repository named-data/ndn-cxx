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
# ifndef NDNBOOST_PREPROCESSOR_ARITHMETIC_MOD_HPP
# define NDNBOOST_PREPROCESSOR_ARITHMETIC_MOD_HPP
#
# include <ndnboost/preprocessor/arithmetic/detail/div_base.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
#
# /* NDNBOOST_PP_MOD */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_MOD(x, y) NDNBOOST_PP_TUPLE_ELEM(3, 1, NDNBOOST_PP_DIV_BASE(x, y))
# else
#    define NDNBOOST_PP_MOD(x, y) NDNBOOST_PP_MOD_I(x, y)
#    define NDNBOOST_PP_MOD_I(x, y) NDNBOOST_PP_TUPLE_ELEM(3, 1, NDNBOOST_PP_DIV_BASE(x, y))
# endif
#
# /* NDNBOOST_PP_MOD_D */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_MOD_D(d, x, y) NDNBOOST_PP_TUPLE_ELEM(3, 1, NDNBOOST_PP_DIV_BASE_D(d, x, y))
# else
#    define NDNBOOST_PP_MOD_D(d, x, y) NDNBOOST_PP_MOD_D_I(d, x, y)
#    define NDNBOOST_PP_MOD_D_I(d, x, y) NDNBOOST_PP_TUPLE_ELEM(3, 1, NDNBOOST_PP_DIV_BASE_D(d, x, y))
# endif
#
# endif

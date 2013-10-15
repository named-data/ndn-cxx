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
# ifndef NDNBOOST_PREPROCESSOR_ARITHMETIC_ADD_HPP
# define NDNBOOST_PREPROCESSOR_ARITHMETIC_ADD_HPP
#
# include <ndnboost/preprocessor/arithmetic/dec.hpp>
# include <ndnboost/preprocessor/arithmetic/inc.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/control/while.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
#
# /* NDNBOOST_PP_ADD */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ADD(x, y) NDNBOOST_PP_TUPLE_ELEM(2, 0, NDNBOOST_PP_WHILE(NDNBOOST_PP_ADD_P, NDNBOOST_PP_ADD_O, (x, y)))
# else
#    define NDNBOOST_PP_ADD(x, y) NDNBOOST_PP_ADD_I(x, y)
#    define NDNBOOST_PP_ADD_I(x, y) NDNBOOST_PP_TUPLE_ELEM(2, 0, NDNBOOST_PP_WHILE(NDNBOOST_PP_ADD_P, NDNBOOST_PP_ADD_O, (x, y)))
# endif
#
# define NDNBOOST_PP_ADD_P(d, xy) NDNBOOST_PP_TUPLE_ELEM(2, 1, xy)
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_ADD_O(d, xy) NDNBOOST_PP_ADD_O_I xy
# else
#    define NDNBOOST_PP_ADD_O(d, xy) NDNBOOST_PP_ADD_O_I(NDNBOOST_PP_TUPLE_ELEM(2, 0, xy), NDNBOOST_PP_TUPLE_ELEM(2, 1, xy))
# endif
#
# define NDNBOOST_PP_ADD_O_I(x, y) (NDNBOOST_PP_INC(x), NDNBOOST_PP_DEC(y))
#
# /* NDNBOOST_PP_ADD_D */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ADD_D(d, x, y) NDNBOOST_PP_TUPLE_ELEM(2, 0, NDNBOOST_PP_WHILE_ ## d(NDNBOOST_PP_ADD_P, NDNBOOST_PP_ADD_O, (x, y)))
# else
#    define NDNBOOST_PP_ADD_D(d, x, y) NDNBOOST_PP_ADD_D_I(d, x, y)
#    define NDNBOOST_PP_ADD_D_I(d, x, y) NDNBOOST_PP_TUPLE_ELEM(2, 0, NDNBOOST_PP_WHILE_ ## d(NDNBOOST_PP_ADD_P, NDNBOOST_PP_ADD_O, (x, y)))
# endif
#
# endif

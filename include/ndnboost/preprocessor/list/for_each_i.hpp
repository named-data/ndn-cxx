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
# ifndef NDNBOOST_PREPROCESSOR_LIST_LIST_FOR_EACH_I_HPP
# define NDNBOOST_PREPROCESSOR_LIST_LIST_FOR_EACH_I_HPP
#
# include <ndnboost/preprocessor/arithmetic/inc.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/list/adt.hpp>
# include <ndnboost/preprocessor/repetition/for.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
# include <ndnboost/preprocessor/tuple/rem.hpp>
#
# /* NDNBOOST_PP_LIST_FOR_EACH_I */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG() && ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MSVC()
#    define NDNBOOST_PP_LIST_FOR_EACH_I(macro, data, list) NDNBOOST_PP_FOR((macro, data, list, 0), NDNBOOST_PP_LIST_FOR_EACH_I_P, NDNBOOST_PP_LIST_FOR_EACH_I_O, NDNBOOST_PP_LIST_FOR_EACH_I_M)
# else
#    define NDNBOOST_PP_LIST_FOR_EACH_I(macro, data, list) NDNBOOST_PP_LIST_FOR_EACH_I_I(macro, data, list)
#    define NDNBOOST_PP_LIST_FOR_EACH_I_I(macro, data, list) NDNBOOST_PP_FOR((macro, data, list, 0), NDNBOOST_PP_LIST_FOR_EACH_I_P, NDNBOOST_PP_LIST_FOR_EACH_I_O, NDNBOOST_PP_LIST_FOR_EACH_I_M)
# endif
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_STRICT()
#    define NDNBOOST_PP_LIST_FOR_EACH_I_P(r, x) NDNBOOST_PP_LIST_FOR_EACH_I_P_D x
#    define NDNBOOST_PP_LIST_FOR_EACH_I_P_D(m, d, l, i) NDNBOOST_PP_LIST_IS_CONS(l)
# else
#    define NDNBOOST_PP_LIST_FOR_EACH_I_P(r, x) NDNBOOST_PP_LIST_IS_CONS(NDNBOOST_PP_TUPLE_ELEM(4, 2, x))
# endif
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_LIST_FOR_EACH_I_O(r, x) NDNBOOST_PP_LIST_FOR_EACH_I_O_D x
#    define NDNBOOST_PP_LIST_FOR_EACH_I_O_D(m, d, l, i) (m, d, NDNBOOST_PP_LIST_REST(l), NDNBOOST_PP_INC(i))
# else
#    define NDNBOOST_PP_LIST_FOR_EACH_I_O(r, x) (NDNBOOST_PP_TUPLE_ELEM(4, 0, x), NDNBOOST_PP_TUPLE_ELEM(4, 1, x), NDNBOOST_PP_LIST_REST(NDNBOOST_PP_TUPLE_ELEM(4, 2, x)), NDNBOOST_PP_INC(NDNBOOST_PP_TUPLE_ELEM(4, 3, x)))
# endif
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_LIST_FOR_EACH_I_M(r, x) NDNBOOST_PP_LIST_FOR_EACH_I_M_D(r, NDNBOOST_PP_TUPLE_ELEM(4, 0, x), NDNBOOST_PP_TUPLE_ELEM(4, 1, x), NDNBOOST_PP_TUPLE_ELEM(4, 2, x), NDNBOOST_PP_TUPLE_ELEM(4, 3, x))
# else
#    define NDNBOOST_PP_LIST_FOR_EACH_I_M(r, x) NDNBOOST_PP_LIST_FOR_EACH_I_M_I(r, NDNBOOST_PP_TUPLE_REM_4 x)
#    define NDNBOOST_PP_LIST_FOR_EACH_I_M_I(r, x_e) NDNBOOST_PP_LIST_FOR_EACH_I_M_D(r, x_e)
# endif
#
# define NDNBOOST_PP_LIST_FOR_EACH_I_M_D(r, m, d, l, i) m(r, d, i, NDNBOOST_PP_LIST_FIRST(l))
#
# /* NDNBOOST_PP_LIST_FOR_EACH_I_R */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_LIST_FOR_EACH_I_R(r, macro, data, list) NDNBOOST_PP_FOR_ ## r((macro, data, list, 0), NDNBOOST_PP_LIST_FOR_EACH_I_P, NDNBOOST_PP_LIST_FOR_EACH_I_O, NDNBOOST_PP_LIST_FOR_EACH_I_M)
# else
#    define NDNBOOST_PP_LIST_FOR_EACH_I_R(r, macro, data, list) NDNBOOST_PP_LIST_FOR_EACH_I_R_I(r, macro, data, list)
#    define NDNBOOST_PP_LIST_FOR_EACH_I_R_I(r, macro, data, list) NDNBOOST_PP_FOR_ ## r((macro, data, list, 0), NDNBOOST_PP_LIST_FOR_EACH_I_P, NDNBOOST_PP_LIST_FOR_EACH_I_O, NDNBOOST_PP_LIST_FOR_EACH_I_M)
# endif
#
# endif

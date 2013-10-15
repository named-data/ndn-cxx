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
# ifndef NDNBOOST_PREPROCESSOR_REPETITION_REPEAT_FROM_TO_HPP
# define NDNBOOST_PREPROCESSOR_REPETITION_REPEAT_FROM_TO_HPP
#
# include <ndnboost/preprocessor/arithmetic/add.hpp>
# include <ndnboost/preprocessor/arithmetic/sub.hpp>
# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/control/while.hpp>
# include <ndnboost/preprocessor/debug/error.hpp>
# include <ndnboost/preprocessor/detail/auto_rec.hpp>
# include <ndnboost/preprocessor/repetition/repeat.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
# include <ndnboost/preprocessor/tuple/rem.hpp>
#
# /* NDNBOOST_PP_REPEAT_FROM_TO */
#
# if 0
#    define NDNBOOST_PP_REPEAT_FROM_TO(first, last, macro, data)
# endif
#
# define NDNBOOST_PP_REPEAT_FROM_TO NDNBOOST_PP_CAT(NDNBOOST_PP_REPEAT_FROM_TO_, NDNBOOST_PP_AUTO_REC(NDNBOOST_PP_REPEAT_P, 4))
#
# define NDNBOOST_PP_REPEAT_FROM_TO_1(f, l, m, dt) NDNBOOST_PP_REPEAT_FROM_TO_D_1(NDNBOOST_PP_AUTO_REC(NDNBOOST_PP_WHILE_P, 256), f, l, m, dt)
# define NDNBOOST_PP_REPEAT_FROM_TO_2(f, l, m, dt) NDNBOOST_PP_REPEAT_FROM_TO_D_2(NDNBOOST_PP_AUTO_REC(NDNBOOST_PP_WHILE_P, 256), f, l, m, dt)
# define NDNBOOST_PP_REPEAT_FROM_TO_3(f, l, m, dt) NDNBOOST_PP_REPEAT_FROM_TO_D_3(NDNBOOST_PP_AUTO_REC(NDNBOOST_PP_WHILE_P, 256), f, l, m, dt)
# define NDNBOOST_PP_REPEAT_FROM_TO_4(f, l, m, dt) NDNBOOST_PP_ERROR(0x0003)
#
# define NDNBOOST_PP_REPEAT_FROM_TO_1ST NDNBOOST_PP_REPEAT_FROM_TO_1
# define NDNBOOST_PP_REPEAT_FROM_TO_2ND NDNBOOST_PP_REPEAT_FROM_TO_2
# define NDNBOOST_PP_REPEAT_FROM_TO_3RD NDNBOOST_PP_REPEAT_FROM_TO_3
#
# /* NDNBOOST_PP_REPEAT_FROM_TO_D */
#
# if 0
#    define NDNBOOST_PP_REPEAT_FROM_TO_D(d, first, last, macro, data)
# endif
#
# define NDNBOOST_PP_REPEAT_FROM_TO_D NDNBOOST_PP_CAT(NDNBOOST_PP_REPEAT_FROM_TO_D_, NDNBOOST_PP_AUTO_REC(NDNBOOST_PP_REPEAT_P, 4))
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_REPEAT_FROM_TO_D_1(d, f, l, m, dt) NDNBOOST_PP_REPEAT_1(NDNBOOST_PP_SUB_D(d, l, f), NDNBOOST_PP_REPEAT_FROM_TO_M_1, (d, f, m, dt))
#    define NDNBOOST_PP_REPEAT_FROM_TO_D_2(d, f, l, m, dt) NDNBOOST_PP_REPEAT_2(NDNBOOST_PP_SUB_D(d, l, f), NDNBOOST_PP_REPEAT_FROM_TO_M_2, (d, f, m, dt))
#    define NDNBOOST_PP_REPEAT_FROM_TO_D_3(d, f, l, m, dt) NDNBOOST_PP_REPEAT_3(NDNBOOST_PP_SUB_D(d, l, f), NDNBOOST_PP_REPEAT_FROM_TO_M_3, (d, f, m, dt))
# else
#    define NDNBOOST_PP_REPEAT_FROM_TO_D_1(d, f, l, m, dt) NDNBOOST_PP_REPEAT_FROM_TO_D_1_I(d, f, l, m, dt)
#    define NDNBOOST_PP_REPEAT_FROM_TO_D_2(d, f, l, m, dt) NDNBOOST_PP_REPEAT_FROM_TO_D_2_I(d, f, l, m, dt)
#    define NDNBOOST_PP_REPEAT_FROM_TO_D_3(d, f, l, m, dt) NDNBOOST_PP_REPEAT_FROM_TO_D_3_I(d, f, l, m, dt)
#    define NDNBOOST_PP_REPEAT_FROM_TO_D_1_I(d, f, l, m, dt) NDNBOOST_PP_REPEAT_1(NDNBOOST_PP_SUB_D(d, l, f), NDNBOOST_PP_REPEAT_FROM_TO_M_1, (d, f, m, dt))
#    define NDNBOOST_PP_REPEAT_FROM_TO_D_2_I(d, f, l, m, dt) NDNBOOST_PP_REPEAT_2(NDNBOOST_PP_SUB_D(d, l, f), NDNBOOST_PP_REPEAT_FROM_TO_M_2, (d, f, m, dt))
#    define NDNBOOST_PP_REPEAT_FROM_TO_D_3_I(d, f, l, m, dt) NDNBOOST_PP_REPEAT_3(NDNBOOST_PP_SUB_D(d, l, f), NDNBOOST_PP_REPEAT_FROM_TO_M_3, (d, f, m, dt))
# endif
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_STRICT()
#    define NDNBOOST_PP_REPEAT_FROM_TO_M_1(z, n, dfmd) NDNBOOST_PP_REPEAT_FROM_TO_M_1_IM(z, n, NDNBOOST_PP_TUPLE_REM_4 dfmd)
#    define NDNBOOST_PP_REPEAT_FROM_TO_M_2(z, n, dfmd) NDNBOOST_PP_REPEAT_FROM_TO_M_2_IM(z, n, NDNBOOST_PP_TUPLE_REM_4 dfmd)
#    define NDNBOOST_PP_REPEAT_FROM_TO_M_3(z, n, dfmd) NDNBOOST_PP_REPEAT_FROM_TO_M_3_IM(z, n, NDNBOOST_PP_TUPLE_REM_4 dfmd)
#    define NDNBOOST_PP_REPEAT_FROM_TO_M_1_IM(z, n, im) NDNBOOST_PP_REPEAT_FROM_TO_M_1_I(z, n, im)
#    define NDNBOOST_PP_REPEAT_FROM_TO_M_2_IM(z, n, im) NDNBOOST_PP_REPEAT_FROM_TO_M_2_I(z, n, im)
#    define NDNBOOST_PP_REPEAT_FROM_TO_M_3_IM(z, n, im) NDNBOOST_PP_REPEAT_FROM_TO_M_3_I(z, n, im)
# else
#    define NDNBOOST_PP_REPEAT_FROM_TO_M_1(z, n, dfmd) NDNBOOST_PP_REPEAT_FROM_TO_M_1_I(z, n, NDNBOOST_PP_TUPLE_ELEM(4, 0, dfmd), NDNBOOST_PP_TUPLE_ELEM(4, 1, dfmd), NDNBOOST_PP_TUPLE_ELEM(4, 2, dfmd), NDNBOOST_PP_TUPLE_ELEM(4, 3, dfmd))
#    define NDNBOOST_PP_REPEAT_FROM_TO_M_2(z, n, dfmd) NDNBOOST_PP_REPEAT_FROM_TO_M_2_I(z, n, NDNBOOST_PP_TUPLE_ELEM(4, 0, dfmd), NDNBOOST_PP_TUPLE_ELEM(4, 1, dfmd), NDNBOOST_PP_TUPLE_ELEM(4, 2, dfmd), NDNBOOST_PP_TUPLE_ELEM(4, 3, dfmd))
#    define NDNBOOST_PP_REPEAT_FROM_TO_M_3(z, n, dfmd) NDNBOOST_PP_REPEAT_FROM_TO_M_3_I(z, n, NDNBOOST_PP_TUPLE_ELEM(4, 0, dfmd), NDNBOOST_PP_TUPLE_ELEM(4, 1, dfmd), NDNBOOST_PP_TUPLE_ELEM(4, 2, dfmd), NDNBOOST_PP_TUPLE_ELEM(4, 3, dfmd))
# endif
#
# define NDNBOOST_PP_REPEAT_FROM_TO_M_1_I(z, n, d, f, m, dt) NDNBOOST_PP_REPEAT_FROM_TO_M_1_II(z, NDNBOOST_PP_ADD_D(d, n, f), m, dt)
# define NDNBOOST_PP_REPEAT_FROM_TO_M_2_I(z, n, d, f, m, dt) NDNBOOST_PP_REPEAT_FROM_TO_M_2_II(z, NDNBOOST_PP_ADD_D(d, n, f), m, dt)
# define NDNBOOST_PP_REPEAT_FROM_TO_M_3_I(z, n, d, f, m, dt) NDNBOOST_PP_REPEAT_FROM_TO_M_3_II(z, NDNBOOST_PP_ADD_D(d, n, f), m, dt)
#
# define NDNBOOST_PP_REPEAT_FROM_TO_M_1_II(z, n, m, dt) m(z, n, dt)
# define NDNBOOST_PP_REPEAT_FROM_TO_M_2_II(z, n, m, dt) m(z, n, dt)
# define NDNBOOST_PP_REPEAT_FROM_TO_M_3_II(z, n, m, dt) m(z, n, dt)
#
# endif

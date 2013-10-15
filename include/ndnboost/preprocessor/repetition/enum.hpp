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
# ifndef NDNBOOST_PREPROCESSOR_REPETITION_ENUM_HPP
# define NDNBOOST_PREPROCESSOR_REPETITION_ENUM_HPP
#
# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/debug/error.hpp>
# include <ndnboost/preprocessor/detail/auto_rec.hpp>
# include <ndnboost/preprocessor/punctuation/comma_if.hpp>
# include <ndnboost/preprocessor/repetition/repeat.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
# include <ndnboost/preprocessor/tuple/rem.hpp>
#
# /* NDNBOOST_PP_ENUM */
#
# if 0
#    define NDNBOOST_PP_ENUM(count, macro, data)
# endif
#
# define NDNBOOST_PP_ENUM NDNBOOST_PP_CAT(NDNBOOST_PP_ENUM_, NDNBOOST_PP_AUTO_REC(NDNBOOST_PP_REPEAT_P, 4))
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ENUM_1(c, m, d) NDNBOOST_PP_REPEAT_1(c, NDNBOOST_PP_ENUM_M_1, (m, d))
#    define NDNBOOST_PP_ENUM_2(c, m, d) NDNBOOST_PP_REPEAT_2(c, NDNBOOST_PP_ENUM_M_2, (m, d))
#    define NDNBOOST_PP_ENUM_3(c, m, d) NDNBOOST_PP_REPEAT_3(c, NDNBOOST_PP_ENUM_M_3, (m, d))
# else
#    define NDNBOOST_PP_ENUM_1(c, m, d) NDNBOOST_PP_ENUM_1_I(c, m, d)
#    define NDNBOOST_PP_ENUM_2(c, m, d) NDNBOOST_PP_ENUM_2_I(c, m, d)
#    define NDNBOOST_PP_ENUM_3(c, m, d) NDNBOOST_PP_ENUM_3_I(c, m, d)
#    define NDNBOOST_PP_ENUM_1_I(c, m, d) NDNBOOST_PP_REPEAT_1(c, NDNBOOST_PP_ENUM_M_1, (m, d))
#    define NDNBOOST_PP_ENUM_2_I(c, m, d) NDNBOOST_PP_REPEAT_2(c, NDNBOOST_PP_ENUM_M_2, (m, d))
#    define NDNBOOST_PP_ENUM_3_I(c, m, d) NDNBOOST_PP_REPEAT_3(c, NDNBOOST_PP_ENUM_M_3, (m, d))
# endif
#
# define NDNBOOST_PP_ENUM_4(c, m, d) NDNBOOST_PP_ERROR(0x0003)
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_STRICT()
#    define NDNBOOST_PP_ENUM_M_1(z, n, md) NDNBOOST_PP_ENUM_M_1_IM(z, n, NDNBOOST_PP_TUPLE_REM_2 md)
#    define NDNBOOST_PP_ENUM_M_2(z, n, md) NDNBOOST_PP_ENUM_M_2_IM(z, n, NDNBOOST_PP_TUPLE_REM_2 md)
#    define NDNBOOST_PP_ENUM_M_3(z, n, md) NDNBOOST_PP_ENUM_M_3_IM(z, n, NDNBOOST_PP_TUPLE_REM_2 md)
#    define NDNBOOST_PP_ENUM_M_1_IM(z, n, im) NDNBOOST_PP_ENUM_M_1_I(z, n, im)
#    define NDNBOOST_PP_ENUM_M_2_IM(z, n, im) NDNBOOST_PP_ENUM_M_2_I(z, n, im)
#    define NDNBOOST_PP_ENUM_M_3_IM(z, n, im) NDNBOOST_PP_ENUM_M_3_I(z, n, im)
# else
#    define NDNBOOST_PP_ENUM_M_1(z, n, md) NDNBOOST_PP_ENUM_M_1_I(z, n, NDNBOOST_PP_TUPLE_ELEM(2, 0, md), NDNBOOST_PP_TUPLE_ELEM(2, 1, md))
#    define NDNBOOST_PP_ENUM_M_2(z, n, md) NDNBOOST_PP_ENUM_M_2_I(z, n, NDNBOOST_PP_TUPLE_ELEM(2, 0, md), NDNBOOST_PP_TUPLE_ELEM(2, 1, md))
#    define NDNBOOST_PP_ENUM_M_3(z, n, md) NDNBOOST_PP_ENUM_M_3_I(z, n, NDNBOOST_PP_TUPLE_ELEM(2, 0, md), NDNBOOST_PP_TUPLE_ELEM(2, 1, md))
# endif
#
# define NDNBOOST_PP_ENUM_M_1_I(z, n, m, d) NDNBOOST_PP_COMMA_IF(n) m(z, n, d)
# define NDNBOOST_PP_ENUM_M_2_I(z, n, m, d) NDNBOOST_PP_COMMA_IF(n) m(z, n, d)
# define NDNBOOST_PP_ENUM_M_3_I(z, n, m, d) NDNBOOST_PP_COMMA_IF(n) m(z, n, d)
#
# endif

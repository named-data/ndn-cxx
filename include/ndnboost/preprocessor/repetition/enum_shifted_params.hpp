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
# ifndef NDNBOOST_PREPROCESSOR_REPETITION_ENUM_SHIFTED_PARAMS_HPP
# define NDNBOOST_PREPROCESSOR_REPETITION_ENUM_SHIFTED_PARAMS_HPP
#
# include <ndnboost/preprocessor/arithmetic/dec.hpp>
# include <ndnboost/preprocessor/arithmetic/inc.hpp>
# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/punctuation/comma_if.hpp>
# include <ndnboost/preprocessor/repetition/repeat.hpp>
#
# /* NDNBOOST_PP_ENUM_SHIFTED_PARAMS */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ENUM_SHIFTED_PARAMS(count, param) NDNBOOST_PP_REPEAT(NDNBOOST_PP_DEC(count), NDNBOOST_PP_ENUM_SHIFTED_PARAMS_M, param)
# else
#    define NDNBOOST_PP_ENUM_SHIFTED_PARAMS(count, param) NDNBOOST_PP_ENUM_SHIFTED_PARAMS_I(count, param)
#    define NDNBOOST_PP_ENUM_SHIFTED_PARAMS_I(count, param) NDNBOOST_PP_REPEAT(NDNBOOST_PP_DEC(count), NDNBOOST_PP_ENUM_SHIFTED_PARAMS_M, param)
# endif
#
# define NDNBOOST_PP_ENUM_SHIFTED_PARAMS_M(z, n, param) NDNBOOST_PP_COMMA_IF(n) NDNBOOST_PP_CAT(param, NDNBOOST_PP_INC(n))
#
# /* NDNBOOST_PP_ENUM_SHIFTED_PARAMS_Z */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ENUM_SHIFTED_PARAMS_Z(z, count, param) NDNBOOST_PP_REPEAT_ ## z(NDNBOOST_PP_DEC(count), NDNBOOST_PP_ENUM_SHIFTED_PARAMS_M, param)
# else
#    define NDNBOOST_PP_ENUM_SHIFTED_PARAMS_Z(z, count, param) NDNBOOST_PP_ENUM_SHIFTED_PARAMS_Z_I(z, count, param)
#    define NDNBOOST_PP_ENUM_SHIFTED_PARAMS_Z_I(z, count, param) NDNBOOST_PP_REPEAT_ ## z(NDNBOOST_PP_DEC(count), NDNBOOST_PP_ENUM_SHIFTED_PARAMS_M, param)
# endif
#
# endif

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
# ifndef NDNBOOST_PREPROCESSOR_REPETITION_ENUM_TRAILING_PARAMS_HPP
# define NDNBOOST_PREPROCESSOR_REPETITION_ENUM_TRAILING_PARAMS_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/repetition/repeat.hpp>
#
# /* NDNBOOST_PP_ENUM_TRAILING_PARAMS */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ENUM_TRAILING_PARAMS(count, param) NDNBOOST_PP_REPEAT(count, NDNBOOST_PP_ENUM_TRAILING_PARAMS_M, param)
# else
#    define NDNBOOST_PP_ENUM_TRAILING_PARAMS(count, param) NDNBOOST_PP_ENUM_TRAILING_PARAMS_I(count, param)
#    define NDNBOOST_PP_ENUM_TRAILING_PARAMS_I(count, param) NDNBOOST_PP_REPEAT(count, NDNBOOST_PP_ENUM_TRAILING_PARAMS_M, param)
# endif
#
# define NDNBOOST_PP_ENUM_TRAILING_PARAMS_M(z, n, param) , param ## n
#
# /* NDNBOOST_PP_ENUM_TRAILING_PARAMS_Z */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ENUM_TRAILING_PARAMS_Z(z, count, param) NDNBOOST_PP_REPEAT_ ## z(count, NDNBOOST_PP_ENUM_TRAILING_PARAMS_M, param)
# else
#    define NDNBOOST_PP_ENUM_TRAILING_PARAMS_Z(z, count, param) NDNBOOST_PP_ENUM_TRAILING_PARAMS_Z_I(z, count, param)
#    define NDNBOOST_PP_ENUM_TRAILING_PARAMS_Z_I(z, count, param) NDNBOOST_PP_REPEAT_ ## z(count, NDNBOOST_PP_ENUM_TRAILING_PARAMS_M, param)
# endif
#
# endif

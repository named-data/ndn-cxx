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
# ifndef NDNBOOST_PREPROCESSOR_REPETITION_ENUM_PARAMS_WITH_A_DEFAULT_HPP
# define NDNBOOST_PREPROCESSOR_REPETITION_ENUM_PARAMS_WITH_A_DEFAULT_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/facilities/intercept.hpp>
# include <ndnboost/preprocessor/repetition/enum_binary_params.hpp>
#
# /* NDNBOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT */
#
# define NDNBOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(count, param, def) NDNBOOST_PP_ENUM_BINARY_PARAMS(count, param, = def NDNBOOST_PP_INTERCEPT)
#
# endif

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
# ifndef NDNBOOST_PREPROCESSOR_CONTROL_EXPR_IF_HPP
# define NDNBOOST_PREPROCESSOR_CONTROL_EXPR_IF_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/control/expr_iif.hpp>
# include <ndnboost/preprocessor/logical/bool.hpp>
#
# /* NDNBOOST_PP_EXPR_IF */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_EXPR_IF(cond, expr) NDNBOOST_PP_EXPR_IIF(NDNBOOST_PP_BOOL(cond), expr)
# else
#    define NDNBOOST_PP_EXPR_IF(cond, expr) NDNBOOST_PP_EXPR_IF_I(cond, expr)
#    define NDNBOOST_PP_EXPR_IF_I(cond, expr) NDNBOOST_PP_EXPR_IIF(NDNBOOST_PP_BOOL(cond), expr)
# endif
#
# endif

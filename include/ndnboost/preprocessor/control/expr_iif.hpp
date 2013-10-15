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
# ifndef NDNBOOST_PREPROCESSOR_CONTROL_EXPR_IIF_HPP
# define NDNBOOST_PREPROCESSOR_CONTROL_EXPR_IIF_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
#
# /* NDNBOOST_PP_EXPR_IIF */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_EXPR_IIF(bit, expr) NDNBOOST_PP_EXPR_IIF_I(bit, expr)
# else
#    define NDNBOOST_PP_EXPR_IIF(bit, expr) NDNBOOST_PP_EXPR_IIF_OO((bit, expr))
#    define NDNBOOST_PP_EXPR_IIF_OO(par) NDNBOOST_PP_EXPR_IIF_I ## par
# endif
#
# define NDNBOOST_PP_EXPR_IIF_I(bit, expr) NDNBOOST_PP_EXPR_IIF_ ## bit(expr)
#
# define NDNBOOST_PP_EXPR_IIF_0(expr)
# define NDNBOOST_PP_EXPR_IIF_1(expr) expr
#
# endif

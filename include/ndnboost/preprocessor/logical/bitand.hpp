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
# ifndef NDNBOOST_PREPROCESSOR_LOGICAL_BITAND_HPP
# define NDNBOOST_PREPROCESSOR_LOGICAL_BITAND_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
#
# /* NDNBOOST_PP_BITAND */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_BITAND(x, y) NDNBOOST_PP_BITAND_I(x, y)
# else
#    define NDNBOOST_PP_BITAND(x, y) NDNBOOST_PP_BITAND_OO((x, y))
#    define NDNBOOST_PP_BITAND_OO(par) NDNBOOST_PP_BITAND_I ## par
# endif
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MSVC()
#    define NDNBOOST_PP_BITAND_I(x, y) NDNBOOST_PP_BITAND_ ## x ## y
# else
#    define NDNBOOST_PP_BITAND_I(x, y) NDNBOOST_PP_BITAND_ID(NDNBOOST_PP_BITAND_ ## x ## y)
#    define NDNBOOST_PP_BITAND_ID(res) res
# endif
#
# define NDNBOOST_PP_BITAND_00 0
# define NDNBOOST_PP_BITAND_01 0
# define NDNBOOST_PP_BITAND_10 0
# define NDNBOOST_PP_BITAND_11 1
#
# endif

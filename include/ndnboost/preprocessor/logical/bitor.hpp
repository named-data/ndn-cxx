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
# ifndef NDNBOOST_PREPROCESSOR_LOGICAL_BITOR_HPP
# define NDNBOOST_PREPROCESSOR_LOGICAL_BITOR_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
#
# /* NDNBOOST_PP_BITOR */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_BITOR(x, y) NDNBOOST_PP_BITOR_I(x, y)
# else
#    define NDNBOOST_PP_BITOR(x, y) NDNBOOST_PP_BITOR_OO((x, y))
#    define NDNBOOST_PP_BITOR_OO(par) NDNBOOST_PP_BITOR_I ## par
# endif
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MSVC()
#    define NDNBOOST_PP_BITOR_I(x, y) NDNBOOST_PP_BITOR_ ## x ## y
# else
#    define NDNBOOST_PP_BITOR_I(x, y) NDNBOOST_PP_BITOR_ID(NDNBOOST_PP_BITOR_ ## x ## y)
#    define NDNBOOST_PP_BITOR_ID(id) id
# endif
#
# define NDNBOOST_PP_BITOR_00 0
# define NDNBOOST_PP_BITOR_01 1
# define NDNBOOST_PP_BITOR_10 1
# define NDNBOOST_PP_BITOR_11 1
#
# endif

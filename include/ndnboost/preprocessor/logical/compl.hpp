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
# ifndef NDNBOOST_PREPROCESSOR_LOGICAL_COMPL_HPP
# define NDNBOOST_PREPROCESSOR_LOGICAL_COMPL_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
#
# /* NDNBOOST_PP_COMPL */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_COMPL(x) NDNBOOST_PP_COMPL_I(x)
# else
#    define NDNBOOST_PP_COMPL(x) NDNBOOST_PP_COMPL_OO((x))
#    define NDNBOOST_PP_COMPL_OO(par) NDNBOOST_PP_COMPL_I ## par
# endif
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MSVC()
#    define NDNBOOST_PP_COMPL_I(x) NDNBOOST_PP_COMPL_ ## x
# else
#    define NDNBOOST_PP_COMPL_I(x) NDNBOOST_PP_COMPL_ID(NDNBOOST_PP_COMPL_ ## x)
#    define NDNBOOST_PP_COMPL_ID(id) id
# endif
#
# define NDNBOOST_PP_COMPL_0 1
# define NDNBOOST_PP_COMPL_1 0
#
# endif

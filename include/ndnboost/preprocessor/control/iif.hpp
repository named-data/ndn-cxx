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
# ifndef NDNBOOST_PREPROCESSOR_CONTROL_IIF_HPP
# define NDNBOOST_PREPROCESSOR_CONTROL_IIF_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_IIF(bit, t, f) NDNBOOST_PP_IIF_I(bit, t, f)
# else
#    define NDNBOOST_PP_IIF(bit, t, f) NDNBOOST_PP_IIF_OO((bit, t, f))
#    define NDNBOOST_PP_IIF_OO(par) NDNBOOST_PP_IIF_I ## par
# endif
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MSVC()
#    define NDNBOOST_PP_IIF_I(bit, t, f) NDNBOOST_PP_IIF_ ## bit(t, f)
# else
#    define NDNBOOST_PP_IIF_I(bit, t, f) NDNBOOST_PP_IIF_II(NDNBOOST_PP_IIF_ ## bit(t, f))
#    define NDNBOOST_PP_IIF_II(id) id
# endif
#
# define NDNBOOST_PP_IIF_0(t, f) f
# define NDNBOOST_PP_IIF_1(t, f) t
#
# endif

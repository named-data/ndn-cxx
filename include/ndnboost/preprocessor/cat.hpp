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
# ifndef NDNBOOST_PREPROCESSOR_CAT_HPP
# define NDNBOOST_PREPROCESSOR_CAT_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
#
# /* NDNBOOST_PP_CAT */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_CAT(a, b) NDNBOOST_PP_CAT_I(a, b)
# else
#    define NDNBOOST_PP_CAT(a, b) NDNBOOST_PP_CAT_OO((a, b))
#    define NDNBOOST_PP_CAT_OO(par) NDNBOOST_PP_CAT_I ## par
# endif
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MSVC()
#    define NDNBOOST_PP_CAT_I(a, b) a ## b
# else
#    define NDNBOOST_PP_CAT_I(a, b) NDNBOOST_PP_CAT_II(~, a ## b)
#    define NDNBOOST_PP_CAT_II(p, res) res
# endif
#
# endif

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
# ifndef NDNBOOST_PREPROCESSOR_LIST_APPEND_HPP
# define NDNBOOST_PREPROCESSOR_LIST_APPEND_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/list/fold_right.hpp>
#
# /* NDNBOOST_PP_LIST_APPEND */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_LIST_APPEND(a, b) NDNBOOST_PP_LIST_FOLD_RIGHT(NDNBOOST_PP_LIST_APPEND_O, b, a)
# else
#    define NDNBOOST_PP_LIST_APPEND(a, b) NDNBOOST_PP_LIST_APPEND_I(a, b)
#    define NDNBOOST_PP_LIST_APPEND_I(a, b) NDNBOOST_PP_LIST_FOLD_RIGHT(NDNBOOST_PP_LIST_APPEND_O, b, a)
# endif
#
# define NDNBOOST_PP_LIST_APPEND_O(d, s, x) (x, s)
#
# /* NDNBOOST_PP_LIST_APPEND_D */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_LIST_APPEND_D(d, a, b) NDNBOOST_PP_LIST_FOLD_RIGHT_ ## d(NDNBOOST_PP_LIST_APPEND_O, b, a)
# else
#    define NDNBOOST_PP_LIST_APPEND_D(d, a, b) NDNBOOST_PP_LIST_APPEND_D_I(d, a, b)
#    define NDNBOOST_PP_LIST_APPEND_D_I(d, a, b) NDNBOOST_PP_LIST_FOLD_RIGHT_ ## d(NDNBOOST_PP_LIST_APPEND_O, b, a)
# endif
#
# endif

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
# ifndef NDNBOOST_PREPROCESSOR_LIST_REVERSE_HPP
# define NDNBOOST_PREPROCESSOR_LIST_REVERSE_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/list/fold_left.hpp>
#
# /* NDNBOOST_PP_LIST_REVERSE */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_LIST_REVERSE(list) NDNBOOST_PP_LIST_FOLD_LEFT(NDNBOOST_PP_LIST_REVERSE_O, NDNBOOST_PP_NIL, list)
# else
#    define NDNBOOST_PP_LIST_REVERSE(list) NDNBOOST_PP_LIST_REVERSE_I(list)
#    define NDNBOOST_PP_LIST_REVERSE_I(list) NDNBOOST_PP_LIST_FOLD_LEFT(NDNBOOST_PP_LIST_REVERSE_O, NDNBOOST_PP_NIL, list)
# endif
#
# define NDNBOOST_PP_LIST_REVERSE_O(d, s, x) (x, s)
#
# /* NDNBOOST_PP_LIST_REVERSE_D */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_LIST_REVERSE_D(d, list) NDNBOOST_PP_LIST_FOLD_LEFT_ ## d(NDNBOOST_PP_LIST_REVERSE_O, NDNBOOST_PP_NIL, list)
# else
#    define NDNBOOST_PP_LIST_REVERSE_D(d, list) NDNBOOST_PP_LIST_REVERSE_D_I(d, list)
#    define NDNBOOST_PP_LIST_REVERSE_D_I(d, list) NDNBOOST_PP_LIST_FOLD_LEFT_ ## d(NDNBOOST_PP_LIST_REVERSE_O, NDNBOOST_PP_NIL, list)
# endif
#
# endif

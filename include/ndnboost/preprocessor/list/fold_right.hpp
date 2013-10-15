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
# ifndef NDNBOOST_PREPROCESSOR_LIST_FOLD_RIGHT_HPP
# define NDNBOOST_PREPROCESSOR_LIST_FOLD_RIGHT_HPP
#
# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/preprocessor/control/while.hpp>
# include <ndnboost/preprocessor/debug/error.hpp>
# include <ndnboost/preprocessor/detail/auto_rec.hpp>
#
# if 0
#    define NDNBOOST_PP_LIST_FOLD_RIGHT(op, state, list)
# endif
#
# define NDNBOOST_PP_LIST_FOLD_RIGHT NDNBOOST_PP_CAT(NDNBOOST_PP_LIST_FOLD_RIGHT_, NDNBOOST_PP_AUTO_REC(NDNBOOST_PP_WHILE_P, 256))
#
# define NDNBOOST_PP_LIST_FOLD_RIGHT_257(o, s, l) NDNBOOST_PP_ERROR(0x0004)
#
# define NDNBOOST_PP_LIST_FOLD_RIGHT_D(d, o, s, l) NDNBOOST_PP_LIST_FOLD_RIGHT_ ## d(o, s, l)
# define NDNBOOST_PP_LIST_FOLD_RIGHT_2ND NDNBOOST_PP_LIST_FOLD_RIGHT
# define NDNBOOST_PP_LIST_FOLD_RIGHT_2ND_D NDNBOOST_PP_LIST_FOLD_RIGHT_D
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    include <ndnboost/preprocessor/list/detail/edg/fold_right.hpp>
# else
#    include <ndnboost/preprocessor/list/detail/fold_right.hpp>
# endif
#
# endif

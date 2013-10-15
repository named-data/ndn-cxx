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
# ifndef NDNBOOST_PREPROCESSOR_LIST_TRANSFORM_HPP
# define NDNBOOST_PREPROCESSOR_LIST_TRANSFORM_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/list/fold_right.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
# include <ndnboost/preprocessor/tuple/rem.hpp>
#
# /* NDNBOOST_PP_LIST_TRANSFORM */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_LIST_TRANSFORM(op, data, list) NDNBOOST_PP_TUPLE_ELEM(3, 2, NDNBOOST_PP_LIST_FOLD_RIGHT(NDNBOOST_PP_LIST_TRANSFORM_O, (op, data, NDNBOOST_PP_NIL), list))
# else
#    define NDNBOOST_PP_LIST_TRANSFORM(op, data, list) NDNBOOST_PP_LIST_TRANSFORM_I(op, data, list)
#    define NDNBOOST_PP_LIST_TRANSFORM_I(op, data, list) NDNBOOST_PP_TUPLE_ELEM(3, 2, NDNBOOST_PP_LIST_FOLD_RIGHT(NDNBOOST_PP_LIST_TRANSFORM_O, (op, data, NDNBOOST_PP_NIL), list))
# endif
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_LIST_TRANSFORM_O(d, odr, elem) NDNBOOST_PP_LIST_TRANSFORM_O_D(d, NDNBOOST_PP_TUPLE_ELEM(3, 0, odr), NDNBOOST_PP_TUPLE_ELEM(3, 1, odr), NDNBOOST_PP_TUPLE_ELEM(3, 2, odr), elem)
# else
#    define NDNBOOST_PP_LIST_TRANSFORM_O(d, odr, elem) NDNBOOST_PP_LIST_TRANSFORM_O_I(d, NDNBOOST_PP_TUPLE_REM_3 odr, elem)
#    define NDNBOOST_PP_LIST_TRANSFORM_O_I(d, im, elem) NDNBOOST_PP_LIST_TRANSFORM_O_D(d, im, elem)
# endif
#
# define NDNBOOST_PP_LIST_TRANSFORM_O_D(d, op, data, res, elem) (op, data, (op(d, data, elem), res))
#
# /* NDNBOOST_PP_LIST_TRANSFORM_D */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_LIST_TRANSFORM_D(d, op, data, list) NDNBOOST_PP_TUPLE_ELEM(3, 2, NDNBOOST_PP_LIST_FOLD_RIGHT_ ## d(NDNBOOST_PP_LIST_TRANSFORM_O, (op, data, NDNBOOST_PP_NIL), list))
# else
#    define NDNBOOST_PP_LIST_TRANSFORM_D(d, op, data, list) NDNBOOST_PP_LIST_TRANSFORM_D_I(d, op, data, list)
#    define NDNBOOST_PP_LIST_TRANSFORM_D_I(d, op, data, list) NDNBOOST_PP_TUPLE_ELEM(3, 2, NDNBOOST_PP_LIST_FOLD_RIGHT_ ## d(NDNBOOST_PP_LIST_TRANSFORM_O, (op, data, NDNBOOST_PP_NIL), list))
# endif
#
# endif

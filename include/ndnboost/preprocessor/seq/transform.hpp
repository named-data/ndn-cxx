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
# ifndef NDNBOOST_PREPROCESSOR_SEQ_TRANSFORM_HPP
# define NDNBOOST_PREPROCESSOR_SEQ_TRANSFORM_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/seq/fold_left.hpp>
# include <ndnboost/preprocessor/seq/seq.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
# include <ndnboost/preprocessor/tuple/rem.hpp>
#
# /* NDNBOOST_PP_SEQ_TRANSFORM */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_SEQ_TRANSFORM(op, data, seq) NDNBOOST_PP_SEQ_TAIL(NDNBOOST_PP_TUPLE_ELEM(3, 2, NDNBOOST_PP_SEQ_FOLD_LEFT(NDNBOOST_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# else
#    define NDNBOOST_PP_SEQ_TRANSFORM(op, data, seq) NDNBOOST_PP_SEQ_TRANSFORM_I(op, data, seq)
#    define NDNBOOST_PP_SEQ_TRANSFORM_I(op, data, seq) NDNBOOST_PP_SEQ_TAIL(NDNBOOST_PP_TUPLE_ELEM(3, 2, NDNBOOST_PP_SEQ_FOLD_LEFT(NDNBOOST_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# endif
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_STRICT()
#    define NDNBOOST_PP_SEQ_TRANSFORM_O(s, state, elem) NDNBOOST_PP_SEQ_TRANSFORM_O_IM(s, NDNBOOST_PP_TUPLE_REM_3 state, elem)
#    define NDNBOOST_PP_SEQ_TRANSFORM_O_IM(s, im, elem) NDNBOOST_PP_SEQ_TRANSFORM_O_I(s, im, elem)
# else
#    define NDNBOOST_PP_SEQ_TRANSFORM_O(s, state, elem) NDNBOOST_PP_SEQ_TRANSFORM_O_I(s, NDNBOOST_PP_TUPLE_ELEM(3, 0, state), NDNBOOST_PP_TUPLE_ELEM(3, 1, state), NDNBOOST_PP_TUPLE_ELEM(3, 2, state), elem)
# endif
#
# define NDNBOOST_PP_SEQ_TRANSFORM_O_I(s, op, data, res, elem) (op, data, res (op(s, data, elem)))
#
# /* NDNBOOST_PP_SEQ_TRANSFORM_S */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_SEQ_TRANSFORM_S(s, op, data, seq) NDNBOOST_PP_SEQ_TAIL(NDNBOOST_PP_TUPLE_ELEM(3, 2, NDNBOOST_PP_SEQ_FOLD_LEFT_ ## s(NDNBOOST_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# else
#    define NDNBOOST_PP_SEQ_TRANSFORM_S(s, op, data, seq) NDNBOOST_PP_SEQ_TRANSFORM_S_I(s, op, data, seq)
#    define NDNBOOST_PP_SEQ_TRANSFORM_S_I(s, op, data, seq) NDNBOOST_PP_SEQ_TAIL(NDNBOOST_PP_TUPLE_ELEM(3, 2, NDNBOOST_PP_SEQ_FOLD_LEFT_ ## s(NDNBOOST_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# endif
#
# endif
